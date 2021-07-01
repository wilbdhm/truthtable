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
    struct List * infix = listMake();
    struct List * operators = listMake();
    struct List * postfix = listMake();

    if (!infix) {
        fputs(ALLOC_FAIL_MSG, stderr);
        return -1;
    }

    if (!operators) {
        fputs(ALLOC_FAIL_MSG, stderr);
        return -1;
    }

    if (!postfix) {
        fputs(ALLOC_FAIL_MSG, stderr);
        return -1;
    }

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

    for (size_t i = 0, t_len = len; i < t_len; i++) {
        if (buffer[i] == '~')
            len++;
    }

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

        for (; lex_i < len && l_err == 0 && buffer[lex_i]; ++lex_i) {
            switch (buffer[lex_i]) {
                case '(':
                    lex_out[tok_i].type = TOK_LB;
                    break;

                case ')':
                    lex_out[tok_i].type = TOK_RB;
                    break;

                case '~':
                    if (lex_i + 1 < len && buffer[lex_i + 1] != '~') {
                        lex_out[tok_i++].type = TOK_ZERO;
                        lex_out[tok_i].type = TOK_NOT;
                    }
                    else {
                        fprintf(stderr, "Wrong use of '~'\n");
                        free(lex_out);
                        free(buffer);
                        return -1;
                    }

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

        len = tok_i;
    }
    //infix to postfix
    {
        for (size_t tok_i = 0; tok_i < len; ++tok_i)
            listPushBack(infix, lex_out[tok_i]);

        for (struct ListElement * it = infix->first; it != NULL; it = it->next) {
            switch (it->value.type) {
                case TOK_VAR:
                case TOK_ZERO:
                case TOK_ONE:
                    listPushFront(postfix, it->value);
                    break;

                case TOK_RB:
                    while (operators->length && operators->first->value.type != TOK_LB)
                        listPushFront(postfix, listPopFront(operators));

                    if (operators->length)
                        listPopFront(operators);
                    else {
                        fprintf(stderr, "Missing left bracket!\n");
                        return 1;
                    }

                    break;

                case TOK_LB:
                    listPushFront(operators, it->value);
                    break;

                default:
                    if (operators->first && operators->first->value.type == TOK_NOT)
                        listPushFront(postfix, listPopFront(operators));

                    listPushFront(operators, it->value);
                    break;
            }
        }

        for (struct ListElement * it = operators->first; it != NULL; it = it->next) {
            if (it->value.type == TOK_LB) {
                fprintf(stderr, "Missing right bracket!\n");
                return 1;
            }

            listPushFront(postfix, it->value);
        }
    }
    // evaluation
    {
        bool var_table[26] = { 0 }; // 26 lowercase letters
        bool var_used[26] = { 0 };
        unsigned int var_count = 0;

        for (struct ListElement * it = postfix->first; it != NULL; it = it->next) {
            if (it->value.type != TOK_VAR);
            else if (it->value.value >= 'a' && it->value.value <= 'z')
                var_used[it->value.value - 'a'] = true;
            else {
                fprintf(stderr, "Variable parse error: %c (%u)\n", it->value.value,
                        it->value.value);
                goto death;
            }
        }

        for (int i = 0; i < 26; ++i) {
            if (var_used[i]) {
                var_count++;
                printf("%c | ", i + 'a');
            }
        }

        puts("");

        // generating all possible arrangements of 0s and 1s
        // by scanning binary digits of numbers from 0 to 2^var_count - 1
        for (int i = 0; i < (1 << var_count); ++i) {
            for (int j = var_count - 1, k = 0; k < 26; ++k) {
                if (var_used[k]) {
                    var_table[k] = (i & (1 << j)) >> j;
                    j--;

                    if (j < 0)
                        break;
                }
            }

            for (int j = 0; j < 26; ++j) {
                if (var_used[j])
                    printf("%u | ", var_table[j]);
            }

            puts("");
        }
    }
death:
    listDestroy(infix);
    listDestroy(operators);
    listDestroy(postfix);
    free(lex_out);
    free(buffer);
    return 0;
}
