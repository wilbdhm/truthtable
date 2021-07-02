#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "token.h"
#include "list.h"

const char * ALLOC_FAIL_MSG = "Failed to allocate memory.\n";

enum {
    ALLOC_ERROR = 1,
    INPUT_ERROR,
    LEX_ERROR,
    PARSE_ERROR,
    EVAL_ERROR
} ERROR_CODE;

int main(int argc, char * argv[]) {
    char * buffer = NULL;
    buffer = (char *)malloc(8192 * sizeof(char));
    struct List * infix = listMake();
    struct List * operators = listMake();
    struct List * postfix = listMake();
    struct List * rpn_stack = listMake();
    int ERR_EXIT = 0;

    if (!infix || !operators || !postfix || !rpn_stack || !buffer) {
        fputs(ALLOC_FAIL_MSG, stderr);
        return ALLOC_ERROR;
    }

    if (fgets(buffer, 8192, stdin) == NULL)
        return INPUT_ERROR;

    for (int i = 0, j = 0, l = strlen(buffer); i <= l; i++) {
        if (!isspace(buffer[i])) {
            buffer[j] = buffer[i];
            j++;
        }
    }

    size_t len = strlen(buffer); // might be useful later on
    size_t buflen = len;
    buffer = (char *)realloc(buffer, len * sizeof(char));
    /* Following error appeared in valgrind. Further investigation advised.
     * ==9155== Invalid read of size 1
     * ==9155==    at 0x1099F0: main (main.c:64)
     * ==9155==  Address 0x4a4a247 is 0 bytes after a block of size 7 alloc'd
     * ==9155==    at 0x484383F: realloc (vg_replace_malloc.c:1192)
     * ==9155==    by 0x109614: main (main.c:44)
     */

    for (size_t i = 0; i < buflen; i++) {
        if (buffer[i] == '~')
            len++;
    }

    struct Token * lex_out = (struct Token *)malloc(len * sizeof(struct Token));

    if (!lex_out) {
        fputs(ALLOC_FAIL_MSG, stderr);
        return ALLOC_ERROR;
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
                        ERR_EXIT = LEX_ERROR;
                        goto death;
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
            ERR_EXIT = LEX_ERROR;
            goto death;
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
                        ERR_EXIT = PARSE_ERROR;
                        goto death;
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
                ERR_EXIT = PARSE_ERROR;
                goto death;
            }
        }

        for (int i = 0; i < 26; ++i) {
            if (var_used[i]) {
                var_count++;
                printf("%c | ", i + 'a');
            }
        }

        // pretty-print expression
        for (int i = 0; i < buflen;) {
            switch (buffer[i]) {
                case '&':
                case '|':
                    printf(" %c ", buffer[i]);
                    ++i;
                    break;

                case '<':
                    fputs(" <=> ", stdout);
                    i += 3;
                    break;

                case '=':
                    fputs(" => ", stdout);
                    i += 2;
                    break;

                default:
                    putchar(buffer[i]);
                    ++i;
                    break;
            }
        }

        puts("");

        // generating all possible arrangements of 0s and 1s
        // by scanning binary digits of numbers from 0 to 2^var_count - 1
        for (int i = 0; i < (1 << var_count); ++i) {
            for (int j = var_count - 1, k = 0; k < 26; ++k) {
                if (var_used[k]) {
                    var_table[k] = (i & (1 << j)) >> j;
                    printf("%u | ", var_table[k]);
                    j--;

                    if (j < 0)
                        break;
                }
            }

            // the main dish - evaluation
            int safety_c = postfix->length;

            for (struct ListElement * it = postfix->last; it != NULL; it = it->prev) {
                struct Token t1 = it->value;

                if (t1.type == TOK_VAR || t1.type == TOK_ONE || t1.type == TOK_ZERO) {
                    listPushFront(rpn_stack, t1);
                    safety_c--;
                }
                else if (safety_c > 0) { // may cause a segfault
                    struct Token t2 = listPopFront(rpn_stack);
                    struct Token t3 = listPopFront(rpn_stack);
                    // assigning logical variables
                    bool p;
                    bool q;
                    bool result;

                    if (t2.type == TOK_VAR) // yes, the order is reversed
                        q = var_table[t2.value - 'a'];
                    else
                        q = t2.type == TOK_ONE;

                    if (t3.type == TOK_VAR)
                        p = var_table[t3.value - 'a'];
                    else
                        p = t3.type == TOK_ONE;

                    // logical evaluation
                    switch (t1.type) {
                        case TOK_AND:
                            result = p && q;
                            break;

                        case TOK_OR:
                            result = p || q;
                            break;

                        case TOK_IMPL:
                            result = (!p) || q;
                            break;

                        case TOK_EQUIV:
                            result = p == q;
                            break;

                        case TOK_NOT:
                            result = !q;
                            break;

                        default:
                            fprintf(stderr, "Evaluation error: Expected an operator.\n");
                            ERR_EXIT = EVAL_ERROR;
                            goto death;
                    }

                    struct Token tok_result;

                    tok_result.type = result ? TOK_ONE : TOK_ZERO;

                    listPushFront(rpn_stack, tok_result);
                }
                else {
                    fprintf(stderr, "Evaluation error at token: %d\n", postfix->length - safety_c);
                    ERR_EXIT = EVAL_ERROR;
                    goto death;
                }
            }

            if (rpn_stack->length > 1) {
                fprintf(stderr,
                        "Evaluation error: the stack contains more than one element.\n");
                ERR_EXIT = EVAL_ERROR;
                goto death;
            }
            else if (rpn_stack->length < 1) {
                fprintf(stderr, "Evaluation error: the stack is empty.\n");
                ERR_EXIT = EVAL_ERROR;
                goto death;
            }

            printf("%d\n", listPopFront(rpn_stack).type == TOK_ONE);
        }
    }
death:
    listDestroy(infix);
    listDestroy(operators);
    listDestroy(postfix);
    listDestroy(rpn_stack);
    free(lex_out);
    free(buffer);
    return ERR_EXIT;
}
