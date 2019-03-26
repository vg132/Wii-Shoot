#include "List.h"

int ShootListSize(const shoot_node *list)
{
	const shoot_node *node=list;
	while(node->previous!=NULL)
	{
		node=node->previous;
	}
	int count=0;
	while(node!=NULL)
	{
		count++;
		node=node->next;
	}
	return count;
}