#include <stdlib.h>
#include <string.h>
#include "memory.h"


int create_program(int instructions[], int inst_count) {
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


int release_process(int process_id) {
    if (!pcb_list) {
        // printf("process does not exist: %d -- RELEASE_PROCESS\n",
        //         process_id);
        return -1;
    }
    // printf("process %d exiting\n", process_id);

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
        return 0;
    }
    // printf("process does not exist: %d -- RELEASE_PROCESS\n",
    //         process_id);
    return -1;
}


int execute_process(int process_id) {
    PCB* process = find_process_by_id(process_id);
    if (!process) {
        // printf("process does not exist: %d -- EXECUTE_PROCESS\n",
        //         process_id);
        return -1;
    }

    Program* program = process->program;
    if (process->inst_executed >= program->inst_count) {
        // printf("unexpected error: instructions of process %d "
        //        "already ran out -- EXECUTE_PROCESS\n", process_id);
        return -1;
    }

    int address = get_next_instruction(process);
    int page_index = get_page_index(address, PAGE_SIZE);
    int offset = get_page_offset(address, PAGE_SIZE);
    // printf("process %d executes instruction %d ,",
    //        process_id, process->inst_executed);
    // printf("visiting virtual memory address %d: page %d, offset %d\n",
    //        address, page_index, offset);
    visit_page(process, page_index);

    process->inst_executed++;
    if (process->inst_executed == program->inst_count) {
        return release_process(process_id);
    }
    return 0;
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
    int process_id = process->id;
    for (int i = 0; i < PHYSICAL_MEM_PAGE; ++i) {
        if (memory[i].process_id == process->id) {
            memory[i].allocated = false;
            memory[i].process_id = -1;
            memory[i].page_index = -1;
            memory[i].reference = false;
        }
    }
    free(process->pages);
}


void visit_page(PCB* process, int page_index) {
    Page page = process->pages[page_index];

    if (page.in_mem) {
        // printf("hit\n");
        memory[page.frame_index].reference = true;
        return;
    }

    // printf("miss\n");
    int frame_index = get_free_frame();
    swap_in(process, page_index, frame_index);
}


void swap_in(PCB* process, int page_index, int frame_index) {
    // printf("page %d of process %d swaps in to frame %d\n",
    //        page_index, process->id, frame_index);
    process->pages[page_index].in_mem = true;
    process->pages[page_index].frame_index = frame_index;
    memory[frame_index].allocated = true;
    memory[frame_index].process_id = process->id;
    memory[frame_index].page_index = page_index;
    memory[frame_index].reference = true;
}


void swap_out(int frame_index) {
    // printf("frame %d, ", frame_index);
    Frame frame = memory[frame_index];
    PCB* process = find_process_by_id(frame.process_id);
    // printf("which belongs to process %d, page %d, swaps out to disk\n",
    //        process->id, frame.page_index);
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


Program* find_program_by_id(int program_id) {
    if (!program_list) return NULL;
    Program* p = program_list;
    while (p && p->id != program_id) {
        p = p->next;
    }
    return p;
}

PCB* find_process_by_id(int process_id) {
    if (!pcb_list) return NULL;
    PCB* p = pcb_list;
    while (p && p->id != process_id) {
        p = p->next;
    }
    return p;
}


int get_next_instruction(PCB* process) {
    return process->program->instructions[process->inst_executed];
}

int get_page_index(int address, int page_size) {
    return (int) (address / page_size);
}

int get_page_offset(int address, int page_size) {
    return address % page_size;
}
