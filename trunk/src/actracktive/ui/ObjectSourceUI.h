/*
 * ObjectSourceUI.h
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

#ifndef OBJECTSOURCEUI_H_
#define OBJECTSOURCEUI_H_

#include "actracktive/ui/NodeUI.h"
#include "actracktive/processing/nodes/ObjectSource.h"
#include "actracktive/processing/nodes/Object.h"
#include "gluit/Panel.h"
#include "gluit/Layout.h"

class ObjectRendererLayout: public gluit::Layout
{
public:
	virtual gluit::Size layout(ComponentPtr component);
};

class ObjectRenderer: public gluit::Panel
{
	friend class ObjectRendererLayout;
public:
	typedef boost::shared_ptr<ObjectRenderer> Ptr;

	ObjectRenderer(Source<Objects>* source);
	virtual ~ObjectRenderer();

protected:
	virtual void initialize();
	virtual void paintComponent(gluit::Graphics g);

private:
	Source<Objects>* source;
	Objects objects;

	void handleSourceDataUpdate(const Source<Objects>& source);

};

class ObjectSourceUI: public NodeUI
{
public:
	ObjectSourceUI(Node* node, gluit::Component::Ptr largeNodeDisplay);

protected:
	virtual void initialize();

private:
	void setLargeNodeDisplay();

};

#endif
