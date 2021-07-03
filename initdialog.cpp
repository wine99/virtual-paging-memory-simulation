#include "initdialog.h"
#include "ui_initdialog.h"
#include "mainwindow.h"

InitDialog::InitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InitDialog)
{
    ui->setupUi(this);
}

InitDialog::~InitDialog()
{
    delete ui;
}

void InitDialog::on_enter_btn_clicked()
{
    MainWindow::VIRTUAL_MEM_PAGE = ui->virtual_page_text->text().toInt();
    MainWindow::PHYSICAL_MEM_PAGE = ui->physical_page_text->text().toInt();
    MainWindow::PAGE_SIZE = ui->page_size_text->text().toInt();

    Frame *memory = (Frame*) calloc(MainWindow::PHYSICAL_MEM_PAGE, sizeof(Frame));
    for (int i = 0; i < MainWindow::PHYSICAL_MEM_PAGE; ++i) {
        memory[i].allocated = false;
        memory[i].process_id = -1;
        memory[i].page_index = -1;
        memory[i].reference = false;
    }
    MainWindow::memory = memory;
}
