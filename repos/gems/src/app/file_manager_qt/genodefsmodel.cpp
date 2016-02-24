#include <QStyle>
#include <QStyleOption>
#include <algorithm>
#include <functional>
#include "genodefsmodel.h"
#include "fsnode.h"

struct nodeComp {
    GenodeFSModel::SortingAttribute sort_attr;
    int sort_dir;
    nodeComp(GenodeFSModel::SortingAttribute attr, int dir) {
        sort_attr=attr;
        sort_dir=dir;
    }

    bool operator()(const FSNode &node1, const FSNode &node2) {
        if (node1.is_dir() && !node2.is_dir()) return true;
        if (node2.is_dir() && !node1.is_dir()) return false;
        if (sort_attr==GenodeFSModel::SORT_NAME || (sort_attr==GenodeFSModel::SORT_SIZE && node1.is_dir())) /* no need to check node2 at this point */
            return node1.getname().compare(node2.getname(),Qt::CaseInsensitive)*sort_dir<0;
        else if (sort_attr==GenodeFSModel::SORT_SIZE) {
            bool ret=(node1.getsize()<node2.getsize());
            if (sort_dir>0)
                return ret;
            else
                return !ret;
        }
        return false;
    }
};

void GenodeFSModel::updateNodes(QList<FSNode> nodes)
{
    qDebug() << "updating";
    beginResetModel();
    std::sort(nodes.begin(),nodes.end(),nodeComp(sort_attr,sort_dir));
    this->nodes=nodes;
    endResetModel();
}

GenodeFSModel::GenodeFSModel()
{
    go("main:/");
}

QModelIndex GenodeFSModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row,column,(quintptr)0);
    /*qDebug() << "creating index:" << row << column;
    if(parent.isValid() && parent.column() != 0)
        return QModelIndex();

    if (row>nodes.size()-1)
        return QModelIndex();

    FSNode *childItem = const_cast<FSNode*>(&nodes[row]);
        if(childItem) return createIndex(row, column, childItem);

    return QModelIndex();*/
}

int GenodeFSModel::rowCount(const QModelIndex &parent) const
{
    return nodes.size();
}

int GenodeFSModel::columnCount(const QModelIndex &parent) const
{
    return (parent.column() > 0) ? 0 : 3;
}

QVariant GenodeFSModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    //FSNode* node=static_cast<FSNode*>(index.internalPointer());
    FSNode node=nodes.value(index.row());
    if(role == Qt::DisplayRole)
    {
        switch(index.column()) {
        case 0:
            return node.getname(); //+"\n("+QString::number(node.getsize())+")";
            break;
        case 1:
            return node.gettype();
            break;
        case 2:
            return node.getsize();
            break;
        }
    } else if (role==Qt::DecorationRole) {
        if (index.column()==0) {
            //QStyleOptionGraphicsItem op;
            QFileIconProvider p;

            if (node.is_dir())
                return p.icon(QFileIconProvider::Folder);
            else
                return p.icon(QFileIconProvider::File);
        }
    } else if (role==Qt::EditRole) {
        return node.getname();
    } else if (role==Qt::StatusTipRole) {
        return node.getname();
    } else if (role==Qt::TextAlignmentRole) {
        return Qt::AlignHCenter + Qt::AlignTop;
    }

    return QVariant();
}

QVariant GenodeFSModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole)
    switch(section) {
    case 0: return tr("Name");
    case 1: return tr("Type");
    case 2: return tr("Size");
    default: return QVariant();
    }

    return QVariant();
}

Qt::ItemFlags GenodeFSModel::flags(const QModelIndex &index) const
{
    if(!index.isValid()) return 0;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

QModelIndex GenodeFSModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

QStringList GenodeFSModel::mimeTypes() const {
    return QStringList("text/uri-list");
}

Qt::DropActions GenodeFSModel::supportedDropActions() const {
    return Qt::CopyAction | Qt::MoveAction;
}

QMimeData *GenodeFSModel::mimeData(const QModelIndexList &indices) const {
    QMimeData *data=new QMimeData();
    QList<QUrl> urls;
    foreach (QModelIndex index, indices) {
        if (index.isValid())
            urls << (nodes.value(index.row()).to_url());
    }
    data->setUrls(urls);
    return data;
}

bool GenodeFSModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    qDebug() << "dropping:" << parent.row();
    if (!data->hasUrls() || !parent.isValid() || (action!=Qt::MoveAction && action!=Qt::CopyAction) || parent.row()<0) return false;
    //FSNode *newdir=static_cast<FSNode*>(parent.internalPointer());
    FSNode newdir=nodes.value(parent.row());
    if (!newdir.is_valid() || !newdir.is_dir() || newdir.getpath()==rootItem.getpath()) return false;
    foreach (QUrl url, data->urls()) {
        FSNode node(url);
        if (node.is_valid() && node.getpath()!=newdir.getpath()) {
            if (action==Qt::MoveAction)
                node.move(newdir);
            else if (action==Qt::CopyAction)
                node.copy(newdir);
        }
    }
    refresh();
}

