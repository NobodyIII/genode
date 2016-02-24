#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "genodefsmodel.h"
#include "nodedelegate.h"

//QFileSystemModel model;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainToolBar->addWidget(&path_box);

    sortGroup=new QActionGroup(this);
    ui->actionSortName->setCheckable(true);
    ui->actionSortSize->setCheckable(true);
    ui->actionSortName->setActionGroup(sortGroup);
    ui->actionSortSize->setActionGroup(sortGroup);
    ui->actionSortName->setChecked(true);
    connect(sortGroup,SIGNAL(triggered(QAction*)),this,SLOT(setGrouping(QAction*)));

    model=new GenodeFSModel();

	connect(ui->actionBack,SIGNAL(triggered(bool)),model,SLOT(back()));
	connect(ui->actionForward,SIGNAL(triggered(bool)),model,SLOT(forward()));
	connect(ui->actionUp,SIGNAL(triggered(bool)),model,SLOT(up()));
	connect(ui->actionRefresh,SIGNAL(triggered(bool)),model,SLOT(refresh()));

	view=ui->folderView;
	view->setModel(model);
	selection=view->selectionModel();
	delegate=new NodeDelegate(view);
	view->setItemDelegate(delegate);

	connect(ui->actionCopy,SIGNAL(triggered(bool)),this,SLOT(copy()));
	connect(ui->actionCut,SIGNAL(triggered(bool)),this,SLOT(cut()));
	connect(ui->actionDelete,SIGNAL(triggered(bool)),this,SLOT(del()));
	connect(ui->actionGo,SIGNAL(triggered(bool)),this,SLOT(go()));
	connect(ui->actionNew_File,SIGNAL(triggered(bool)),this,SLOT(new_file()));
	connect(ui->actionNew_Folder,SIGNAL(triggered(bool)),this,SLOT(new_folder()));
	connect(ui->actionPaste,SIGNAL(triggered(bool)),this,SLOT(paste()));
	connect(ui->actionQuit,SIGNAL(triggered(bool)),this,SLOT(quit()));
	connect(ui->actionRename,SIGNAL(triggered(bool)),this,SLOT(rename()));
	connect(ui->action_New_Window,SIGNAL(triggered(bool)),this,SLOT(new_window()));

    connect(view,SIGNAL(doubleClicked(QModelIndex)),model,SLOT(open(QModelIndex)));
    connect(view,SIGNAL(activated(QModelIndex)),model,SLOT(open(QModelIndex)));

    connect(model,SIGNAL(modelReset()),this,SLOT(update_path()));

    connect(&path_box,SIGNAL(returnPressed()),this,SLOT(go()));

    update_path();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete model;
}

void MainWindow::cut()
{
	model->cut(selection->selectedIndexes());
    QModelIndex index=model->index(0,5);
    qDebug() << index.isValid();
    //selection->select(index,QItemSelectionModel::ClearAndSelect);
    view->setCurrentIndex(index);
    view->edit(index);
}

void MainWindow::copy()
{
	model->copy(selection->selectedIndexes());
}

void MainWindow::paste()
{
	model->paste();
}

void MainWindow::del()
{
	model->del(selection->selectedIndexes());
}

void MainWindow::rename()
{
    view->edit(selection->currentIndex());
}

void MainWindow::new_folder()
{
    selection->setCurrentIndex(model->new_folder(),QItemSelectionModel::ClearAndSelect);
    rename();
}

void MainWindow::new_file()
{
	selection->setCurrentIndex(model->new_file(),QItemSelectionModel::ClearAndSelect);
    rename();
}

void MainWindow::go()
{
	model->go(path_box.text());
    view->setFocus();
}

void MainWindow::new_window()
{

}

void MainWindow::quit()
{
	quit();
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    if (focusWidget()==view) {
        if (selection->hasSelection()) {
            menu.addAction(ui->actionCopy);
            menu.addAction(ui->actionCut);
            menu.addSeparator();
            menu.addAction(ui->actionRename);
            menu.addAction(ui->actionDelete);
        } else {
            menu.addAction(ui->actionNew_Folder);
            menu.addAction(ui->actionNew_File);
            menu.addSeparator();
            menu.addAction(ui->actionPaste);
        }
        menu.exec(event->globalPos());
    }
}

void MainWindow::update_path()
{
    path_box.setText(model->getcwd());
}

void MainWindow::setGrouping(QAction *selection) {
    if (selection==ui->actionSortName)
        model->setSorting(GenodeFSModel::SORT_NAME);
    else if (selection==ui->actionSortSize)
        model->setSorting(GenodeFSModel::SORT_SIZE);
}
