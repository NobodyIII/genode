#include "dragdropmenu.h"

DragDropMenu::DragDropMenu()
{
    copyAction=addAction(QIcon::fromTheme("edit-copy",QIcon(":/images/edit-copy")), "Copy Here");
    moveAction=addAction(QIcon::fromTheme("go-jump",QIcon(":/images/go-jump")), "Move Here");
	addSeparator();
    cancelAction=addAction(QIcon::fromTheme("window-close",QIcon(":/images/window-close")), "Cancel");
	setDefaultAction(cancelAction);
}

Qt::DropAction DragDropMenu::ask(QPoint p)
{
	DragDropMenu menu;
	QAction* action=menu.exec(p);
	if (action==menu.copyAction)
		return Qt::CopyAction;
	else if (action==menu.moveAction)
		return Qt::MoveAction;
	else
		return Qt::IgnoreAction;
}

