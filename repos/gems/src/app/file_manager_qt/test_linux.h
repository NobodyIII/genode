#ifndef TEST_LINUX_H
#define TEST_LINUX_H
#include <QFile>
#include <QSharedPointer>

struct NodeHandle { /* use typedef on genode */
    QSharedPointer<QFile> file;
    bool valid() {return file->exists();}
    //~NodeHandle() {delete file;}
};

#endif // TEST_LINUX_H