QString GenodeFSModel::url_to_string(QUrl url)
{
    return url.toLocalFile();
}

QUrl GenodeFSModel::string_to_url(QString str)
{
    return QUrl("file://"+str);
}

QStringList GenodeFSModel::urls_to_strings(QList<QUrl> urls)
{
    QStringList strs;
    foreach (QUrl url, urls) {
        strs << url.toLocalFile();
    }
    return strs;
}

QList<QUrl> GenodeFSModel::strings_to_urls(QStringList strs)
{
    QList<QUrl> urls;
    foreach (QString str, strs) {
        urls << string_to_url(str);
    }
    return urls;
}

QString GenodeFSModel::getcwd()
{
    return rootItem.getpath();
}

void GenodeFSModel::up()
{
    go("..");
}

void GenodeFSModel::back()
{
    if (hist_pos>0) {
        --hist_pos;
        rootItem.chdir(history[hist_pos]);
        refresh();
    }
}

void GenodeFSModel::forward()
{
    if (hist_pos<history.size()-1) {
        ++hist_pos;
        rootItem.chdir(history[hist_pos]);
        refresh();
    }
}

void GenodeFSModel::refresh()
{
    updateNodes(rootItem.dir());
}

void GenodeFSModel::cut(QModelIndexList indices)
{
    cut_copy_list=indices_to_nodes(indices);
    cutting=true;
}

void GenodeFSModel::copy(QModelIndexList indices)
{
    cut_copy_list=indices_to_nodes(indices);
    cutting=false;
}

void GenodeFSModel::paste() /* paste into cwd */
{
    qDebug() << "pasting" << cut_copy_list.size() << "items";
    foreach (FSNode node,cut_copy_list) {
        qDebug() << "paste: " << node.getpath();
        if (node.is_valid()) {
            if (cutting)
                node.move(rootItem);
            else
                node.copy(rootItem);
        }
    }
    refresh();
}

void GenodeFSModel::del(QModelIndexList indices)
{
    foreach (QModelIndex index,indices) {
        if (index.isValid()) {
            //FSNode *node=static_cast<FSNode*>(index.internalPointer());
            FSNode node=nodes.value(index.row());
            qDebug() << "delete: " << node.getfspath();
            if (node.is_valid()) node.rm();
        }
    }
    refresh();
}

void GenodeFSModel::rename(QModelIndex index, QString new_name)
{
    if (index.isValid()) {
        //FSNode *node=static_cast<FSNode*>(index.internalPointer());
        FSNode node=nodes.value(index.row());
        if (node.is_valid()) node.rename(new_name);
        refresh();
    }
}

QModelIndex GenodeFSModel::new_folder()
{
    QString new_name="New Folder";
    rootItem.mkdir(new_name);
    refresh();
    for (int i = 0; i < nodes.size(); ++i) {
        FSNode &node=nodes[i];
        if (node.getname()==new_name) return index(i,0);
    }
    return QModelIndex();
}

QModelIndex GenodeFSModel::new_file()
{
    QString new_name="New File";
    rootItem.mkfile(new_name);
    refresh();
    for (int i = 0; i < nodes.size(); ++i) {
        FSNode &node=nodes[i];
        if (node.getname()==new_name) return index(i,0);
    }
    return QModelIndex();
}

void GenodeFSModel::go(QString path)
{
    QString prev_path=rootItem.getpath();
    rootItem.chdir(path);
    while (history.size()-1>hist_pos) {
        history.removeLast();
    }
    if (rootItem.getpath()!=prev_path) {
        history << rootItem.getpath(); /* handles .. nicely */
        ++hist_pos;
    }
    refresh();
}

void GenodeFSModel::open(QModelIndex index)
{
    if (index.isValid()) {
        //FSNode *node=static_cast<FSNode*>(index.internalPointer());
        FSNode node=nodes.value(index.row());
        if (node.is_valid() && node.is_dir()) go(node.getpath());
    }
}

QList<FSNode> GenodeFSModel::indices_to_nodes(QModelIndexList indices)
{
    QList<FSNode> new_nodes;
    foreach (QModelIndex index, indices) {
        //FSNode* node=static_cast<FSNode*>(index.internalPointer());
        qDebug() << "indices_to_nodes():" << index.row();
        FSNode node=nodes.value(index.row());
        if (node.is_valid()) new_nodes << node; /* copies node to list */
    }
    return new_nodes;
}

bool GenodeFSModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    //FSNode* node=static_cast<FSNode*>(index.internalPointer());
    FSNode node=nodes.value(index.row());
    if (node.is_valid()) node.rename(value.toString());
    refresh();
}

void GenodeFSModel::setSorting(SortingAttribute attr, int dir) {
    sort_attr=attr;
    sort_dir=dir;
    refresh();
}
