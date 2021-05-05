#ifndef LIST_H
#define LIST_H

#include "token.h"

struct ListElement {
	struct ListElement * prev;
	struct ListElement * next;
	struct Token value;
};

struct List {
	struct ListElement * first;
	struct ListElement * last;
	unsigned int length;
};

// MEMORY ALLOC/DEALLOC
struct List * listMake(void);
void listDestroy(struct List *);

// ADDING/REMOVING ELEMENTS
void listPushFront(struct List *, struct Token);
void listPushBack(struct List *, struct Token);
struct Token listPopFront(struct List *);
struct Token listPopBack(struct List *);

/* ITERATION
 * If you want to iterate through a list,
 * be sure to do it like this:
 *
 * 1. front to back
 * for (struct ListElement * it = list->first; it != NULL; it = it->next)
 *
 * 2. back to front
 * for (struct ListElement * it = list->last; it != NULL; it = it->prev)
 * 
 * If you choose not to take this word of advice seriously,
 * you will burn in hell, surrounded by segmentation faults.
 */

#endif
