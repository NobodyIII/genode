#ifdef TEST_LINUX
#include <QDir>
#include <QDebug>

#include "test_linux.h"
#include "genodefs.h"

QList<QFile> open_nodes;
QDir rootdir("/");

GenodeFS::GenodeFS(QString label)
{

}

GenodeFS::~GenodeFS()
{

}

QString GenodeFS::getlabel()
{
	return "main";
}

NodeStatus GenodeFS::stat(QString path)
{
	QFileInfo info(path);
	info.refresh();
	NodeStatus status(info.fileName(), info.isDir()? FSNode::NODE_TYPE_DIR : FSNode::NODE_TYPE_FILE, info.size());
	return status;
}

QList<FSNode> GenodeFS::dir(QString dirpath)
{
	qDebug() << "GenodeFS::dir(): " << dirpath;
	QList<FSNode> nodes;
	QDir dir("/");
	if (!dir.cd(dirpath)) return nodes;
	foreach (QString node, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden)) {
        nodes << FSNode(getlabel()+":"+dirpath+"/"+node);
	}
	foreach (QString node, dir.entryList(QDir::Files | QDir::Hidden)) {
        nodes << FSNode(getlabel()+":"+dirpath+"/"+node);
	}
    return nodes;
}

NodeHandle GenodeFS::open(QString path, OPEN_MODE mode, bool create)
{
	qDebug() << "GenodeFS::open";
	QFile* node=new QFile(path);
	NodeHandle handle;
	bool ro=mode==MODE_R;
	qDebug() << "GenodeFS::open(): " << (ro? "R" : "W");
	if (node->open(ro? QFile::ReadOnly : QFile::WriteOnly))
		handle.file.reset(node);
	else
		delete node;
	return handle;
}

bool GenodeFS::read(NodeHandle node, void *buffer, int bytes, quint64 offset)
{
	qDebug() << "GenodeFS::read";
    qint64 bytes_read=node.file->read((char*)buffer,bytes);
	qDebug() << "bytes read: " << QString::number(bytes_read);
	return bytes_read==bytes;
}

bool GenodeFS::write(NodeHandle node, void *buffer, int bytes, quint64 offset)
{
	qDebug() << "GenodeFS::write";
    return node.file->write((char*)buffer,bytes);
}

bool GenodeFS::move(QString src, QString dest)
{
	qDebug() << "GenodeFS::move(): " << src << " -> " << dest;
	return rootdir.rename(src,dest);
}

bool GenodeFS::link(QString src, QString dest)
{
    qDebug() << "GenodeFS::link"; /* XXX add link code */
}

bool GenodeFS::del(QString path)
{
	qDebug() << "GenodeFS::del(): " << path;
	return rootdir.remove(path);
}

bool GenodeFS::close(NodeHandle handle)
{
	handle.file->close();
	return true;
}

bool GenodeFS::mkdir(QString path)
{
    qDebug() << "GenodeFS::mkdir(): " << path;
    return rootdir.mkpath(path);
}

bool GenodeFS::rmdir(QString path)
{
    qDebug() << "GenodeFS::rmdir(): " << path;
    return rootdir.rmdir(path);
}

bool GenodeFS::mkfile(QString path)
{
    qDebug() << "GenodeFS::mkfile(): " << path;
    QFile f(path);
    f.open(QFile::WriteOnly);
    f.close();
}

#endif
