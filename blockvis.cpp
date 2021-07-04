#include "blockvis.h"
#include "ui_blockvis.h"

BlockVis::BlockVis(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BlockVis)
{
    ui->setupUi(this);
}

BlockVis::BlockVis(QWidget *parent, int block_num, int id, QColor color) :
    QMainWindow(parent),
    ui(new Ui::BlockVis)
{
    ui->setupUi(this);
    this->id = id;
    this->block_num = block_num;
    this->color = color;

    int block_height = 480 / block_num;
    block_height = block_height < 30 ? 30 : block_height;

    this->block_table_model = new QStandardItemModel;
    this->block_table_model->setColumnCount(1);
    this->block_table_model->setRowCount(block_num);
    this->ui->block_table->setModel(block_table_model);

    this->ui->block_table->setColumnWidth(0, 178);
    this->ui->block_table->verticalHeader()->setDefaultSectionSize(block_height);
    this->ui->block_table->verticalHeader()->setHidden(true);
    this->ui->block_table->horizontalHeader()->setHidden(true);
}

BlockVis::~BlockVis()
{
    delete ui;
}

QColor BlockVis::get_color() {
    return this->color;
}

void BlockVis::set_block(int index, int value, QColor color) {
    auto model = this->block_table_model;
    model->setItem(index, 0, new QStandardItem(QString::number(value)));
    model->item(index, 0)->setTextAlignment(Qt::AlignCenter);
    model->item(index, 0)->setFont(QFont("Times", 14, QFont::Black));
    model->item(index, 0)->setBackground(color);
}

void BlockVis::unset_block(int index) {
    auto model = this->block_table_model;
    model->item(index, 0)->setBackground(Qt::white);
    model->setItem(index, 0, new QStandardItem(""));
}
