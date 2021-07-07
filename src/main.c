#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "util/util.h"


int VIRTUAL_MEM_PAGE = 8;
int PHYSICAL_MEM_PAGE = 4;
int PAGE_SIZE = 4;

Frame* memory = NULL;
Program* program_list = NULL;
PCB* pcb_list = NULL;
// clock algorithm
int clock = 0;


int menu() {
    printf("\n-----------------\n");
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
    init();
    while (1) {
        switch (menu()) {
            case 1:
                create_program_interactive();
                break;
            case 2:
                create_process_interactive();
                break;
            case 3:
                execute_process_interactive();
                break;
            case 4:
                return;
        }
    }
}


void create_program_interactive() {
    int instructions[128];
    int inst_count = 0;
    printf("what is the execution sequence (e.g. 0 4 8 12 16 12 4 20)\n");
    printf("the number represents the memory address that instruction uses\n");
    printf("numbers should range from 0 to VIRTUAL_MEM_PAGE*PAGE_SIZE, i.e. %d\n",
           VIRTUAL_MEM_PAGE * PAGE_SIZE);
    char str[128];
    getline_stdin(str);
    printf("\n");

    char delims[] = " ";
    char *inst_chars = NULL;
    inst_chars = strtok(str, delims);
    int inst;
    while (inst_chars) {
        instructions[inst_count] = atoi(inst_chars);
        inst_count++;
        inst_chars = strtok(NULL, delims);
    }

    int program_id = create_program(instructions, inst_count);
    printf("program %d created\n", program_id);
}


void create_process_interactive() {
    printf("choose program id to create process from\n");
    int program_id;
    scanf("%d", &program_id);
    printf("\n");
    int process_id = create_process(program_id);
    if (process_id == -1) {
        printf("program %d not found\n", program_id);
    } else {
        printf("process %d created\n", process_id);
    }
    display_all_processes();
    display_memory();
}


void execute_process_interactive() {
    printf("which process do you want execute\n");
    int process_id;
    scanf("%d", &process_id);
    printf("\n");
    if (execute_process(process_id) == -1) {
        printf("process %d does not exist\n", process_id);
    } else {
        display_all_processes();
        display_memory();
    }
}


void display_all_processes() {
    PCB* p = pcb_list;
    while (p) {
        display_process(p);
        p = p->next;
    }
}

void display_process(PCB* process) {
    Program* program = process->program;

    printf("Process %d instructions:", process->id);
    for (int i = 0; i < program->inst_count; ++i) {
        printf(" %d", program->instructions[i]);
    }
    printf("\n");

    printf("PC: %d, PC instruction: %d",
           process->inst_executed,
           program->instructions[process->inst_executed]);
    printf("\n");
}


void display_memory() {
    printf("---------------------------------\n");
    for (int i = 0; i < PHYSICAL_MEM_PAGE; ++i) {
        if (memory[i].allocated) {
            printf("| process %d\t   page %d\t|\n",
                   memory[i].process_id,
                   memory[i].page_index);
        } else {
            printf("| \t\t\t\t|\n");
        }
        printf("---------------------------------\n");
    }
}


void init() {
    printf("setup PAGE SIZE: ");
    scanf("%d", &PAGE_SIZE);
    printf("setup PHYSICAL MEMORY PAGE size: ");
    scanf("%d", &PHYSICAL_MEM_PAGE);
    printf("setup VIRTUAL MEMORY PAGE size: ");
    scanf("%d", &VIRTUAL_MEM_PAGE);
    init_memory();
}
