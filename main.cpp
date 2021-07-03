#include "mainwindow.h"
#include "initdialog.h"

#include <QApplication>

int MainWindow::VIRTUAL_MEM_PAGE = 8;
int MainWindow::PHYSICAL_MEM_PAGE = 4;
int MainWindow::PAGE_SIZE = 4;
// int PROCESS_MAX_FRAME = 4;

Frame* MainWindow::memory = NULL;
Program* MainWindow::program_list = NULL;
PCB* MainWindow::pcb_list = NULL;
// clock algorithm
int MainWindow::clock = 0;


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    //InitMem init_mem_window;
    //init_mem_window.show();
    InitDialog dlg;
    if (dlg.exec() == QDialog::Accepted) {
        MainWindow w;
        w.show();
        return a.exec();
    }
    return 0;
}
