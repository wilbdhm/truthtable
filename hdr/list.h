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
	size_t length;
};

// MEMORY ALLOC/DEALLOC
struct List * listMake(void);
void listDestroy(struct List *);

// ADDING/REMOVING ELEMENTS
void listPushFront(struct List *, struct Token);
void listPushBack(struct List *, struct Token);
void listPopFront(struct List *, struct Token);
void listPopBack(struct List *, struct Token);

#endif
