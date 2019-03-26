#ifndef __Common_h__
#define __Common_h__

#include <grrlib.h>

// RGBA Colors
#define GRRLIB_BLACK   0x000000FF
#define GRRLIB_MAROON  0x800000FF
#define GRRLIB_GREEN   0x008000FF
#define GRRLIB_OLIVE   0x808000FF
#define GRRLIB_NAVY    0x000080FF
#define GRRLIB_PURPLE  0x800080FF
#define GRRLIB_TEAL    0x008080FF
#define GRRLIB_GRAY    0x808080FF
#define GRRLIB_SILVER  0xC0C0C0FF
#define GRRLIB_RED     0xFF0000FF
#define GRRLIB_LIME    0x00FF00FF
#define GRRLIB_YELLOW  0xFFFF00FF
#define GRRLIB_BLUE    0x0000FFFF
#define GRRLIB_FUCHSIA 0xFF00FFFF
#define GRRLIB_AQUA    0x00FFFFFF
#define GRRLIB_WHITE   0xFFFFFFFF

#define TARGETS 9
#define PLAYERS 4
#define BULLETS 10

enum shoot_speed
{
	SHOOT_SPEED_SLOW=1,
	SHOOT_SPEED_MEDIUM=2,
	SHOOT_SPEED_FAST=3,
};

struct shoot_rumble
{
	u32 timeLeft;

	shoot_rumble(u32 frames)
	{
		timeLeft=frames;
	}
};

struct shoot_rect
{
	int x;
	int y;
	int width;
	int height;
};

struct shoot_target
{
	shoot_rect pos;
	shoot_speed speed;
	int value;
	f32 size;
	bool active;
	u32 lastMove;
	GRRLIB_texImg *texture;	
};

struct shoot_player
{
	shoot_player()
	{
		name=NULL;
		rumble=NULL;
	}
	char *name;
	int points;
	int bullets;
	bool isPlaying;
	int controller;
	shoot_rumble *rumble;
};

enum shoot_state
{
	SHOOT_INTRO=1,
	SHOOT_MAINMENU=2,
	SHOOT_PLAYING=3,
	SHOOT_GAMEOVER=4
};

enum shoot_align
{
	SHOOT_ALIGN_LEFT=1,
	SHOOT_ALIGN_CENTER=2,
	SHOOT_ALIGN_RIGHT=3
};

void Shoot_DrawImg(const f32 xpos, const f32 ypos, shoot_align align, const GRRLIB_texImg *tex, const f32 degrees, const f32 scaleX, const f32 scaleY, const u32 color)
{
	if(align==SHOOT_ALIGN_LEFT)
	{
		GRRLIB_DrawImg(xpos,ypos,tex,degrees,scaleX,scaleY,color);
	}
	else if(align==SHOOT_ALIGN_CENTER)
	{
		GRRLIB_DrawImg(xpos-((tex->w*scaleX)/2),ypos,tex,degrees,scaleX,scaleY,color);
	}
	else if(align==SHOOT_ALIGN_RIGHT)
	{
		GRRLIB_DrawImg(xpos-(tex->w*scaleX),ypos,tex,degrees,scaleX,scaleY,color);
	}
}

void Shoot_Printf(const f32 xpos, const f32 ypos, shoot_align align, const GRRLIB_texImg *tex, const u32 color, const f32 zoom, const char *text)
{
	if(align==SHOOT_ALIGN_LEFT)
	{
		GRRLIB_Printf(xpos,ypos,tex,color,zoom,text);
	}
	else if(align==SHOOT_ALIGN_CENTER)
	{
		GRRLIB_Printf(xpos-((strlen(text)*(tex->tilew*zoom))/2),ypos,tex,color,zoom,text);
	}
	else if(align==SHOOT_ALIGN_RIGHT)
	{
		GRRLIB_Printf(xpos-(strlen(text)*(tex->tilew*zoom)),ypos,tex,color,zoom,text);
	}
}

#endif