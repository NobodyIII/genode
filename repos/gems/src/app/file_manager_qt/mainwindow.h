#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QLineEdit>
#include <QListView>
#include <QItemSelectionModel>
#include <QActionGroup>
#include "genodefsmodel.h"
#include "folderview.h"
#include "nodedelegate.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void nodeUpdate();

private:
    Ui::MainWindow *ui;
	GenodeFSModel *model;
	QListView *view;
	QItemSelectionModel *selection;
    NodeDelegate *delegate;
    QActionGroup *sortGroup;

    QLineEdit path_box;

public slots:
    void contextMenuEvent(QContextMenuEvent *event);

private slots:
    /* use model instead for these
    void up();
    void back();
    void forward();
    void refresh();*/
    void cut();
    void copy();
    void paste();
    void del();
    void rename();
    void new_folder();
    void new_file();
    void go();
	//void exit();
    void new_window();
    void quit();
    void update_path();
    void setGrouping(QAction *selection);
};

#endif // MAINWINDOW_H
