#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "util.h"


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
void page_swap_in(PCB* process, int page_index, int frame_index);
void page_swap_out(PCB* process, int page_index);


int menu() {
    printf("-------------------");
    printf("1 create_program\n");
    printf("2 create_process\n");
    printf("3 execute_process\n");
    printf("4 quit\n\n");
    int choice;
    scanf("%d", &choice);
    printf("\n");
    return choice;
}

int create_program() {
    int instructions[128];
    int inst_count = 0;
    printf("what is the execution sequence (e.g. 1 2 3 4 5 1 2)\n");
    printf("the number represents the memory address that instruction uses\n");
    printf("numbers should range from 0 to VIRTUAL_MEM_PAGE*PAGE_SIZE, i.e. %d\n",
           VIRTUAL_MEM_PAGE * PAGE_SIZE);
    char str[128];
    getline_stdin(str);

    char delims[] = " ";
    char *inst_chars = NULL;
    inst_chars = strtok(str, delims);
    int inst;
    while (inst_chars) {
        instructions[inst_count] = atoi(inst_chars);
        inst_count++;
        inst_chars = strtok(NULL, delims);
    }

    return create_program_from_inst(instructions, inst_count);
}

int create_program_from_inst(int instructions[], int inst_count) {
    Program* program = (Program*) malloc(sizeof(Program));
    program->inst_count = inst_count;
    memcpy(program->instructions, instructions, inst_count * sizeof(int));
    program->next = NULL;

    if (program_count == 0) {
        program->id = 0;
        program_list = program;
        program_count++;
        return program->id;
    }
    Program* p = program_list;
    while (p->next) {
        p = p->next;
    }

    program->id = p->id + 1;
    p->next = program;
    program_count++;
    return program->id;
}

int create_process(int program_id) {
    Program* program = find_program_by_id(program_id);
    if (!program) {
        printf("program not found: %d -- CREATE_PROCESS\n", program_id);
        return -1;
    }

    PCB* pcb = (PCB*) malloc(sizeof(PCB));
    pcb->program = program;
    pcb->inst_executed = 0;
    pcb->next = NULL;
    pcb->pages = create_process_pages();

    if (!pcb_list) {
        pcb->id = 0;
        pcb_list = pcb;
        return pcb->id;
    }

    PCB* p = pcb_list;
    while (p->next) {
        p = p->next;
    }
    pcb->id = p->id + 1;
    p->next = pcb;
    return pcb->id;
}

void release_process(int process_id) {
    if (!pcb_list) {
        printf("process does not exist: %d -- RELEASE_PROCESS\n",
                process_id);
        return;
    }
    printf("process %d exiting\n", process_id);

    PCB* p = pcb_list;
    PCB* prev = NULL;
    while (p) {
        if (p->id != process_id) {
            prev = p;
            p = p->next;
            continue;
        }

        release_process_pages(p);
        if (!prev) {
            pcb_list = pcb_list->next;
        } else {
            prev->next = p->next;
        }
        free(p);
        return;
    }
    printf("process does not exist: %d -- RELEASE_PROCESS\n",
            process_id);
}

void execute_process(int process_id) {
    if (!pcb_list) {
        printf("process does not exist: %d -- EXECUTE_PROCESS\n",
                process_id);
        return;
    }

    PCB* process = find_process_by_id(process_id);
    Program* program = process->program;

    if (process->inst_executed >= program->inst_count) {
        printf("unexpected error: instructions of process %d "
               "already ran out -- EXECUTE_PROCESS\n", process_id);
        return;
    }

    // 拿到要指令也就是地址
    // 地址转换并打印：进程i要执行第j条指令，访问地址k，在第l页中
    // 判断l页是否在内存
    // 若不在，call 请求调页，参数进程id，页号l
    // inst_executed++，若已经和指令总数相同，call release_process

    int address = get_next_instruction(process);
    int page_index = get_page_index(address, PAGE_SIZE);
    int offset = get_page_offset(address, PAGE_SIZE);
    printf("process %d executes instruction %d ,",
           process_id, process->inst_executed);
    printf("visiting virtual memory address %d\n: page %d, offset %d\n",
           address, page_index, offset);
    visit_page(process, page_index);

    process->inst_executed++;
    if (process->inst_executed == program->inst_count) {
        release_process(process_id);
    }
}

void init_memory() {
    memory = (Frame*) calloc(PHYSICAL_MEM_PAGE, sizeof(Frame));
    for (int i = 0; i < PHYSICAL_MEM_PAGE; ++i) {
        memory[i].allocated = false;
        memory[i].process_id = -1;
        memory[i].page_index = -1;
        memory[i].reference = 0;
    }
}

Page* create_process_pages() {
    Page* pages = (Page*) calloc(VIRTUAL_MEM_PAGE, sizeof(Page));
    for (int i = 0; i < VIRTUAL_MEM_PAGE; ++i) {
        pages[i].in_mem = false;
        pages[i].frame_index = -1;
    }
    return pages;
}

void release_process_pages(PCB* process) {
    free(process->pages);
}

void visit_page(PCB* process, int page_index) {
    Page page = process->pages[page_index];

    if (page.in_mem) {
        printf("hit\n");
        memory[page.frame_index].reference = true;
        return;
    }

    printf("miss\n");
    int frame_index = get_free_frame();
    page_swap_in(process, page_index, frame_index);
}

void page_swap_in(PCB* process, int page_index, int frame_index) {
    process->pages[page_index].in_mem = true;
    process->pages[page_index].frame_index = frame_index;
    memory[frame_index].allocated = true;
    memory[frame_index].process_id = process->id;
    memory[frame_index].page_index = page_index;
    memory[frame_index].reference = true;
}

void page_swap_out()

int get_free_frame() {
    for (int i = 0; i < PHYSICAL_MEM_PAGE; ++i) {
        if (!memory[i].allocated) return i;
    }

}


int main() {
    while (1) {
        switch (menu()) {
            case 1:
                create_program();
                break;
            case 2:
                printf("choose program id to create process from\n");
                int program_id;
                scanf("%d", &program_id);
                printf("\n");
                create_process(program_id);
                break;
            case 3:
                printf("which process do you want execute\n");
                int process_id;
                scanf("%d", &process_id);
                printf("\n");
                execute_process(process_id);
                break;
            case 4:
            default:
                return;
        }
    }
}
