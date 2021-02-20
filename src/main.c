#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char * argv[]) {
    char * buffer = NULL;
    buffer = (char *)malloc(8192 * sizeof(char));

    if (!buffer) {
        fputs("Failed to allocate memory.\n", stderr);
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
    free(buffer);
    return 0;
}
