/*
 * MessageDialog.h
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

#ifndef MESSAGEDIALOG_H_
#define MESSAGEDIALOG_H_

#include "gluit/Window.h"
#include "gluit/Event.h"
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace gluit
{

	class MessageDialog: public boost::noncopyable, public boost::enable_shared_from_this<MessageDialog>
	{
	public:
		typedef boost::shared_ptr<MessageDialog> Ptr;

		SimpleSignal onClose;

		static Ptr create(const std::string& title, const std::string& message);

		virtual void open(Window::Ptr parent = Window::Ptr());
		virtual void close();

	protected:
		MessageDialog(const std::string& title, const std::string& message);

	private:
		Window::Ptr window;
		std::string title;
		std::string message;

		void windowDisposed();

	};

}

#endif
