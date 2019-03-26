#ifndef __LIST_H__
#define __LIST_H__

#include <stdio.h>

struct shoot_node
{
	void *data;
	shoot_node *next;
	shoot_node *previous;

	shoot_node(void *d, shoot_node *p)
	{
		data=d;
		previous=p;
		next=NULL;
	}
};

extern int ShootListSize(const shoot_node *list);

#endif