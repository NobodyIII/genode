#ifndef DRAGDROPMENU_H
#define DRAGDROPMENU_H

#include <QMenu>
#include "genodefsmodel.h"

class DragDropMenu : public QMenu
{
	Q_OBJECT
public:
	DragDropMenu();
	static Qt::DropAction ask(QPoint p);

private:
	QAction* copyAction;
	QAction* moveAction;
	QAction* cancelAction;
};

#endif // DRAGDROPMENU_H
