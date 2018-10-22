
#pragma once

// Qt includes
#include <QMainWindow>

// local includes
#include "button.h"
#include "button_factory.h"
#include "ui_main_window.h"


namespace Ui
{
	class QToolButton_demo;
}


namespace QToolButton_demo
{
	class Main_window;

	using Genode::Env;
}


class QToolButton_demo::Main_window : public QMainWindow
{
	Q_OBJECT

public:

	explicit Main_window(Env&) noexcept;
	virtual ~Main_window() noexcept { }

private:

	void _update_buttons() noexcept;

	using Button_factory = QToolButton_demo::Button_factory<QToolButton_demo::Launcher_button>;

	Ui::QToolButton_demo  _ui;

	Env&                  _env;

	Button_factory        _button_factory { _env };
};
