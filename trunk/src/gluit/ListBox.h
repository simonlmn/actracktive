/*
 * ListBox.h
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

#ifndef GLUITLISTBOX_H_
#define GLUITLISTBOX_H_

#include "gluit/ScrollPanel.h"
#include <vector>

namespace gluit
{

	class ListBox: public Component
	{
	public:
		typedef boost::shared_ptr<ListBox> Ptr;

		ChangeEvent<std::string>::Signal onSelectionChange;

		static const std::string NO_SELECTION_ITEM;
		static const unsigned int NO_SELECTION;

		ListBox();

		virtual void addItem(const std::string& item);
		virtual void removeItem(const std::string& item);
		virtual unsigned int getItemCount() const;
		virtual void clearItems();

		virtual void setSelectedItem(const std::string& itemToSelect);
		virtual void setSelectedItemAt(unsigned int selectedItem);
		virtual const std::string& getSelectedItem() const;

	protected:
		virtual void initialize();

	private:
		ScrollPanel::Ptr panel;

		std::vector<std::string> items;
		std::map<std::string, Panel::Ptr> itemPanels;
		unsigned int selectedItem;

		void rebuildList();
		const std::string& getItem(unsigned int position) const;

	};

}

#endif
