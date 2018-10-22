#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QDesktopWidget>

// Genode includes
#include <base/log.h>
#include <libc/component.h>

// local includes
#include "main_window.h"

extern void initialize_qt_core(Genode::Env &);
extern void initialize_qt_gui(Genode::Env &);

namespace QToolButton_demo {

}


void Libc::Component::construct(Libc::Env &env)
{
	Libc::with_libc([&] {

		initialize_qt_core(env);
		initialize_qt_gui(env);

		int argc = 1;
		char const *argv[] = { "QToolButton_demo", 0 };

		QApplication app(argc, (char**)argv);

		QToolButton_demo::Main_window main_window { env };

		main_window.showNormal();

		app.exec();
	});
}
