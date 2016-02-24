#ifndef FOLDERVIEW_H
#define FOLDERVIEW_H

#include <QListView>
#include <QDropEvent>

class FolderView : public QListView
{
	Q_OBJECT
public:
	FolderView();
	FolderView(QWidget *parent = 0): QListView(parent) {};

protected:
	void dropEvent(QDropEvent *event);

private:
};

#endif // FOLDERVIEW_H
