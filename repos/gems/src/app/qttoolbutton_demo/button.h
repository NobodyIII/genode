
#pragma once

// Qt includes
#include <QPushButton>
#include <QToolButton>

// stdcxx includes
#include <string>


namespace QToolButton_demo
{
	class Button;

	class Launcher_button;

	using Component_name = std::string;
}


/**
 * Using the following works for both run script and 
 * src archive.
 *
class QToolButton_demo::Button : public QPushButton
*/

/**
 * This works for run script but not for src archive.
 */
class QToolButton_demo::Button : public QToolButton
{
	Q_OBJECT

	protected:

		const Component_name&  _name;

		virtual void _setup() noexcept = 0;

	public:

		bool operator ==(const Component_name& name) const noexcept
		{
			return _name == name;
		}

		bool operator ==(const Button& other) const noexcept
		{
			return _name == other._name;
		}

		Button(const Component_name& slice_name) :
			QToolButton { nullptr },
			_name { slice_name }
		{ }

		virtual ~Button() = default;

		const Component_name& name() const noexcept { return _name; }
};


class QToolButton_demo::Launcher_button : public QToolButton_demo::Button
{
	private:

		void _setup() noexcept override;

	public:

		static Component_name button_name(const Component_name& slice_name) noexcept
		{
			return Component_name("btn_launch_slice_") + slice_name;
		}

		Launcher_button(const Component_name& name) :
			Button { name }
		{
			_setup();
		}
};
