#ifndef MAIN_H
#define MAIN_H
#include "memory_api.h"

void main();
void init();
int menu();

void create_program_interactive();
void create_process_interactive();
void execute_process_interactive();

void display_all_processes();
void display_memory();
void display_process(PCB* process);

#endif
