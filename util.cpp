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

int get_program_count(Program* program_list) {
    Program* p = program_list;
    int count = 0;
    while (p) {
        ++count;
        p = p->next;
    }
    return count;
}

int get_process_count(PCB* pcb_list) {
    PCB* p = pcb_list;
    int count = 0;
    while (p) {
        ++count;
        p = p->next;
    }
    return count;
}
