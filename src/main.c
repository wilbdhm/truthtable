#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "token.h"
#include "list.h"

const char * ALLOC_FAIL_MSG = "Failed to allocate memory.\n";

int main(int argc, char * argv[]) {
    char * buffer = NULL;
    buffer = (char *)malloc(8192 * sizeof(char));

    if (!buffer) {
        fputs(ALLOC_FAIL_MSG, stderr);
        return -1;
    }

    if (fgets(buffer, 8192, stdin) == NULL)
        return 1;

    for (int i = 0, j = 0, l = strlen(buffer); i <= l; i++) {
        if (!isspace(buffer[i])) {
            buffer[j] = buffer[i];
            j++;
        }
    }

    size_t len = strlen(buffer); // might be useful later on
    buffer = (char *)realloc(buffer, len * sizeof(char));
    struct Token * lex_out = (struct Token *)malloc(len * sizeof(struct Token));

    if (!lex_out) {
        fputs(ALLOC_FAIL_MSG, stderr);
        return -1;
    }

    // lexing
    {
        size_t lex_i = 0;
        size_t tok_i = 0;
        int l_err = 0;

        for (; lex_i < len && l_err == 0; ++lex_i) {
            switch (buffer[lex_i]) {
                case '(':
                    lex_out[tok_i].type = TOK_LB;
                    break;

                case ')':
                    lex_out[tok_i].type = TOK_RB;
                    break;

                case '~':
                    lex_out[tok_i].type = TOK_NOT;
                    break;

                case '&':
                    lex_out[tok_i].type = TOK_AND;
                    break;

                case '|':
                    lex_out[tok_i].type = TOK_OR;
                    break;

                case '=':
                    if (lex_i + 1 >= len)
                        l_err++;
                    else if (buffer[lex_i + 1] == '>') {
                        lex_out[tok_i].type = TOK_IMPL;
                        lex_i++;
                    }
                    else
                        l_err++;

                    break;

                case '<':
                    if (lex_i + 2 >= len)
                        l_err++;
                    else if (buffer[lex_i + 1] == '=' && buffer[lex_i + 2] == '>') {
                        lex_out[tok_i].type = TOK_EQUIV;
                        lex_i += 2;
                    }
                    else
                        l_err++;

                    break;

                case '0':
                    lex_out[tok_i].type = TOK_ZERO;
                    break;

                case '1':
                    lex_out[tok_i].type = TOK_ONE;
                    break;

                default:
                    if (isalpha(buffer[lex_i]) && islower(buffer[lex_i])) {
                        lex_out[tok_i].type = TOK_VAR;
                        lex_out[tok_i].value = buffer[lex_i];
                    }
                    else
                        l_err++;

                    break;
            }

            ++tok_i;
        }

        if (l_err != 0) {
            fprintf(stderr, "Unknown symbol: %c\n", buffer[lex_i]);
            free(lex_out);
            free(buffer);
            return -1;
        }
    }
    free(lex_out);
    free(buffer);
    return 0;
}
