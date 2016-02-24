#include "dragdropmenu.h"
#include "folderview.h"

FolderView::FolderView()
{
}
void FolderView::dropEvent(QDropEvent *event)
{
	event->setDropAction(DragDropMenu::ask(QCursor::pos()));
	QListView::dropEvent(event);
}
