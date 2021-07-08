#include "mainwindow.h"
#include "initdialog.h"

#include <QApplication>

int MainWindow::VIRTUAL_MEM_PAGE = 8;
int MainWindow::PHYSICAL_MEM_PAGE = 4;
int MainWindow::PAGE_SIZE = 4;

Frame* MainWindow::memory = NULL;
Program* MainWindow::program_list = NULL;
PCB* MainWindow::pcb_list = NULL;
// Clock algorithm
int MainWindow::clock = 0;

BlockVis* MainWindow::physical_mem_vis = NULL;
QMap<int, BlockVis *> MainWindow::virtual_mems_vis;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    InitDialog dlg;
    if (dlg.exec() == QDialog::Accepted) {
        MainWindow w;
        w.show();
        return a.exec();
    }
    return 0;
}
