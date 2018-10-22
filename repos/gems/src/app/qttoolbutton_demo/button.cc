
// Qt includes
#include <QVariant>

// local includes
#include "button.h"


void QToolButton_demo::Launcher_button::_setup() noexcept
{
	QSizePolicy size_policy  { QSizePolicy::Fixed, QSizePolicy::Fixed };

	setText(_name.c_str());
	setObjectName(button_name(_name).c_str());
	size_policy.setHeightForWidth(sizePolicy().hasHeightForWidth());
	setSizePolicy(size_policy);
//	setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

	setProperty("type", QVariant("launcher"));
}
