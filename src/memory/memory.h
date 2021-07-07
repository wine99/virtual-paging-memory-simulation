#ifndef MEMORY_H
#define MEMORY_H
#include <stdbool.h>

/*
输入：虚拟内存页数，物理内存页数，页大小
输入：程序，一个程序包含一个访问地址序列，一个程序可以创建多个进程
*/


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


extern int VIRTUAL_MEM_PAGE;
extern int PHYSICAL_MEM_PAGE;
extern int PAGE_SIZE;

extern Frame* memory;
extern Program* program_list;
extern PCB* pcb_list;
// Clock algorithm
extern int clock;


int create_program(int instructions[], int inst_count);
int create_process(int program_id);
int release_process(int process_id);
int execute_process(int process_id);

void init_memory();
Page* create_process_pages();
void release_process_pages(PCB* pcb);

void visit_page(PCB* process, int page_index);
int get_free_frame();
void swap_in(PCB* process, int page_index, int frame_index);
void swap_out(int frame_index);
int find_frame_to_swap_out();

Program* find_program_by_id(int program_id);
PCB* find_process_by_id(int process_id);
int get_next_instruction(PCB* process);
int get_page_index(int address, int page_size);
int get_page_offset(int address, int page_size);

#endif
