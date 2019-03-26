
#include <grrlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <fat.h>
#include <wiiuse/wpad.h>

#include "Common.h"
#include "List.h"
#include "Graphics/BMfont5.h"
#include "Graphics/century_gothic_22.h"
#include "Graphics/background.h"
#include "Graphics/target.h"
#include "Graphics/sight_p1.h"
#include "Graphics/sight_p2.h"
#include "Graphics/sight_p3.h"
#include "Graphics/sight_p4.h"

#include "Graphics/plate_p1.h"
#include "Graphics/plate_p2.h"
#include "Graphics/plate_p3.h"
#include "Graphics/plate_p4.h"

#include "Graphics/logo.h"
#include "Graphics/grrlib_logo.h"
#include "Graphics/url.h"
#include "Graphics/url_white.h"

#include "Graphics/menu_background.h"
#include "Graphics/title.h"

#include "Graphics/button_1player.h"
#include "Graphics/button_2player.h"
#include "Graphics/button_3player.h"
#include "Graphics/button_4player.h"
#include "Graphics/button_ok.h"

#include "Graphics/button_1player_selected.h"
#include "Graphics/button_2player_selected.h"
#include "Graphics/button_3player_selected.h"
#include "Graphics/button_4player_selected.h"
#include "Graphics/button_ok_selected.h"

// Global variables
GRRLIB_texImg *tex_background;
GRRLIB_texImg *tex_target;
GRRLIB_texImg *tex_target_hit;
GRRLIB_texImg *tex_logo;
GRRLIB_texImg *tex_url;
GRRLIB_texImg *tex_url_white;
GRRLIB_texImg *tex_GRRLogo;

GRRLIB_texImg *tex_sight[PLAYERS];
GRRLIB_texImg *tex_plate[PLAYERS];


GRRLIB_texImg *tex_font;
GRRLIB_texImg *tex_FontCenturyGothic22;

GXRModeObj *renderMode;

shoot_node *m_TargetList=NULL;

shoot_player players[4];

u32 m_frame_ctr;

shoot_state m_state;

u32 color=RGBA(255,255,255,0);
u32 logoFrame=0;
u32 nextSwitch=0;
bool showText=true;

// Main menu variables
s8 m_MainMenuActiveItem=-1;
bool m_MainMenuActiveItemChanged=false;
GRRLIB_texImg *tex_button[PLAYERS];
GRRLIB_texImg *tex_selected_button[PLAYERS];
GRRLIB_texImg *tex_menuBackground;
GRRLIB_texImg *tex_title;

// Game over variables
GRRLIB_texImg *tex_ButtonOK;
GRRLIB_texImg *tex_ButtonOKSelected;
bool m_GameOverButtonActiveChanged=false;
bool m_GameOverButtonActive=false;

void DrawDebug(void)
{
	if(m_TargetList!=NULL)
	{
		GRRLIB_Printf(50,50,tex_font,GRRLIB_WHITE,1.0f,"Next is not NULL, size: %d",ShootListSize(m_TargetList));
	}
}

void LoadTextures(void)
{
	tex_background = GRRLIB_LoadTexture(background);
	tex_target = GRRLIB_LoadTexture(target);
	tex_target_hit=GRRLIB_CreateEmptyTexture(tex_target->w, tex_target->h);
	tex_logo=GRRLIB_LoadTexture(logo);
	tex_GRRLogo=GRRLIB_LoadTexture(grrlib_logo);
	tex_url=GRRLIB_LoadTexture(url);
	tex_url_white=GRRLIB_LoadTexture(url_white);
	tex_title=GRRLIB_LoadTexture(title);
	tex_ButtonOK = GRRLIB_LoadTexture(button_ok);
	tex_ButtonOKSelected = GRRLIB_LoadTexture(button_ok_selected);
	tex_menuBackground=GRRLIB_LoadTexture(menu_background);
	GRRLIB_BMFX_Sepia(tex_target, tex_target_hit);
  GRRLIB_FlushTex(tex_target_hit);

	tex_sight[0] = GRRLIB_LoadTexture(sight_p1);
	tex_plate[0] = GRRLIB_LoadTexture(plate_p1);
	tex_button[0]=GRRLIB_LoadTexture(button_1player);
	tex_selected_button[0]=GRRLIB_LoadTexture(button_1player_selected);
	tex_sight[1] = GRRLIB_LoadTexture(sight_p2);
	tex_plate[1] = GRRLIB_LoadTexture(plate_p2);
	tex_button[1]=GRRLIB_LoadTexture(button_2player);
	tex_selected_button[1]=GRRLIB_LoadTexture(button_2player_selected);
	tex_sight[2] = GRRLIB_LoadTexture(sight_p3);
	tex_plate[2] = GRRLIB_LoadTexture(plate_p3);
	tex_button[2]=GRRLIB_LoadTexture(button_3player);
	tex_selected_button[2]=GRRLIB_LoadTexture(button_3player_selected);
	tex_sight[3] = GRRLIB_LoadTexture(sight_p4);
	tex_plate[3] = GRRLIB_LoadTexture(plate_p4);
	tex_button[3]=GRRLIB_LoadTexture(button_4player);
	tex_selected_button[3]=GRRLIB_LoadTexture(button_4player_selected);

	tex_font = GRRLIB_LoadTexture(BMfont5);
	GRRLIB_InitTileSet(tex_font, 8, 16, 0);
	tex_FontCenturyGothic22=GRRLIB_LoadTexture(century_gothic_22);
	GRRLIB_InitTileSet(tex_FontCenturyGothic22,27,36,0);
}

