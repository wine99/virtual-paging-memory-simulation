#ifndef UTIL
#define UTIL

#include <stdio.h>
#include <string.h>
#include "memory.h"

void getline_stdin(char* buf);

Program* find_program_by_id(int program_id);

PCB* find_process_by_id(int process_id);

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
