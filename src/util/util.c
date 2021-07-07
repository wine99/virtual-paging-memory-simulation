#include "util.h"
#include <stdio.h>
#include <string.h>

void getline_stdin(char* buf) {
    int c;
    do {
        c = getchar();
    } while (c != EOF && c != '\n');
    fgets(buf, 80, stdin);
    char* find = strchr(buf, '\n');
    if (find) *find = '\0';
}
