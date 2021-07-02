# truthtable
generates a truth table for a particular logical expression.

## Compilation
    $ mkdir obj
    $ make release

## Usage
`truthtable` generates a table upon receiving a logical expression from standard input.  
For example:

    $ ./truthtable 
    p & q
    p | q | p & q
    0 | 0 | 0
    0 | 1 | 0
    1 | 0 | 0
    1 | 1 | 1

It's also possible to pipe it in (duh):

    $ echo "p => q" | ./truthtable 
    p | q | p => q
    0 | 0 | 1
    0 | 1 | 1
    1 | 0 | 0
    1 | 1 | 1

## Supported symbols
| symbol | meaning | formal name |
|:-:|:-:|:-:|
|`&` | AND | conjunction |
|`\|` | OR | disjunction |
|`~` | NOT | negation |
|`=>` | IMPLIES | implication |
|`<=>` | IS EQUIVALENT | equivalence |
|`1` | TRUE | true |
|`0` | FALSE | false |
| `a` through `z` | VARIABLE |
|`(`| left parentheses|
|`)`|right parentheses|

## Additional notes

### Whitespace
`truthtable` strips whitespace from input. This means `p <=> ~q`, `p<=>~q`, `p   <=>   ~   q`, `p < = > ~ q` and `p <= >~q` are valid expressions.

### Error codes
| error code | meaning |
|:-:|:-:|
|0|no error|
|1|memory allocation error|
|2|input error|
|3|lexical error|
|4|parse error|
|5|evaluation error|

### Double tilde
Two or more consecutive tildes (`~~`) result in a lexical error. Double negation can be achieved by using parentheses: `~(~p)`.

### I'd like to denote true as T and false as F
Use `tr` and piping:

    $ echo "c => (a & b)" | ./truthtable | tr 10 TF
    a | b | c | c => (a & b)
    F | F | F | F
    F | F | F | T
    F | F | T | F
    F | T | F | T
    F | T | T | F
    T | F | F | T
    T | F | T | F
    T | T | F | T
    T | T | T | T

### How many variables can I use?
In principle, 26 lowercase letters of the English alphabet are available. This is more than enough, as a truth table of 26 variables would have 2^26 = 67108864 rows.
