#ifndef TEXTDELEGATE_H
#define TEXTDELEGATE_H

#include <QItemDelegate>
#include <QStyledItemDelegate>

class NodeDelegate : public QStyledItemDelegate //QItemDelegate
{
Q_OBJECT

public:
	NodeDelegate(QWidget *parent = 0) : QStyledItemDelegate(parent) {}
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	/* add paint to fix text alignment on rename */
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void drawText(QPainter* painter, QStyleOptionViewItem& opt, QRectF& textRect) const;
};
#endif // TEXTDELEGATE_H
