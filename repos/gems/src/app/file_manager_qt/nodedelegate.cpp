#include <QPainter>
#include <QApplication>
#include <QIcon>
#include <QTextLayout>
#include <QTextOption>
#include <QTextLine>
#include "nodedelegate.h"
#include "fsnode.h"

QSize NodeDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItem opt=option;
	initStyleOption(&opt,index);
	//QRectF textRect(0,0,opt.rect.width()-4,opt.rect.height()-opt.decorationSize.height()-4);
	/* use hardcoded values for Genode */
	QRectF textRect(0,0,96-4,96-opt.decorationSize.height()-4);
	drawText(NULL,opt,textRect);
	int width=textRect.width()+4;
	int height=textRect.height()+opt.decorationSize.height()+4;
	if (width<opt.decorationSize.width()) width=opt.decorationSize.width();
	return QSize(width,height);
}

void NodeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItem opt=option;
	initStyleOption(&opt,index);
	painter->save();
	painter->setClipRect(option.rect);

	QIcon icon=opt.icon;
	QIcon::Mode iconMode;
	if (opt.state & QStyle::State_Enabled) {
		if (opt.state & QStyle::State_Selected)
			iconMode=QIcon::Selected;
		else
			iconMode=QIcon::Normal;
	} else iconMode=QIcon::Disabled;
	QPoint iconPos(opt.rect.x() + (opt.rect.width() - opt.decorationSize.width()) / 2, opt.rect.y());
	QPixmap pixmap = icon.pixmap(opt.decorationSize, iconMode);
	painter->drawPixmap(iconPos, pixmap);
	QRectF textRect(opt.rect.x(),opt.rect.y()+opt.decorationSize.height(),opt.rect.width(),opt.rect.height()-opt.decorationSize.height());
	drawText(painter,opt,textRect);

	painter->restore();
}

/* from libfm-qt */
void NodeDelegate::drawText(QPainter* painter, QStyleOptionViewItem& opt, QRectF& textRect) const {
  QTextLayout layout(opt.text, opt.font);
  QTextOption textOption;
  textOption.setAlignment(opt.displayAlignment);
  textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
  textOption.setTextDirection(opt.direction);
  layout.setTextOption(textOption);
  qreal height = 0;
  qreal width = 0;
  int visibleLines = 0;
  layout.beginLayout();
  QString elidedText;
  for(;;) {
	QTextLine line = layout.createLine();
	if(!line.isValid())
	  break;
	line.setLineWidth(textRect.width());
	height += opt.fontMetrics.leading();
	line.setPosition(QPointF(0, height));
	if((height + line.height() + textRect.y()) > textRect.bottom()) {
	  // if part of this line falls outside the textRect, ignore it and quit.
	  QTextLine lastLine = layout.lineAt(visibleLines - 1);
	  elidedText = opt.text.mid(lastLine.textStart());
	  elidedText = opt.fontMetrics.elidedText(elidedText, opt.textElideMode, textRect.width());
	  if(visibleLines == 1) // this is the only visible line
		width = textRect.width();
	  break;
	}
	height += line.height();
	width = qMax(width, line.naturalTextWidth());
	++ visibleLines;
  }
  layout.endLayout();

  // draw background for selected item
  QRectF boundRect = layout.boundingRect();
  boundRect.setWidth(width);
  boundRect.moveTo(textRect.x() + (textRect.width() - width)/2, textRect.y());

  if(!painter) { // no painter, calculate the bounding rect only
	textRect = boundRect;
	return;
  }

  QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
  if(opt.state & QStyle::State_Selected) {
	painter->fillRect(boundRect, opt.palette.highlight());
	painter->setPen(opt.palette.color(cg, QPalette::HighlightedText));
  }
  else
	painter->setPen(opt.palette.color(cg, QPalette::Text));

  // draw text
  for(int i = 0; i < visibleLines; ++i) {
	QTextLine line = layout.lineAt(i);
	if(i == (visibleLines - 1) && !elidedText.isEmpty()) { // the last line, draw elided text
	  QPointF pos(textRect.x() + line.position().x(), textRect.y() + line.y() + line.ascent());
	  painter->drawText(pos, elidedText);
	}
	else {
	  line.draw(painter, textRect.topLeft());
	}
  }

  if(opt.state & QStyle::State_HasFocus) {
	// draw focus rect
	QStyleOptionFocusRect o;
	o.QStyleOption::operator=(opt);
	o.rect = boundRect.toRect(); // subElementRect(SE_ItemViewItemFocusRect, vopt, widget);
	o.state |= QStyle::State_KeyboardFocusChange;
	o.state |= QStyle::State_Item;
	QPalette::ColorGroup cg = (opt.state & QStyle::State_Enabled)
				  ? QPalette::Normal : QPalette::Disabled;
	o.backgroundColor = opt.palette.color(cg, (opt.state & QStyle::State_Selected)
								  ? QPalette::Highlight : QPalette::Window);
	if (const QWidget* widget = opt.widget) {
	  QStyle* style = widget->style() ? widget->style() : qApp->style();
	  style->drawPrimitive(QStyle::PE_FrameFocusRect, &o, painter, widget);
	}
  }
}
