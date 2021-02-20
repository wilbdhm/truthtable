#ifndef TOKEN_H
#define TOKEN_H

enum TOKEN {
	TOK_LB, TOK_RB,
	TOK_NOT,
	TOK_AND, TOK_OR, TOK_IMPL, TOK_EQUIV,
	TOK_VAR
};

struct Token {
	enum TOKEN type;
	char value;
};

#endif
