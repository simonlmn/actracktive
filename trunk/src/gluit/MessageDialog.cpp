/*
 * MessageDialog.cpp
 *
 * Copyright (C) 2012 Simon Lehmann
 *
 * This file is part of Actracktive.
 *
 * Actracktive is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Actracktive is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MessageDialog.h"
#include "gluit/Graphics.h"
#include "gluit/Component.h"
#include "gluit/BorderLayout.h"
#include "gluit/VerticalLayout.h"
#include "gluit/HorizontalLayout.h"
#include "gluit/Label.h"
#include "gluit/Button.h"
#include "gluit/Panel.h"
#include "gluit/Toolkit.h"
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>

namespace gluit
{

	static void messageDialogShown(Window::WeakPtr weak)
	{
		Window::Ptr window = weak.lock();
		if (!window) {
			return;
		}

		window->pack();

		const Size& windowSize = window->getSize();
		const Size& screenSize = getScreenSize();

		window->setPosition(Point((screenSize.width - windowSize.width) / 2, (screenSize.height - windowSize.height) / 2));
	}

	MessageDialog::Ptr MessageDialog::create(const std::string& title, const std::string& message)
	{
		Ptr dialog = Ptr(new MessageDialog(title, message));
		return dialog;
	}

	MessageDialog::MessageDialog(const std::string& title, const std::string& message)
		: window(), title(title), message(message)
	{
	}

	void MessageDialog::open(Window::Ptr parent)
	{
		if (window) {
			return;
		}

		window = Window::create(title);
		window->onShow.connect(boost::bind(&messageDialogShown, Window::WeakPtr(window)));
		window->onDispose.connect(SimpleSignal::slot_type(&MessageDialog::windowDisposed, this).track(shared_from_this()));
		window->setLayout(boost::make_shared<BorderLayout>());

		Panel::Ptr messagePanel = Component::create<Panel>();
		messagePanel->setLayout(boost::make_shared<VerticalLayout>());
		messagePanel->setAlignment(Component::ALIGN_CENTER);
		messagePanel->setPadding(Insets(20));

		Label::Ptr messageLabel = Component::create<Label>();
		messageLabel->setMultiline(true);
		messageLabel->setText(message);
		messagePanel->add(messageLabel);

		window->add(messagePanel);

		Panel::Ptr buttonPanel = Component::create<Panel>();
		buttonPanel->setLayout(boost::make_shared<VerticalLayout>());
		buttonPanel->setAlignment(Component::ALIGN_BOTTOM);
		buttonPanel->setPadding(Insets(20, 0, 20, 20));

		Button::Ptr ok = gluit::Component::create<Button>();
		ok->setAlignment(Component::ALIGN_CENTER);
		ok->setStretchable(false);
		ok->setText("OK");
		ok->onButtonPress.connect(boost::bind(&Window::dispose, window.get()));
		buttonPanel->add(ok);

		window->add(buttonPanel);

		if (parent) {
			parent->onDispose.connect(SimpleSignal::slot_type(&Window::dispose, window.get()).track(window));
		}

		window->setVisible(true);
	}

	void MessageDialog::close()
	{
		if (window) {
			window->dispose();
		}
	}

	void MessageDialog::windowDisposed()
	{
		onClose();
		window.reset();
		window->onDispose.disconnect_all_slots();
	}

}
