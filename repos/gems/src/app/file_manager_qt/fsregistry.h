#ifndef GENODEFSBACKEND_H
#define GENODEFSBACKEND_H

#include <QString>
//#include "genodefs.h"

class GenodeFS;

namespace FSRegistry
{
	GenodeFS *get_fs(QString fsname);
	bool detach_fs(QString fsname);
}

#endif // GENODEFSBACKEND_H
