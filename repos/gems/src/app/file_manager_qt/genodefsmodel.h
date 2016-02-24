#ifndef GENODEFSMODEL_H
#define GENODEFSMODEL_H

#include <QtGui>
#include <QFileIconProvider>
#include "fsnode.h"
#include "fsregistry.h"

class GenodeFSModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum SortingAttribute {SORT_NAME, SORT_SIZE};
    GenodeFSModel();
    void updateNodes(QList<FSNode> nodes);
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QStringList mimeTypes() const Q_DECL_OVERRIDE;
    QMimeData *mimeData(const QModelIndexList &indices) const Q_DECL_OVERRIDE;
    bool dropMimeData(const QMimeData *data,Qt::DropAction action,int row,int column,const QModelIndex & parent) Q_DECL_OVERRIDE;
    Qt::DropActions supportedDropActions() const Q_DECL_OVERRIDE;
    QString getcwd();
	void cut(QModelIndexList indices);
	void copy(QModelIndexList indices);
	void paste();
	void del(QModelIndexList indices);
	void rename(QModelIndex index, QString new_name);
	QModelIndex new_folder();
	QModelIndex new_file();
	void go(QString path);
	QList<FSNode> indices_to_nodes(QModelIndexList indices);
    void setSorting(SortingAttribute attr, int dir=1);

public slots:
    void up();
    void back();
    void forward();
    void refresh();
	void open(QModelIndex index);

private:
    static QString url_to_string(QUrl url);
    static QUrl string_to_url(QString str);
    static QStringList urls_to_strings(QList<QUrl> urls);
    static QList<QUrl> strings_to_urls(QStringList strs);
    //bool nodeComp(const FSNode *node1, const FSNode *node2);
    FSNode rootItem;
    QList<FSNode> nodes;
	QList<FSNode> cut_copy_list;
	bool cutting=false;
    QStringList history;
    int hist_pos=-1;
    SortingAttribute sort_attr=SORT_NAME;
    int sort_dir=1;
    //GenodeFSBackend backend; //change to pointer?
};

#endif // GENODEFSMODEL_H
