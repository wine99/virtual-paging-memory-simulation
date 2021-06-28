#ifndef MEMORY
#define MEMORY

#include <stdlib.h>
#include <stdbool.h>

/*
输入：虚拟内存页数，物理内存页数，页大小（给进程分配的页框数（最大设为 8），没用)
输入：程序，一个程序包含一个访问地址序列，一个程序可以创建多个进程
*/

int VIRTUAL_MEM_PAGE = 32;
int PHYSICAL_MEM_PAGE = 16;
int PAGE_SIZE = 4;
// int PROCESS_MAX_FRAME = 4;

// process page
typedef struct s_Page {
    // int index;
    // int content_size;
    // int process_id;
    bool in_mem;
    int frame_index;
    // bool reference;
    // struct s_Page* next;
} Page;

// memory page frame
typedef struct s_Frame {
    // int index;
    // int content_size;
    bool allocated;
    int process_id;
    int page_index;
    bool reference;
    // struct s_Frame* next;
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
    // Frame* frames[8];
} PCB;

Frame* memory = NULL;
Program* program_list = NULL;
PCB* pcb_list = NULL;
// clock algorithm
int clock = 0;


void main();
int menu();

int create_program();
int create_program_from_inst(int instructions[], int inst_count);
int create_process(int program_id);
void release_process(int process_id);
void execute_process(int process_id);

void init_memory();
Page* create_process_pages();
void release_process_pages(PCB* pcb);

void visit_page(PCB* process, int page_index);
int get_free_frame();
void swap_in(PCB* process, int page_index, int frame_index);
void swap_out(int frame_index);
int find_frame_to_swap_out();


#endif
