#ifndef LEXER_H
#define LEXER_H

#include "token.h"

struct Lexer {
	char * str;
	size_t idx;
	size_t len;
};

char get_next(struct Lexer *);
char get_prev(struct Lexer *);

#endif
