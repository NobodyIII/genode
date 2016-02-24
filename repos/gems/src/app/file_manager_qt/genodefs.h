#ifndef GENODEFS_H
#define GENODEFS_H

#ifndef TEST_LINUX

#include <base/allocator_avl.h>
#include <file_system_session/connection.h>

//using namespace Genode; /* conflicting definitions (e.g. memcpy, strlen) */
//using namespace File_system;

typedef File_system::File_handle NodeHandle; /* XXX change NodeHandle to FileHandle */

#endif

#include "fsnode.h"

class ERR_NO_FS {};

struct NodeStatus
{
    QString name="invalid";
    FSNode::TYPE type;
    quint64 size;
    NodeStatus() { }
    NodeStatus(QString node_name, FSNode::TYPE node_type, quint64 node_size): name(node_name), type(node_type), size(node_size) { }
};

class GenodeFS
{
public:
    enum OPEN_MODE {MODE_R=1, MODE_W=2}; //, MODE_RW=3};/* MODE_RW = MODE_R | MODE_W */
    GenodeFS(QString label);
    ~GenodeFS();
    QString getlabel();
    NodeStatus stat(QString path);
    QList<FSNode> dir(QString dirpath);
    NodeHandle open(QString path, OPEN_MODE mode, bool create);
    bool read(NodeHandle node, void *buffer, int bytes, quint64 offset=0); /* use stat to determine size */
    bool write(NodeHandle node, void *buffer, int bytes, quint64 offset=0);
    bool move(QString src, QString dest);
    bool link(QString src, QString dest);
    bool del(QString path);
    bool close(NodeHandle handle);
    bool mkfile(QString path);
    bool mkdir(QString path);
    bool rmdir(QString path);
#ifndef TEST_LINUX
    
private:
    File_system::Node_handle open_node(QString path);
    File_system::Dir_handle open_dir(QString path, bool create=false);
    File_system::File_handle open_file(QString path, OPEN_MODE mode, bool create=false);
    File_system::Dir_handle parent_dir(QString path);
    QString last_element(QString path);
    
    Genode::Allocator_avl alloc;
    File_system::Connection fs;
    QString label;
    
#endif
};

#endif // GENODEFS_H
