#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "token.h"

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

        for (; lex_i < len; ++lex_i) {
            switch (buffer[lex_i]) {
                case '(':
                    lex_out[tok_i] = TOK_LB;
                    break;

                case ')':
                    lex_out[tok_i] = TOK_RB;
                    break;

                case '~':
                    lex_out[tok_i] = TOK_NOT;
                    break;

                case '&':
                    lex_out[tok_i] = TOK_AND;
                    break;

                case '|':
                    lex_out[tok_i] = TOK_OR;
                    break;

                case '=':
                    if (lex_i >= len) {
                        // big error
                        // crash or something
                        // goto for now
                        fprintf(stderr, "OVERFLOW AAAAAAA\n");
                        goto panic;
                    }

                    break;

                case '<':
                    break;

                default:
                    break;
            }

            ++tok_i;
        }
    }
panic:
    free(lex_out);
    free(buffer);
    return 0;
}
