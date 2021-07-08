#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>
#include <QMap>
#include <QMainWindow>
#include <QStandardItemModel>
#include <blockvis.h>
#include <memory.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static int VIRTUAL_MEM_PAGE;
    static int PHYSICAL_MEM_PAGE;
    static int PAGE_SIZE;

    static Frame* memory;
    static Program* program_list;
    static PCB* pcb_list;
    // Clock algorithm
    static int clock;

    static BlockVis *physical_mem_vis;
    static QMap<int, BlockVis*> virtual_mems_vis;

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

    Program* find_program_by_id(int program_id);
    PCB* find_process_by_id(int process_id);
    int get_next_instruction(PCB* process);
    int get_page_index(int address, int page_size);
    int get_page_offset(int address, int page_size);


private slots:
    void on_create_program_btn_clicked();

    void on_create_process_btn_clicked();

    void on_exec_process_btn_clicked();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *program_table_model;
    QStandardItemModel *pcb_table_model;

    int find_process_row(int process_id);

};
#endif // MAINWINDOW_H
