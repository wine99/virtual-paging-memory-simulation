#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "util.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->program_table_model = new QStandardItemModel;
    this->program_table_model->setColumnCount(2);
    this->program_table_model->setHorizontalHeaderItem(0, new QStandardItem("id") );
    this->program_table_model->setHorizontalHeaderItem(1, new QStandardItem("instruction sequence"));
    this->ui->program_table->setModel(this->program_table_model);

    this->ui->program_table->verticalHeader()->setHidden(true);
    this->ui->program_table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    this->ui->program_table->setColumnWidth(0, 48);
    this->ui->program_table->setColumnWidth(1, 750);

    this->pcb_table_model = new QStandardItemModel;
    this->pcb_table_model->setColumnCount(4);
    this->pcb_table_model->setHorizontalHeaderItem(0, new QStandardItem("id") );
    this->pcb_table_model->setHorizontalHeaderItem(1, new QStandardItem("instruction sequence"));
    this->pcb_table_model->setHorizontalHeaderItem(2, new QStandardItem("pc"));
    this->pcb_table_model->setHorizontalHeaderItem(3, new QStandardItem("pc inst"));
    // this->pcb_table_model->setHorizontalHeaderItem(4, new QStandardItem("exec"));
    this->ui->pcb_table->setModel(this->pcb_table_model);

    this->ui->pcb_table->verticalHeader()->setHidden(true);
    this->ui->pcb_table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    this->ui->pcb_table->setColumnWidth(0, 48);
    this->ui->pcb_table->setColumnWidth(1, 600);
    this->ui->pcb_table->setColumnWidth(2, 50);
    this->ui->pcb_table->setColumnWidth(3, 100);
    // this->ui->pcb_table->setColumnWidth(4, 100);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::create_program() {
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
    while (inst_chars) {
        instructions[inst_count] = atoi(inst_chars);
        inst_count++;
        inst_chars = strtok(NULL, delims);
    }

    int program_id = create_program_from_inst(instructions, inst_count);
    printf("program %d created\n", program_id);
    return program_id;
}


int MainWindow::create_program_from_inst(int instructions[], int inst_count) {
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

int MainWindow::create_process(int program_id) {
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
        printf("process %d created\n", pcb->id);
        return pcb->id;
    }

    PCB* p = pcb_list;
    while (p->next) {
        p = p->next;
    }
    pcb->id = p->id + 1;
    p->next = pcb;
    printf("process %d created\n", pcb->id);
    return pcb->id;
}

