#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "util.h"


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

void main() {
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

    if (!program_list) {
        program->id = 0;
        program_list = program;
        return program->id;
    }
    Program* p = program_list;
    while (p->next) {
        p = p->next;
    }

    program->id = p->id + 1;
    p->next = program;
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
        memory[i].reference = false;
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
    swap_in(process, page_index, frame_index);
}

void swap_in(PCB* process, int page_index, int frame_index) {
    process->pages[page_index].in_mem = true;
    process->pages[page_index].frame_index = frame_index;
    memory[frame_index].allocated = true;
    memory[frame_index].process_id = process->id;
    memory[frame_index].page_index = page_index;
    memory[frame_index].reference = true;
}

void swap_out(int frame_index) {
    Frame frame = memory[frame_index];
    PCB* process = find_process_by_id(frame.process_id);
    Page page = process->pages[frame.page_index];
    page.in_mem = false;
    page.frame_index = -1;
    frame.allocated = false;
    frame.process_id = -1;
    frame.page_index = -1;
    frame.reference = false;
}

int get_free_frame() {
    for (int i = 0; i < PHYSICAL_MEM_PAGE; ++i) {
        if (!memory[i].allocated) return i;
    }
    int frame_index = find_frame_to_swap_out();
    swap_out(frame_index);
    return frame_index;
}

int find_frame_to_swap_out() {
    while (1) {
        if (clock == PHYSICAL_MEM_PAGE) clock = 0;
        if (!memory[clock].allocated) {
            clock++;
            continue;
        }
        if (!memory[clock].reference) {
            return clock;
        }
        memory[clock].reference = false;
        clock++;
    }
}
