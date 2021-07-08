#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <stdbool.h>

/*
输入：虚拟内存页数，物理内存页数，页大小（给进程分配的页框数（最大设为 8），没用)
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


#endif // MEMORY_H
