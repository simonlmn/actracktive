/*
 * ListBox.cpp
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

#include "gluit/ListBox.h"
#include "gluit/GridLayout.h"
#include "gluit/VerticalLayout.h"
#include "gluit/Label.h"
#include "gluit/Utils.h"
#include <boost/bind.hpp>

namespace gluit
{

	class SetSelectedItem
	{
		typedef boost::weak_ptr<ListBox> ListBoxWeakPtr;

		ListBoxWeakPtr listBox;
		const std::string item;

	public:
		SetSelectedItem(ListBoxWeakPtr listBox, const std::string& item)
			: listBox(listBox), item(item)
		{
		}

		void operator()(const MouseEvent&)
		{
			listBox.lock()->setSelectedItem(item);
		}

	};

	const std::string ListBox::NO_SELECTION_ITEM = "";
	const unsigned int ListBox::NO_SELECTION = 0;

	ListBox::ListBox()
		: Component(boost::make_shared<GridLayout>(1, 1)), panel(Component::create<ScrollPanel>()), items(), itemPanels(),
			selectedItem(NO_SELECTION)
	{
	}

	void ListBox::initialize()
	{
		Component::initialize();

		panel->setPadding(0);
		panel->setLayout(boost::make_shared<VerticalLayout>(Component::ALIGN_LEFT, 0));
		panel->setScrollHorizontally(false);

		add(panel);
	}

	void ListBox::addItem(const std::string& item)
	{
		items.push_back(item);
		rebuildList();
	}

	void ListBox::removeItem(const std::string& itemToRemove)
	{
		if ((items.size() - 1) < selectedItem) {
			setSelectedItemAt(items.size() - 1);
		}

		for (std::vector<std::string>::iterator item = items.begin(); item != items.end(); ++item) {
			if (*item == itemToRemove) {
				items.erase(item);
			}
		}
		rebuildList();
	}

	unsigned int ListBox::getItemCount() const
	{
		return items.size();
	}

	void ListBox::clearItems()
	{
		setSelectedItemAt(NO_SELECTION);
		items.clear();
		rebuildList();
	}

	void ListBox::setSelectedItem(const std::string& itemToSelect)
	{
		unsigned int selectedItem = NO_SELECTION;
		for (std::vector<std::string>::iterator item = items.begin(); item != items.end(); ++item) {
			selectedItem += 1;
			if (*item == itemToSelect) {
				setSelectedItemAt(selectedItem);
				return;
			}
		}
	}

	void ListBox::setSelectedItemAt(unsigned int selectedItem)
	{
		unsigned int oldSelectedItem = this->selectedItem;

		if (selectedItem > items.size()) {
			selectedItem = items.size();
		}

		this->selectedItem = selectedItem;

		if (selectedItem != oldSelectedItem) {
			const std::string& oldItem = getItem(oldSelectedItem);
			const std::string& newItem = getItem(selectedItem);

			if (oldItem != NO_SELECTION_ITEM) {
				itemPanels[oldItem]->setColor(Color::WHITE);
			}

			if (newItem != NO_SELECTION_ITEM) {
				itemPanels[newItem]->setColor(Color::TEXT_HIGHLIGHT);
			}

			ChangeEvent<std::string> change(shared_from_this(), "selectedItem", oldItem, newItem);
			onSelectionChange(change);
		}

		repaint();
	}

	const std::string& ListBox::getSelectedItem() const
	{
		return getItem(selectedItem);
	}

	void ListBox::rebuildList()
	{
		panel->removeAll();
		itemPanels.clear();

		unsigned int index = 1;
		for (std::vector<std::string>::iterator item = items.begin(); item != items.end(); ++item) {
			Panel::Ptr itemPanel = Component::create<Panel>();
			itemPanel->setBorder(boost::make_shared<EmptyBorder>());
			if (index == selectedItem) {
				itemPanel->setColor(Color::TEXT_HIGHLIGHT);
			} else {
				itemPanel->setColor(Color::WHITE);
			}

			itemPanel->add(Component::create<Label>(*item));
			itemPanel->onMouseClick.connect(SetSelectedItem(boost::static_pointer_cast<ListBox>(shared_from_this()), *item));

			panel->add(itemPanel);
			itemPanels[*item] = itemPanel;

			index += 1;
		}
	}

	const std::string& ListBox::getItem(unsigned int position) const
	{
		if (position > 0 && position <= items.size()) {
			return items[position - 1];
		} else {
			return NO_SELECTION_ITEM;
		}
	}

}
