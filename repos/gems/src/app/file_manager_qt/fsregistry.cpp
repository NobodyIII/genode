#include "fsregistry.h"
#include "genodefs.h"

static QList<GenodeFS*> filesystems;

GenodeFS *FSRegistry::get_fs(QString fsname)
{
    try  {
        foreach (GenodeFS *fs, filesystems) {
            if (fs->getlabel()==fsname) return fs;
        }
        GenodeFS *fs=new GenodeFS(fsname);
        filesystems << fs;
        return fs;
	} catch (...) {
        return 0;
    }
}

bool FSRegistry::detach_fs(QString fsname)
{
    foreach (GenodeFS *fs, filesystems) {
        if (fs->getlabel()==fsname) {
            filesystems.removeAll(fs);
            delete fs;
            return true;
        }
    }
    return false;
}
