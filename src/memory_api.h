#ifndef MEMORY_API_H
#define MEMORY_API_H
#include <stdbool.h>

// Process page
typedef struct s_Page {
    bool in_mem;
    int frame_index;
} Page;

// Memory page frame
typedef struct s_Frame {
    bool allocated;
    int process_id;
    int page_index;
    bool reference;
} Frame;

typedef struct s_Program {
    int id;
    int inst_count;
    int instructions[128];
    struct s_Program* next;
} Program;

typedef struct s_PCB {
    int id;
    Program* program;
    int inst_executed;
    Page* pages;
    struct s_PCB* next;
} PCB;


void init_memory();

int create_program(int instructions[], int inst_count);
int create_process(int program_id);
int release_process(int process_id);
int execute_process(int process_id);

Program* find_program_by_id(int program_id);
PCB* find_process_by_id(int process_id);
int get_next_instruction(PCB* process);
int get_page_index(int address, int page_size);
int get_page_offset(int address, int page_size);

#endif
