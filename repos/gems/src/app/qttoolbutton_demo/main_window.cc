
// Genode includes
#include <base/log.h>
#include <util/xml_node.h>

// local includes
#include "main_window.h"


namespace QToolButton_demo
{
	using namespace Genode;
}


QToolButton_demo::Main_window::Main_window(Env& env) noexcept :
	QMainWindow { nullptr },
	_env { env }
{
	_ui.setupUi(this);

	// display content that was read before connecting the SIGNAL
	_update_buttons();
}


void QToolButton_demo::Main_window::_update_buttons() noexcept
{
	static unsigned int xpos { 0 };
	static unsigned int ypos { 0 };

	// add button to list
	Launcher_button* button = _button_factory.insert("test 1");
	button->setParent(_ui.widget_slices);
	_ui.layout_slices->addWidget(button, ypos, xpos);

	// mark if slice is running or not
	button->setProperty("enable", true);

	// force application of style changes
	button->style()->unpolish(button);
	button->style()->polish(button);
}
