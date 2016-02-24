#include <QDebug>
#include "fsnode.h"
#include "fsregistry.h"
#include "genodefs.h"

QString FSNode::canonicalize_path(QString path)
{
    path.append('/'); //for dotdot
    for (;;) {
        int slashslash=path.indexOf("//");
        if (slashslash==-1) break;
        path.remove(slashslash,1);
    }
    for (;;) {
        int slashdot=path.indexOf("/./");
        if (slashdot==-1) break;
        path.remove(slashdot,2);
    }
    for (;;) {
        int dotdot=path.indexOf("/../");
        if (dotdot==-1) break;
        int parent=path.lastIndexOf('/',dotdot-1);
        path.remove(parent,dotdot-parent+3);
    }
    if (path.mid(path.length()-2,1)!=":") path.remove(path.length()-1,1); /* strip ending '/' */
    return path;
}

void FSNode::setpath(QString path)
{
    path=canonicalize_path(path);
    node_full_path=path;
    int label_end=path.indexOf(":/");
    node_fs=path.left(label_end);
    fs=FSRegistry::get_fs(node_fs);
    if (fs==0) {qDebug() << "invalid fs: " << node_fs; valid=false; return;}
    node_fs_path=path.mid(label_end+1);
    qDebug() << "begin stat";
    NodeStatus stat=fs->stat(node_fs_path);
    qDebug() << "end stat";
    if (stat.name=="invalid") {qDebug() << "invalid node: " << node_full_path << " (" << node_fs_path << ")"; valid=false; return;}
    int parent_end=path.lastIndexOf('/');
    parentdir_path=path.left(parent_end);
    if (parentdir_path=="") parentdir_path=node_fs+":/";
    node_name=stat.name;
    node_type=stat.type;
    node_size=stat.size;
    valid=true;
}

FSNode::FSNode(): valid(false) { }

FSNode::FSNode(QString path)
{
    setpath(path);
}

FSNode::FSNode(QUrl url)
{
    setpath(url.toLocalFile());
}

QString FSNode::getname() const
{
    return node_name;
}

QString FSNode::getpath() const
{
    return node_full_path;
}

quint64 FSNode::getsize() const
{
    return node_size;
}

QString FSNode::getfsname() const
{
    return node_fs;
}

QString FSNode::getfspath() const
{
    return node_fs_path;
}

FSNode::TYPE FSNode::gettype() const
{
    return node_type;
}

QUrl FSNode::to_url() const
{
    return QUrl::fromLocalFile(node_full_path);
}

QList<FSNode> FSNode::dir() const
{
    return fs->dir(node_fs_path);
}

bool FSNode::chdir(QString dir)
{
    qDebug() << "chdir(): " << dir;
    if (dir.contains(":/")) {
        setpath(dir);
    } else if (dir.left(1)=="/") {
        if (!valid) return false;
        setpath(node_fs+":"+dir);
    } else {
        if (!valid) return false;
        qDebug() << "chdir(): relative path";
        QString newdir=node_full_path+"/"+dir;
        setpath(canonicalize_path(newdir));
    }
    return valid;
}

bool FSNode::_copy(GenodeFS* targetfs, QString targetfspath)
{
    qDebug() << "FSNode::_copy():" << node_fs_path << targetfspath << node_size;
    void* buffer=malloc(buffer_size);
    quint64 bytes_remaining=node_size;
    quint64 offset=0;
    int readlen;
    if (node_size==0) return false;
    NodeHandle src=fs->open(node_fs_path,GenodeFS::MODE_R,false);
    /* XXX add check to prevent overwriting */
    NodeHandle dest=targetfs->open(targetfspath,GenodeFS::MODE_W,true);
    if (!(src.valid() && dest.valid())) {
        qDebug() << "_copy(): src valid?:" << src.valid() << "dest valid?:" << dest.valid();
        fs->close(src);
        targetfs->close(dest);
        free(buffer);
        return false;
    }
    while (bytes_remaining) {
        readlen = bytes_remaining>buffer_size? buffer_size:bytes_remaining;
        if (!fs->read(src,buffer,readlen,offset) || !targetfs->write(dest,buffer,readlen,offset)) {
            fs->close(src);
            targetfs->close(dest);
            free(buffer);
            return false;
        }
        bytes_remaining-=readlen;
        offset+=readlen;
    }
    fs->close(src);
    targetfs->close(dest);
    free(buffer);
    return true;
}

bool FSNode::_copy_recursive(GenodeFS* targetfs, QString targetfspath)
{
    QList<FSNode> children=dir();
    targetfs->mkdir(canonicalize_path(targetfspath)); /* fixes "//" */
    foreach (FSNode child, children) {
        child.copy(FSNode(targetfs->getlabel()+":"+targetfspath));
    }
}

bool FSNode::copy(QString newname)
{
    if (node_type==NODE_TYPE_DIR) {
        return _copy_recursive(fs,to_fspath(canonicalize_path(parentdir_path+"/"+newname)));
    } else return _copy(fs,to_fspath(canonicalize_path(parentdir_path+"/"+newname)));
}

bool FSNode::copy(FSNode targetdir) {
    if (node_type==NODE_TYPE_DIR) return _copy_recursive(targetdir.fs,targetdir.node_fs_path+"/"+node_name);
    GenodeFS* targetfs=targetdir.fs;
    return _copy(targetfs,targetdir.node_fs_path+"/"+node_name);

}

bool FSNode::move(FSNode targetdir) {
    if (targetdir.node_fs==node_fs) return fs->move(node_fs_path,targetdir.node_fs_path+"/"+node_name);
    if (copy(targetdir) && rm()) return true;
    return false;
}

bool FSNode::rename(QString newname)
{
    if (newname.contains('/')) return false;
    QString newpath=to_fspath(canonicalize_path(parentdir_path+"/"+newname));
    return fs->move(node_fs_path,newpath);
}

bool FSNode::link(QString linkpath)
{
    if (linkpath.contains(":/")) return false;
    return fs->link(node_fs_path,linkpath);
}

bool FSNode::is_valid() const
{
    return valid;
}

bool FSNode::is_dir() const
{
    return node_type & NODE_TYPE_DIR;
}

bool FSNode::is_file() const
{
    return node_type & NODE_TYPE_FILE;
}

bool FSNode::is_symlink() const
{
    return node_type & ANY_SYMLINK;
}

bool FSNode::_rm()
{
    if (fs->del(node_fs_path)) {
        valid=false;
        return true;
    }
    return false;
}

bool FSNode::_rm_recursive()
{
    QList<FSNode> children=dir();
    foreach (FSNode child, children) {
        child.rm();
    }
    fs->rmdir(node_fs_path);
}

bool FSNode::rm()
{
    if (node_type==NODE_TYPE_DIR)
        return _rm_recursive();
    else
        return _rm();
}

bool FSNode::refresh()
{
    setpath(node_full_path);
}

QString FSNode::to_fspath(QString path)
{
    int label_end=path.indexOf(":/");
    return path.mid(label_end+1);
}

bool FSNode::is_hidden() const
{
    if (node_name.left(1)==".")
        return true;
    else
        return false;
}

bool FSNode::mkdir(QString name)
{
    if (is_dir() && is_valid()) return fs->mkdir(canonicalize_path(node_fs_path+"/"+name)); /* fixes "//" */
    return false;
}

bool FSNode::mkfile(QString name)
{
    if (is_dir() && is_valid()) return fs->mkfile(canonicalize_path(node_fs_path+"/"+name));
    return false;
}
