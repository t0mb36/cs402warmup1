/*
 * Implementation of Prof Cheng's my402list
 *
 */


#include <stdlib.h>
#include "my402list.h"

int My402ListInit(My402List *list){
	if (list == NULL){
		return FALSE;
	}
	list->num_members = 0;
	list->anchor.obj = NULL;
	list->anchor.next = &(list->anchor);
	list->anchor.prev = &(list->anchor);
	return TRUE;
}

int My402ListLength(My402List *list){
	return list->num_members;
}

int My402ListEmpty(My402List *list){
	return (list->num_members == 0);
}

My402ListElem *My402ListFirst(My402List *list){
	if (My402ListEmpty(list)){
		return NULL;
	}
	return list->anchor.next;
}

My402ListElem *My402ListLast(My402List *list){
	if (My402ListEmpty(list)){
		return NULL;
	}
	return list->anchor.prev;
}

My402ListElem *My402ListNext(My402List *list, My402ListElem *elem){
	if (elem->next == &(list->anchor)){
		return NULL;
	}
	return elem->next;
}

My402ListElem *My402ListPrev(My402List *list, My402ListElem *elem){
	if (elem->prev == &(list->anchor)){
		return NULL;
	}
	return elem->prev;
}

My402ListElem *My402ListFind(My402List *list, void *obj){
	My402ListElem *elem;

	for (elem = My402ListFirst(list); elem != NULL; elem = My402ListNext(list, elem)){
		if (elem->obj == obj){
			return elem;
		}
	}
	return NULL;
}

int My402ListAppend(My402List *list, void *obj){
	My402ListElem *elem = (My402ListElem *)malloc(sizeof(My402ListElem));
	if (elem == NULL){
		return FALSE;
	}
	
	elem->obj = obj;
	elem->prev = list->anchor.prev;
	elem->next = &(list->anchor);

	list->anchor.prev->next = elem;
	list->anchor.prev = elem;

	list->num_members++;
	return TRUE;
}

int My402ListPrepend(My402List *list, void *obj){
	My402ListElem *elem = (My402ListElem *)malloc(sizeof(My402ListElem));
	if (elem == NULL){
		return FALSE;
	}

	elem->obj = obj;
	elem->prev = &(list->anchor);
	elem->next = list->anchor.next;

	list->anchor.next->prev = elem;
	list->anchor.next = elem;

	list->num_members++;
	return TRUE;
}

void My402ListUnlink(My402List *list, My402ListElem *elem){
	elem->prev->next = elem->next;
	elem->next->prev = elem->prev;
	list->num_members--;
	free(elem);
}

void My402ListUnlinkAll(My402List *list){
	My402ListElem *elem = My402ListFirst(list);

	while (elem != NULL){
		My402ListElem *next = My402ListNext(list, elem);
		free(elem);
		elem = next;
	}
	list->num_members = 0;
	list->anchor.next = &(list->anchor);
	list->anchor.prev = &(list->anchor);
}

int My402ListInsertAfter(My402List *list, void *obj, My402ListElem *elem){
	if (elem == NULL){
		return My402ListAppend(list, obj);
	}

	My402ListElem *new = (My402ListElem *)malloc(sizeof(My402ListElem));
	if (new == NULL){
		return FALSE;
	}
	
	new->obj = obj;

	new->prev = elem;
	new->next = elem->next;
	
	elem->next->prev = new;
	elem->next = new;

	list->num_members++;
	return TRUE;
}

int My402ListInsertBefore(My402List *list, void *obj, My402ListElem *elem){
	if (elem == NULL){
		return My402ListPrepend(list, obj);
	}

	My402ListElem *new = (My402ListElem *)malloc(sizeof(My402ListElem));
	if (new == NULL){
		return FALSE;
	}

	new->obj = obj;

	new->next = elem;
	new->prev = elem->prev;

	elem->prev->next = new;
	elem->prev = new;

	list->num_members++;
	return TRUE;

}