void UnloadTextures(void)
{
	GRRLIB_FreeTexture(tex_background);
	GRRLIB_FreeTexture(tex_target);
	GRRLIB_FreeTexture(tex_logo);
	GRRLIB_FreeTexture(tex_GRRLogo);
	GRRLIB_FreeTexture(tex_url);
	GRRLIB_FreeTexture(tex_url_white);
	GRRLIB_FreeTexture(tex_menuBackground);
	GRRLIB_FreeTexture(tex_title);
	GRRLIB_FreeTexture(tex_ButtonOK);
	GRRLIB_FreeTexture(tex_ButtonOKSelected);
	for(int i=0;i<PLAYERS;i++)
	{
		GRRLIB_FreeTexture(tex_sight[i]);
		GRRLIB_FreeTexture(tex_plate[i]);
	}

	GRRLIB_FreeTexture(tex_font);
	GRRLIB_FreeTexture(tex_FontCenturyGothic22);
}

void Initialize(void)
{
	srand(time(NULL));
	GRRLIB_Init();
	WPAD_Init();
	WPAD_SetVRes(WPAD_CHAN_ALL, 640, 480);
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetDataFormat(WPAD_CHAN_1, WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetDataFormat(WPAD_CHAN_2, WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetDataFormat(WPAD_CHAN_3, WPAD_FMT_BTNS_ACC_IR);

	renderMode = VIDEO_GetPreferredMode(NULL);
	LoadTextures();
}

void Uninitialize(void)
{
	GRRLIB_Exit();
	UnloadTextures();
}

void DrawSight(void)
{
	if(m_state==SHOOT_MAINMENU || m_state==SHOOT_GAMEOVER)
	{
		ir_t ir;
		WPAD_IR(players[0].controller, &ir);
		if(ir.valid)
		{
			GRRLIB_DrawImg(ir.x,ir.y,tex_sight[0],0,1.0f,1.0f,GRRLIB_WHITE);
		}
	}
	else if(m_state==SHOOT_PLAYING)
	{
		for(int i=0;i<PLAYERS;i++)
		{
			if(players[i].isPlaying)
			{
				ir_t ir;
				WPAD_IR(players[i].controller, &ir);
				if(ir.valid)
				{
					GRRLIB_DrawImg(ir.x,ir.y,tex_sight[i],0,1.0f,1.0f,GRRLIB_WHITE);
				}
			}
		}
	}
}

void DrawInfo(void)
{
	int x=50;
	int y=330;
	for(int i=0;i<PLAYERS;i++)
	{
		shoot_player player=players[i];
		GRRLIB_DrawImg(x,y,tex_plate[i],0,0.5f,0.40f,GRRLIB_WHITE);
		if(player.isPlaying)
		{
			GRRLIB_Printf(x+10,y+10,tex_font,GRRLIB_BLACK,1.0f,player.name);
			GRRLIB_Printf(x+10,y+25,tex_font,GRRLIB_BLACK,1.0f,"Points:");
			GRRLIB_Printf(x+12,y+40,tex_font,GRRLIB_BLACK,1.0f,"%d",player.points);
			GRRLIB_Printf(x+10,y+55,tex_font,GRRLIB_BLACK,1.0f,"Bullets:");
			GRRLIB_Printf(x+10,y+70,tex_font,GRRLIB_BLACK,1.0f,"%d",player.bullets);
		}
		else
		{
			if(showText)
			{
				GRRLIB_Printf(x+10,y+10,tex_font,GRRLIB_BLACK,1.0f,"Press A to");
				GRRLIB_Printf(x+10,y+25,tex_font,GRRLIB_BLACK,1.0f,"join game!");
			}
			if(m_frame_ctr>nextSwitch)
			{
				showText=!showText;
				nextSwitch=m_frame_ctr+60;
			}
		}
		x+=140;
	}
}

void DrawLogo(void)
{
	logoFrame++;
	if(logoFrame>300)
	{
		GRRLIB_FillScreen(color);
	}
	else
	{
		GRRLIB_FillScreen(GRRLIB_WHITE);
	}
	if(logoFrame>30)
	{
		GRRLIB_DrawImg(140,202,tex_logo,0,1.0f,1.0f,color);
		GRRLIB_DrawImg(256,400,tex_url,0,1.0f,1.0f,color);
		if(logoFrame<90)
		{
			color=RGBA(R(color),G(color),B(color),A(color)+4);
		}
		else if(logoFrame>300 && logoFrame<422)
		{
			color=RGBA(R(color)-2,G(color)-2,B(color)-2,A(color)-2);
		}
		if(logoFrame>422)
		{
			m_state=SHOOT_MAINMENU;
		}
	}
}

void DrawMainMenuItem(u16 x, u16 y, u8 id)
{
	ir_t ir;
	WPAD_IR(WPAD_CHAN_0,&ir);

	if(GRRLIB_PtInRect(x,y,tex_button[id]->w*0.5f,tex_button[id]->h*0.5f,ir.x,ir.y))
	{
		GRRLIB_DrawImg(x,y,tex_selected_button[id],0,0.5f,0.5f,GRRLIB_WHITE);
		if(m_MainMenuActiveItem!=id)
		{
			m_MainMenuActiveItem=id;
			m_MainMenuActiveItemChanged=true;
		}
	}
	else
	{
		GRRLIB_DrawImg(x,y,tex_button[id],0,0.5f,0.5f,GRRLIB_WHITE);
		if(m_MainMenuActiveItem==id)
		{
			m_MainMenuActiveItem=-1;
			m_MainMenuActiveItemChanged=false;
		}
	}
}

void DrawMainMenu()
{
	GRRLIB_FillScreen(GRRLIB_BLACK);
	GRRLIB_DrawImg(0,0,tex_background,0,1.0f,1.0f,GRRLIB_WHITE);

	GRRLIB_DrawImg(0,0,tex_menuBackground,0,10.0f,10.0f,GRRLIB_WHITE);
	GRRLIB_DrawImg(164,75,tex_title,0,1.0f,1.0f,GRRLIB_WHITE);
	GRRLIB_DrawImg(480,400,tex_GRRLogo,0,0.5f,0.5f,GRRLIB_WHITE);
	GRRLIB_DrawImg(256,400,tex_url_white,0,1.0f,1.0f,GRRLIB_WHITE);
	DrawMainMenuItem(118,230,0);
	DrawMainMenuItem(330,230,1);
	DrawMainMenuItem(118,310,2);
	DrawMainMenuItem(330,310,3);

	DrawSight();
}

void DrawGameOver(void)
{
	GRRLIB_FillScreen(GRRLIB_BLACK);
	GRRLIB_DrawImg(0,0,tex_background,0,1.0f,1.0f,GRRLIB_WHITE);
	GRRLIB_DrawImg(0,0,tex_menuBackground,0,10.0f,10.0f,GRRLIB_WHITE);

	Shoot_Printf(320,100,SHOOT_ALIGN_CENTER,tex_FontCenturyGothic22,GRRLIB_WHITE,1.0f,"Game Over");

	char buffer[100];
	int yPos=170;
	for(int i=0;i<PLAYERS;i++)
	{
		if(players[i].isPlaying)
		{
			sprintf(buffer,"%s: %i",players[i].name,players[i].points);
			Shoot_Printf(320,yPos,SHOOT_ALIGN_CENTER,tex_FontCenturyGothic22,GRRLIB_WHITE,0.5f,buffer);
			yPos+=30;
		}
	}

	ir_t ir;
	WPAD_IR(WPAD_CHAN_0,&ir);
	if(GRRLIB_PtInRect(320-((tex_ButtonOK->w*0.5f)/2),290,tex_ButtonOK->w*0.5f,tex_ButtonOK->h*0.5f,ir.x,ir.y))
	{
		Shoot_DrawImg(320,290,SHOOT_ALIGN_CENTER,tex_ButtonOKSelected,0,0.5f,0.5f,GRRLIB_WHITE);
		if(!m_GameOverButtonActive)
		{
			m_GameOverButtonActive=true;
			m_GameOverButtonActiveChanged=true;
		}
	}
	else
	{
		Shoot_DrawImg(320,290,SHOOT_ALIGN_CENTER,tex_ButtonOK,0,0.5f,0.5f,GRRLIB_WHITE);
		if(m_GameOverButtonActive)
		{
			m_GameOverButtonActive=false;
			m_GameOverButtonActiveChanged=false;
		}
	}
	DrawSight();
}

void DrawGame(void)
{
	GRRLIB_FillScreen(GRRLIB_BLACK);
	GRRLIB_DrawImg(0,0,tex_background,0,1.0f,1.0f,GRRLIB_WHITE);
	shoot_node *node=m_TargetList;
	while(node!=NULL)
	{
		shoot_target *target=(shoot_target*)node->data;
		GRRLIB_DrawImg(target->pos.x,target->pos.y,target->texture,0,target->size,target->size,GRRLIB_WHITE);
		node=node->next;
	}
	DrawInfo();
	DrawSight();
}

void Draw(void)
{
	if(m_state==SHOOT_INTRO)
	{
		DrawLogo();
	}
	else if(m_state==SHOOT_MAINMENU)
	{
		DrawMainMenu();
	}
	else if(m_state==SHOOT_GAMEOVER)
	{
		DrawGameOver();
	}
	else
	{
		DrawGame();
	}
	DrawDebug();
	GRRLIB_Render();
}

void ResetPlayer(int id)
{
	players[id].controller=id;
	if(players[id].name!=NULL)
	{
		delete players[id].name;
	}
	players[id].name=new char[9];
	sprintf(players[id].name,"Player %i",(id+1));
	players[id].points=0;
	players[id].bullets=BULLETS;
	if(players[id].rumble!=NULL)
	{
		delete players[id].rumble;
	}
	players[id].rumble=NULL;
}

void SetupTargets(void)
{
	int x=100;
	int y=50;
	int speed=4;
	float size=0.15f;
	int value=25;

	shoot_node *currentNode=m_TargetList;

	for(int i=0;i<TARGETS;i++)
	{
		if(i>0 && i%3==0)
		{
			size+=0.15f;
			x=100;
			y+=100;
			speed--;
			value-=10;
		}
		shoot_rect rect;
		rect.height=20;
		rect.width=20;
		rect.x=x;
		rect.y=y;
		x=x+100;
		shoot_target *target=new shoot_target();
		target->pos=rect;
		target->active=true;
		target->lastMove=0;
		target->texture=tex_target;
		target->value=value;
		target->speed=(shoot_speed)speed;
		target->size=size;

		if(currentNode==NULL)
		{
			currentNode=new shoot_node(target,NULL);
			m_TargetList=currentNode;
		}
		else
		{
			currentNode->next=new shoot_node(target,NULL);
			currentNode=currentNode->next;
		}
	}
}

shoot_target* CheckHit(int x, int y)
{
	shoot_node *node=m_TargetList;
	while(node!=NULL)
	{
		shoot_target *target=(shoot_target*)node->data;
		if(target->active && GRRLIB_PtInRect(target->pos.x,target->pos.y,target->texture->w*target->size,target->texture->h*target->size,x,y))
		{
			return target;
		}
		node=node->next;
	}
	return NULL;
}

void MoveTargets(void)
{
	shoot_node* node=m_TargetList;
	while(node!=NULL)
	{
		shoot_target* target=(shoot_target*)node->data;
		target->pos.x+=target->speed;
		target->lastMove=m_frame_ctr;
		if(target->pos.x>700)
		{
			target->active=true;
			target->texture=tex_target;
			target->pos.x=-50;
		}
		node=node->next;
	}
	//switch(target->speed)
	//{
	//	case SHOOT_SPEED_SLOW:
	//		target->pos.x+=1;

	//		break;
	//	case SHOOT_SPEED_MEDIUM:
	//		target->pos.x+=2;
	//		break;
	//	case SHOOT_SPEED_FAST:
	//		target->pos.x+=3;
	//		break;
	//}
}

void DoRumble(shoot_player *player)
{
	if(player->rumble!=NULL)
	{
		WPAD_Rumble(player->controller,1);
		player->rumble->timeLeft--;
		if(player->rumble->timeLeft<=0)
		{
			delete player->rumble;
			player->rumble=NULL;
		}
	}
	else
	{
		WPAD_Rumble(player->controller,0);
	}
}

void CheckInput(shoot_player *player)
{
	ir_t ir;
	WPAD_IR(player->controller,&ir);
	u32 pressed = WPAD_ButtonsDown(player->controller);
	if ( pressed & WPAD_BUTTON_HOME )
	{
		exit(0);
	}
	if(pressed & WPAD_BUTTON_A && !player->isPlaying)
	{
		player->isPlaying=true;
		//GRRLIB_ScrShot("sd:/wii_shoot2.png");
	}
	if(m_state==SHOOT_PLAYING)
	{
		if(pressed & WPAD_BUTTON_B && player->bullets>0)
		{
			player->rumble=new shoot_rumble(4);
			shoot_target* target=CheckHit(ir.x,ir.y);
			if(target!=NULL)
			{
				target->texture=tex_target_hit;
				player->points+=target->value;
				target->active=false;
			}
			else
			{
				player->bullets--;
			}
		}
	}
	else if(m_state==SHOOT_MAINMENU)
	{
		if(m_MainMenuActiveItemChanged)
		{
			m_MainMenuActiveItemChanged=false;
			player->rumble=new shoot_rumble(2);
		}
		if(pressed & WPAD_BUTTON_B && m_MainMenuActiveItem>=0)
		{
			m_state=SHOOT_PLAYING;
			if(m_MainMenuActiveItem>=0)
			{
				ResetPlayer(0);
				players[0].isPlaying=true;
			}
			if(m_MainMenuActiveItem>=1)
			{
				ResetPlayer(1);
				players[1].isPlaying=true;
			}
			if(m_MainMenuActiveItem>=2)
			{
				ResetPlayer(2);
				players[2].isPlaying=true;
			}
			if(m_MainMenuActiveItem>=3)
			{
				ResetPlayer(3);
				players[3].isPlaying=true;
			}
		}
	}
	else if(m_state==SHOOT_GAMEOVER)
	{
		if(m_GameOverButtonActiveChanged)
		{
			m_GameOverButtonActiveChanged=false;
			player->rumble=new shoot_rumble(2);
		}
		if(pressed & WPAD_BUTTON_B && m_GameOverButtonActive)
		{
			m_state=SHOOT_MAINMENU;
		}
	}
}

void CheckGameOver(void)
{
	for(int i=0;i<PLAYERS;i++)
	{
		if(players[i].isPlaying && players[i].bullets>0)
		{
			return;
		}
	}
	m_state=SHOOT_GAMEOVER;
}

void SetupPlayers()
{
	ResetPlayer(0);
	ResetPlayer(1);
	ResetPlayer(2);
	ResetPlayer(3);
}

int main(int argc, char **argv)
{
	Initialize();
	SetupPlayers();
	SetupTargets();
	m_frame_ctr=0;
	m_state=SHOOT_INTRO;
	while(1) {
		m_frame_ctr++;
		WPAD_ScanPads();
		if(m_state==SHOOT_PLAYING)
		{
			for(int i=0;i<PLAYERS;i++)
			{
				//if(players[i].isPlaying)
				{
					CheckInput(&players[i]);
					DoRumble(&players[i]);
					CheckGameOver();
				}
			}
		}
		else if(m_state==SHOOT_MAINMENU)
		{
			CheckInput(&players[0]);
			DoRumble(&players[0]);
		}
		else if(m_state==SHOOT_GAMEOVER)
		{
			CheckInput(&players[0]);
			DoRumble(&players[0]);
		}
		MoveTargets();
		Draw();
	}
	Uninitialize();
	return 0;
}

/*

Timer för spelet.

*/