void MainWindow::release_process(int process_id) {
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

void MainWindow::execute_process(int process_id) {
    PCB* process = find_process_by_id(process_id);
    if (!process) {
        printf("process does not exist: %d -- EXECUTE_PROCESS\n",
                process_id);
        return;
    }

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
    printf("visiting virtual memory address %d: page %d, offset %d\n",
           address, page_index, offset);
    visit_page(process, page_index);

    process->inst_executed++;
    if (process->inst_executed == program->inst_count) {
        release_process(process_id);
    }
}

void MainWindow::init_memory() {
    memory = (Frame*) calloc(PHYSICAL_MEM_PAGE, sizeof(Frame));
    for (int i = 0; i < PHYSICAL_MEM_PAGE; ++i) {
        memory[i].allocated = false;
        memory[i].process_id = -1;
        memory[i].page_index = -1;
        memory[i].reference = false;
    }
}

Page* MainWindow::create_process_pages() {
    Page* pages = (Page*) calloc(VIRTUAL_MEM_PAGE, sizeof(Page));
    for (int i = 0; i < VIRTUAL_MEM_PAGE; ++i) {
        pages[i].in_mem = false;
        pages[i].frame_index = -1;
    }
    return pages;
}

void MainWindow::release_process_pages(PCB* process) {
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

void MainWindow::visit_page(PCB* process, int page_index) {
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

void MainWindow::swap_in(PCB* process, int page_index, int frame_index) {
    printf("page %d of process %d swaps in to frame %d\n",
           page_index, process->id, frame_index);
    process->pages[page_index].in_mem = true;
    process->pages[page_index].frame_index = frame_index;
    memory[frame_index].allocated = true;
    memory[frame_index].process_id = process->id;
    memory[frame_index].page_index = page_index;
    memory[frame_index].reference = true;
}

void MainWindow::swap_out(int frame_index) {
    printf("frame %d, ", frame_index);
    Frame frame = memory[frame_index];
    PCB* process = find_process_by_id(frame.process_id);
    printf("which belongs to process %d, page %d, swaps out to disk\n",
           process->id, frame.page_index);
    Page page = process->pages[frame.page_index];
    page.in_mem = false;
    page.frame_index = -1;
    frame.allocated = false;
    frame.process_id = -1;
    frame.page_index = -1;
    frame.reference = false;
}

int MainWindow::get_free_frame() {
    for (int i = 0; i < PHYSICAL_MEM_PAGE; ++i) {
        if (!memory[i].allocated) return i;
    }
    int frame_index = find_frame_to_swap_out();
    swap_out(frame_index);
    return frame_index;
}

int MainWindow::find_frame_to_swap_out() {
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

Program* MainWindow::find_program_by_id(int program_id) {
    if (!program_list) return NULL;
    Program* p = program_list;
    while (p && p->id != program_id) {
        p = p->next;
    }
    return p;
}

PCB* MainWindow::find_process_by_id(int process_id) {
    if (!pcb_list) return NULL;
    PCB* p = pcb_list;
    while (p && p->id != process_id) {
        p = p->next;
    }
    return p;
}

int MainWindow::get_next_instruction(PCB* process) {
    return process->program->instructions[process->inst_executed];
}

int MainWindow::get_page_index(int address, int page_size) {
    return (int) (address / page_size);
}

int MainWindow::get_page_offset(int address, int page_size) {
    return address % page_size;
}


int MainWindow::find_process_row(int process_id) {
    for (int i = 0; i < get_process_count(pcb_list); ++i) {
        if (this->pcb_table_model->item(i, 0)->text().toInt() == process_id)
            return i;
    }
    return -1;
}


void MainWindow::on_create_program_btn_clicked()
{
    char *c = this->ui->create_program_text->text().toLatin1().data();
    int instructions[128];
    int inst_count = 0;

    char delims[] = " ";
    char *inst_chars = NULL;
    inst_chars = strtok(c, delims);
    while (inst_chars) {
        instructions[inst_count] = atoi(inst_chars);
        inst_count++;
        inst_chars = strtok(NULL, delims);
    }

    // for tuples in program_table, ROW always equals to ID
    int program_id = create_program_from_inst(instructions, inst_count);
    int row = get_program_count(program_list) - 1;
    this->program_table_model->setItem(row, 0, new QStandardItem(QString::number(program_id)));
    this->program_table_model->setItem(row, 1, new QStandardItem(this->ui->create_program_text->text()));
}


void MainWindow::on_create_process_btn_clicked()
{
    int program_id = this->ui->create_process_text->text().toInt();
    int process_id = create_process(program_id);
    if (process_id == -1) return;

    Program* program = find_program_by_id(program_id);
    int row = get_process_count(pcb_list) - 1;
    this->pcb_table_model->setItem(row, 0, new QStandardItem(QString::number(process_id)));
    this->pcb_table_model->setItem(row, 1, new QStandardItem(this->program_table_model->item(program_id, 1)->text()));
    this->pcb_table_model->setItem(row, 2, new QStandardItem(QString::number(0)));
    this->pcb_table_model->setItem(row, 3, new QStandardItem(QString::number(program->instructions[0])));
}


void MainWindow::on_exec_process_btn_clicked()
{
    int process_id = this->ui->exec_process_text->text().toInt();

    // code below is the same as execute_process()
    PCB* process = find_process_by_id(process_id);
    if (!process) {
        printf("process does not exist: %d -- EXECUTE_PROCESS\n",
                process_id);
        return;
    }

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
    printf("visiting virtual memory address %d: page %d, offset %d\n",
           address, page_index, offset);
    visit_page(process, page_index);

    process->inst_executed++;
    // code above is the same as execute_process()

    int row = find_process_row(process_id);
    if (process->inst_executed == program->inst_count) {
        this->pcb_table_model->removeRow(row);
        release_process(process_id);
    } else {
        int row = find_process_row(process_id);
        this->pcb_table_model->setItem(row, 2, new QStandardItem(QString::number(process->inst_executed)));
        this->pcb_table_model->setItem(row, 3, new QStandardItem(QString::number(program->instructions[process->inst_executed])));
    }
}

