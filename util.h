#ifndef UTIL
#define UTIL

#include <stdio.h>
#include <string.h>
#include "memory.h"

void getline_stdin(char* buf) {
    fgets(buf, 80, stdin);
    char *find = strchr(buf, '\n');
    if (find) *find = '\0';
}

Program* find_program_by_id(int program_id) {
    if (program_count == 0) return NULL;
    Program* p = program_list;
    while (p && p->id != program_id) {
        p = p->next;
    }
    return p;
}

Program* find_process_by_id(int process_id) {
    if (process_count == 0) return NULL;
    PCB* p = pcb_list;
    while (p && p->id != process_id) {
        p = p->next;
    }
    return p;
}

inline int get_next_instruction(PCB* process) {
    return process->program->instructions[process->inst_executed];
}

inline int get_page_index(int address, int page_size) {
    return (int) (address / page_size);
}

inline int get_page_offset(int address, int page_size) {
    return address % page_size;
}

#endif
