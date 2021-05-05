#include "list.h"
#include <stdlib.h>

// MEMORY ALLOC/DEALLOC
struct List * listMake() {
    // no checking if allocation was successful because fuck you
    return (struct List *)calloc(1, sizeof(struct List));
}

void listDestroy(struct List * list) {
    while (list->length > 0)
        listPopFront(list);

    free(list);
}

// ADDING/REMOVING ELEMENTS
void listPushFront(struct List * list, struct Token token) {
    struct ListElement * el;
    el = (struct ListElement *)malloc(sizeof(struct ListElement));
    el->value = token;
    el->prev = NULL;

    if (list->length > 0) {
        el->next = list->first;
        list->first->prev = el;
    }
    else
        el->next = NULL;

    list->first = el;
    list->length++;
}

void listPushBack(struct List * list, struct Token token) {
    struct ListElement * el;
    el = (struct ListElement *)malloc(sizeof(struct ListElement));
    el->value = token;
    el->next = NULL;

    if (list->length > 0) {
        el->prev = list->last;
        list->last->next = el;
    }
    else
        el->prev = NULL;

    list->first = el;
    list->length++;
}

struct Token listPopFront(struct List * list) {
    if (list->length == 0) {
        exit(-1); // panic
    }

    struct Token tok = list->first->value;

    if (list->length == 1) {
        free(list->first);
        list->first = NULL;
        list->last = NULL;
    }
    else {
        list->first = list->first->next;
        free(list->first->prev);
        list->first->prev = NULL;
    }

    list->length--;
    return tok;
}

struct Token listPopBack(struct List * list) {
    if (list->length == 0) {
        exit(-1); // panic
    }

    struct Token tok = list->last->value;

    if (list->length == 1) {
        free(list->last);
        list->first = NULL;
        list->last = NULL;
    }
    else {
        list->last = list->last->prev;
        free(list->last->next);
        list->last->next = NULL;
    }

    list->length--;
    return tok;
}
