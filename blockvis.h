#ifndef BLOCKVIS_H
#define BLOCKVIS_H

#include <QMainWindow>
#include <QStandardItemModel>

namespace Ui {
class BlockVis;
}

class BlockVis : public QMainWindow
{
    Q_OBJECT

public:
    explicit BlockVis(QWidget *parent = nullptr);
    explicit BlockVis(QWidget *parent = nullptr, int block_num = 8, int id = 0, QColor color = Qt::white);

    QColor get_color();
    void set_block(int index, int value, QColor color);
    void unset_block(int index);
    ~BlockVis();

private:
    Ui::BlockVis *ui;
    int block_num;
    int id;
    QColor color;
    QStandardItemModel *block_table_model;
};

#endif // BLOCKVIS_H
