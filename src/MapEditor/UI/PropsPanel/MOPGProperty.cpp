
// -----------------------------------------------------------------------------
// SLADE - It's a Doom Editor
// Copyright(C) 2008 - 2017 Simon Judd
//
// Email:       sirjuddington@gmail.com
// Web:         https://slade.mancubus.net
// Filename:    MOPGProperty.cpp
// Description: MOPGProperty and child classes - specialisations of wxProperty
//              to handle various map object property types, including display
//              and modification of values, for use with the MapObjectPropsPanel
//              grid
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110 - 1301, USA.
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
//
// Includes
//
// -----------------------------------------------------------------------------
#include "Main.h"
#include "MOPGProperty.h"
#include "Game/Configuration.h"
#include "MapEditor/MapEditContext.h"
#include "MapEditor/MapEditor.h"
#include "MapEditor/SLADEMap/SLADEMap.h"
#include "MapEditor/UI/Dialogs/ActionSpecialDialog.h"
#include "MapEditor/UI/Dialogs/MapTextureBrowser.h"
#include "MapEditor/UI/Dialogs/SectorSpecialDialog.h"
#include "MapEditor/UI/Dialogs/ThingTypeBrowser.h"
#include "MapObjectPropsPanel.h"


// -----------------------------------------------------------------------------
//
// MOPGProperty Class Functions
//
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Reloads the property value from the object(s) currently open in the parent
// MapObjectPropsPanel, if any
// -----------------------------------------------------------------------------
void MOPGProperty::resetValue()
{
	// Do nothing if no parent (and thus no object list)
	if (!parent_ || noupdate_)
		return;

	// Read value from selection
	openObjects(parent_->getObjects());
}


// -----------------------------------------------------------------------------
//
// MOPGBoolProperty Class Functions
//
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// MOPGBoolProperty class constructor
// -----------------------------------------------------------------------------
MOPGBoolProperty::MOPGBoolProperty(const wxString& label, const wxString& name) : wxBoolProperty(label, name, false)
{
	propname_ = name;
}

// -----------------------------------------------------------------------------
// Reads the value of this boolean property from [objects]
// (if the value differs between objects, it is set to unspecified)
// -----------------------------------------------------------------------------
void MOPGBoolProperty::openObjects(vector<MapObject*>& objects)
{
	// Set unspecified if no objects given
	if (objects.size() == 0)
	{
		SetValueToUnspecified();
		return;
	}

	// Get property of first object
	bool first = objects[0]->boolProperty(GetName());

	// Check whether all objects share the same value
	for (unsigned a = 1; a < objects.size(); a++)
	{
		if (objects[a]->boolProperty(GetName()) != first)
		{
			// Different value found, set unspecified
			SetValueToUnspecified();
			return;
		}
	}

	// Set to common value
	noupdate_ = true;
	SetValue(first);
	updateVisibility();
	noupdate_ = false;
}

// -----------------------------------------------------------------------------
// Default to hiding this property if set to its default value.
// -----------------------------------------------------------------------------
void MOPGBoolProperty::updateVisibility()
{
	if (!parent_->showAll() && !IsValueUnspecified() && udmf_prop_ && !udmf_prop_->showAlways()
		&& udmf_prop_->defaultValue().getBoolValue() == GetValue().GetBool())
		Hide(true);
	else
		Hide(false);
}

// -----------------------------------------------------------------------------
// Applies the current property value to all objects currently open in the
// parent MapObjectPropsPanel, if a value is specified
// -----------------------------------------------------------------------------
void MOPGBoolProperty::applyValue()
{
	// Do nothing if no parent (and thus no object list)
	if (!parent_ || noupdate_)
		return;

	// Do nothing if the value is unspecified
	if (IsValueUnspecified())
		return;

	// Go through objects and set this value
	vector<MapObject*>& objects = parent_->getObjects();
	for (unsigned a = 0; a < objects.size(); a++)
		objects[a]->setBoolProperty(GetName(), m_value.GetBool());
}


// -----------------------------------------------------------------------------
//
// MOPGIntProperty Class Functions
//
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// MOPGIntProperty class constructor
// -----------------------------------------------------------------------------
MOPGIntProperty::MOPGIntProperty(const wxString& label, const wxString& name) : wxIntProperty(label, name, 0)
{
	propname_ = name;
}

// -----------------------------------------------------------------------------
// Reads the value of this integer property from [objects]
// (if the value differs between objects, it is set to unspecified)
// -----------------------------------------------------------------------------
void MOPGIntProperty::openObjects(vector<MapObject*>& objects)
{
	// Assume unspecified until we see otherwise
	SetValueToUnspecified();
	// Set unspecified if no objects given
	if (objects.size() == 0)
	{
		return;
	}

	// Get property of first object
	int first = objects[0]->intProperty(GetName());

	// Check whether all objects share the same value
	for (unsigned a = 1; a < objects.size(); a++)
	{
		if (objects[a]->intProperty(GetName()) != first)
		{
			// Different value found, set unspecified
			SetValueToUnspecified();
			return;
		}
	}

	// Set to common value
	noupdate_ = true;
	SetValue(first);
	updateVisibility();
	noupdate_ = false;
}

// -----------------------------------------------------------------------------
// Default to hiding this property if set to its default value.
// -----------------------------------------------------------------------------
void MOPGIntProperty::updateVisibility()
{
	if (!parent_->showAll() && !IsValueUnspecified() && udmf_prop_ && !udmf_prop_->showAlways()
		&& udmf_prop_->defaultValue().getIntValue() == GetValue().GetInteger())
		Hide(true);
	else
		Hide(false);
}

// -----------------------------------------------------------------------------
// Applies the current property value to all objects currently open in the
// parent MapObjectPropsPanel, if a value is specified
// -----------------------------------------------------------------------------
void MOPGIntProperty::applyValue()
{
	// Do nothing if no parent (and thus no object list)
	if (!parent_ || noupdate_)
		return;

	// Do nothing if the value is unspecified
	if (IsValueUnspecified())
		return;

	// Go through objects and set this value
	vector<MapObject*>& objects = parent_->getObjects();
	for (unsigned a = 0; a < objects.size(); a++)
		objects[a]->setIntProperty(GetName(), m_value.GetInteger());
}


// -----------------------------------------------------------------------------
//
// MOPGFloatProperty Class Functions
//
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// MOPGFloatProperty class constructor
// -----------------------------------------------------------------------------
MOPGFloatProperty::MOPGFloatProperty(const wxString& label, const wxString& name) : wxFloatProperty(label, name, 0)
{
	propname_ = name;
}

// -----------------------------------------------------------------------------
// Reads the value of this float property from [objects]
// (if the value differs between objects, it is set to unspecified)
// -----------------------------------------------------------------------------
void MOPGFloatProperty::openObjects(vector<MapObject*>& objects)
{
	// Set unspecified if no objects given
	if (objects.size() == 0)
	{
		SetValueToUnspecified();
		return;
	}

	// Get property of first object
	double first = objects[0]->floatProperty(GetName());

	// Check whether all objects share the same value
	for (unsigned a = 1; a < objects.size(); a++)
	{
		if (objects[a]->floatProperty(GetName()) != first)
		{
			// Different value found, set unspecified
			SetValueToUnspecified();
			return;
		}
	}

	// Set to common value
	noupdate_ = true;
	SetValue(first);
	updateVisibility();
	noupdate_ = false;
}

// -----------------------------------------------------------------------------
// Default to hiding this property if set to its default value.
// -----------------------------------------------------------------------------
void MOPGFloatProperty::updateVisibility()
{
	if (!parent_->showAll() && !IsValueUnspecified() && udmf_prop_ && !udmf_prop_->showAlways()
		&& udmf_prop_->defaultValue().getFloatValue() == GetValue().GetDouble())
		Hide(true);
	else
		Hide(false);
}

// -----------------------------------------------------------------------------
// Applies the current property value to all objects currently open in the
// parent MapObjectPropsPanel, if a value is specified
// -----------------------------------------------------------------------------
void MOPGFloatProperty::applyValue()
{
	// Do nothing if no parent (and thus no object list)
	if (!parent_ || noupdate_)
		return;

	// Do nothing if the value is unspecified
	if (IsValueUnspecified())
		return;

	// Go through objects and set this value
	vector<MapObject*>& objects = parent_->getObjects();
	for (unsigned a = 0; a < objects.size(); a++)
		objects[a]->setFloatProperty(GetName(), m_value.GetDouble());
}


// -----------------------------------------------------------------------------
//
// MOPGStringProperty Class Functions
//
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// MOPGStringProperty class constructor
// -----------------------------------------------------------------------------
MOPGStringProperty::MOPGStringProperty(const wxString& label, const wxString& name) : wxStringProperty(label, name, "")
{
	propname_ = name;
}

// -----------------------------------------------------------------------------
// Load a list of possible choices from the given UDMF prop, if any
// -----------------------------------------------------------------------------
void MOPGStringProperty::setUDMFProp(UDMFProperty* prop)
{
	MOPGProperty::setUDMFProp(prop);

	// If this is a soft enum (e.g. renderstyle can be "translucent" or "add",
	// but we don't want to enforce that strictly), use a combobox populated
	// with the possible values
	if (prop && prop->hasPossibleValues())
	{
		wxPGChoices choices = wxPGChoices();

		for (auto& val : prop->possibleValues())
			choices.Add(val.getStringValue());

		SetChoices(choices);
		SetEditor(wxPGEditor_ComboBox);
	}
	// else
	//	SetEditor(wxPGEditor_TextCtrl);
}

// -----------------------------------------------------------------------------
// Reads the value of this string property from [objects]
// (if the value differs between objects, it is set to unspecified)
// -----------------------------------------------------------------------------
void MOPGStringProperty::openObjects(vector<MapObject*>& objects)
{
	// Set unspecified if no objects given
	if (objects.size() == 0)
	{
		SetValueToUnspecified();
		return;
	}

	// Get property of first object
	string first = objects[0]->stringProperty(GetName());

	// Check whether all objects share the same value
	for (unsigned a = 1; a < objects.size(); a++)
	{
		if (objects[a]->stringProperty(GetName()) != first)
		{
			// Different value found, set unspecified
			SetValueToUnspecified();
			return;
		}
	}

	// Set to common value
	noupdate_ = true;
	SetValue(first);
	updateVisibility();
	noupdate_ = false;
}

// -----------------------------------------------------------------------------
// Default to hiding this property if set to its default value.
// -----------------------------------------------------------------------------
void MOPGStringProperty::updateVisibility()
{
	if (!parent_->showAll() && !IsValueUnspecified() && udmf_prop_ && !udmf_prop_->showAlways()
		&& udmf_prop_->defaultValue().getStringValue() == GetValue().GetString())
		Hide(true);
	else
		Hide(false);
}

// -----------------------------------------------------------------------------
// Applies the current property value to all objects currently open in the
// parent MapObjectPropsPanel, if a value is specified
// -----------------------------------------------------------------------------
void MOPGStringProperty::applyValue()
{
	// Do nothing if no parent (and thus no object list)
	if (!parent_ || noupdate_)
		return;

	// Do nothing if the value is unspecified
	if (IsValueUnspecified())
		return;

	// Go through objects and set this value
	vector<MapObject*>& objects = parent_->getObjects();
	for (unsigned a = 0; a < objects.size(); a++)
		objects[a]->setStringProperty(GetName(), m_value.GetString());
}


// -----------------------------------------------------------------------------
// MOPGIntWithArgsProperty Class Functions
//
// Superclass for shared functionality between action specials and things, which
// both have arguments.
// Arguments that are used by the engine (i.e. those with names) should always
// be shown even if zero.
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// MOPGIntWithArgsProperty class constructor
// -----------------------------------------------------------------------------
MOPGIntWithArgsProperty::MOPGIntWithArgsProperty(const wxString& label, const wxString& name) :
	MOPGIntProperty(label, name)
{
	// Set to text+button editor
	SetEditor(wxPGEditor_TextCtrlAndButton);
}

// -----------------------------------------------------------------------------
// Return whether the selected special or thing type takes any arguments.
// -----------------------------------------------------------------------------
bool MOPGIntWithArgsProperty::hasArgs()
{
	return getArgspec().count > 0;
}

// -----------------------------------------------------------------------------
// Update the UI to show the names of the arguments for the current special or
// thing type, and hide those that don't have names.
// -----------------------------------------------------------------------------
void MOPGIntWithArgsProperty::updateArgs(wxPGProperty* args[5])
{
	auto     argspec       = getArgspec();
	int      default_value = 0;
	unsigned argcount;

	if (udmf_prop_)
		default_value = udmf_prop_->defaultValue().getIntValue();

	if (parent_->showAll())
		argcount = 5;
	else if (IsValueUnspecified())
		argcount = 0;
	else
		argcount = argspec.count;

	for (unsigned a = 0; a < 5; a++)
	{
		if (!args[a])
			continue;

		if (IsValueUnspecified())
		{
			args[a]->SetLabel(S_FMT("Arg%d", a + 1));
			args[a]->SetHelpString("");
		}
		else
		{
			args[a]->SetLabel(argspec[a].name);
			args[a]->SetHelpString(argspec[a].desc);
		}

		// Show any args that this special uses, hide the others, but never
		// hide an arg with a value
		args[a]->Hide(
			a >= argcount && !args[a]->IsValueUnspecified() && args[a]->GetValue().GetInteger() == default_value);
	}
}

// -----------------------------------------------------------------------------
// Applies the current property value to all objects currently open in the
// parent MapObjectPropsPanel, if a value is specified
// -----------------------------------------------------------------------------
void MOPGIntWithArgsProperty::applyValue()
{
	// Do nothing if no parent (and thus no object list)
	if (!parent_ || noupdate_)
		return;

	// Do nothing if the value is unspecified
	if (IsValueUnspecified())
		return;

	// Initialize any unset and meaningful args to 0
	const auto argspec = getArgspec();

	// Go through objects and set this value
	vector<MapObject*>& objects = parent_->getObjects();
	for (unsigned a = 0; a < objects.size(); a++)
	{
		objects[a]->setIntProperty(GetName(), m_value.GetInteger());

		for (int argn = 0; argn < argspec.count; argn++)
		{
			string key = S_FMT("arg%d", argn);
			if (!objects[a]->hasProp(key))
				objects[a]->setIntProperty(key, 0);
		}
	}
}

// -----------------------------------------------------------------------------
// Ask the parent to refresh the args display after the value changes
// -----------------------------------------------------------------------------
void MOPGIntWithArgsProperty::OnSetValue()
{
	if (parent_)
		parent_->updateArgs(this);

	MOPGIntProperty::OnSetValue();
}


// -----------------------------------------------------------------------------
// MOPGActionSpecialProperty Class Functions
//
// Property grid cell for action special properties, links to 5 other cells for
// the special args (which will update when the special value is changed)
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Returns a little object describing the args for this thing type
// -----------------------------------------------------------------------------
const Game::ArgSpec& MOPGActionSpecialProperty::getArgspec()
{
	int special = m_value.GetInteger();
	return Game::configuration().actionSpecial(special).argSpec();
}

// -----------------------------------------------------------------------------
// Returns the action special value as a string (contains the special name)
// -----------------------------------------------------------------------------
wxString MOPGActionSpecialProperty::ValueToString(wxVariant& value, int argFlags) const
{
	// Get value as integer
	int special = value.GetInteger();

	if (special == 0)
		return "0: None";
	else
		return S_FMT("%d: %s", special, Game::configuration().actionSpecial(special).name());
}

// -----------------------------------------------------------------------------
// Called when an event is raised for the control
// -----------------------------------------------------------------------------
bool MOPGActionSpecialProperty::OnEvent(wxPropertyGrid* propgrid, wxWindow* window, wxEvent& e)
{
	// '...' button clicked
	if (e.GetEventType() == wxEVT_BUTTON)
	{
		int                 special = -1;
		ActionSpecialDialog dlg(MapEditor::windowWx());
		dlg.setSpecial(GetValue().GetInteger());
		if (dlg.ShowModal() == wxID_OK)
			special = dlg.selectedSpecial();

		if (special >= 0)
			GetGrid()->ChangePropertyValue(this, special);
	}

	return wxIntProperty::OnEvent(propgrid, window, e);
}


// -----------------------------------------------------------------------------
// MOPGThingTypeProperty Class Functions
//
// Behaves similarly to MOPGActionSpecialProperty, except for thing types
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// Returns a little object describing the args for this thing type
// -----------------------------------------------------------------------------
const Game::ArgSpec& MOPGThingTypeProperty::getArgspec()
{
	return Game::configuration().thingType(m_value.GetInteger()).argSpec();
}

// -----------------------------------------------------------------------------
// Returns the thing type value as a string (contains the thing type name)
// -----------------------------------------------------------------------------
wxString MOPGThingTypeProperty::ValueToString(wxVariant& value, int argFlags) const
{
	// Get value as integer
	int type = value.GetInteger();

	if (type == 0)
		return "0: None";

	auto& tt = Game::configuration().thingType(type);
	return S_FMT("%d: %s", type, tt.name());
}

// -----------------------------------------------------------------------------
// Called when an event is raised for the control
// -----------------------------------------------------------------------------
bool MOPGThingTypeProperty::OnEvent(wxPropertyGrid* propgrid, wxWindow* window, wxEvent& e)
{
	if (e.GetEventType() == wxEVT_BUTTON)
	{
		// Get type to select initially (if any)
		int init_type = -1;
		if (!IsValueUnspecified())
			init_type = GetValue().GetInteger();

		// Open thing browser
		ThingTypeBrowser browser(MapEditor::windowWx(), init_type);
		if (browser.ShowModal() == wxID_OK)
		{
			// Set the value if a type was selected
			int type = browser.getSelectedType();
			if (type >= 0)
				GetGrid()->ChangePropertyValue(this, type);
		}
	}

	return wxIntProperty::OnEvent(propgrid, window, e);
}


// -----------------------------------------------------------------------------
//
// MOPGLineFlagProperty Class Functions
//
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// MOPGLineFlagProperty class constructor
// -----------------------------------------------------------------------------
MOPGLineFlagProperty::MOPGLineFlagProperty(const wxString& label, const wxString& name, int index) :
	MOPGBoolProperty(label, name)
{
	// Init variables
	this->index_ = index;
}

// -----------------------------------------------------------------------------
// Reads the value of this line flag property from [objects]
// (if the value differs between objects, it is set to unspecified)
// -----------------------------------------------------------------------------
void MOPGLineFlagProperty::openObjects(vector<MapObject*>& objects)
{
	// Set unspecified if no objects given
	if (objects.size() == 0)
	{
		SetValueToUnspecified();
		return;
	}

	// Check flag against first object
	bool first = Game::configuration().lineFlagSet(index_, (MapLine*)objects[0]);

	// Check whether all objects share the same flag setting
	for (unsigned a = 1; a < objects.size(); a++)
	{
		if (Game::configuration().lineFlagSet(index_, (MapLine*)objects[a]) != first)
		{
			// Different value found, set unspecified
			SetValueToUnspecified();
			return;
		}
	}

	// Set to common value
	noupdate_ = true;
	SetValue(first);
	updateVisibility();
	noupdate_ = false;
}

// -----------------------------------------------------------------------------
// Applies the current property value to all objects currently open in the
// parent MapObjectPropsPanel, if a value is specified
// -----------------------------------------------------------------------------
void MOPGLineFlagProperty::applyValue()
{
	// Do nothing if no parent (and thus no object list)
	if (!parent_ || noupdate_)
		return;

	// Do nothing if the value is unspecified
	if (IsValueUnspecified())
		return;

	// Go through objects and set this value
	vector<MapObject*>& objects = parent_->getObjects();
	for (unsigned a = 0; a < objects.size(); a++)
		Game::configuration().setLineFlag(index_, (MapLine*)objects[a], GetValue());
}


// -----------------------------------------------------------------------------
//
// MOPGThingFlagProperty Class Functions
//
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// MOPGThingFlagProperty class constructor
// -----------------------------------------------------------------------------
MOPGThingFlagProperty::MOPGThingFlagProperty(const wxString& label, const wxString& name, int index) :
	MOPGBoolProperty(label, name)
{
	// Init variables
	this->index_ = index;
}

// -----------------------------------------------------------------------------
// Reads the value of this thing flag property from [objects]
// (if the value differs between objects, it is set to unspecified)
// -----------------------------------------------------------------------------
void MOPGThingFlagProperty::openObjects(vector<MapObject*>& objects)
{
	// Set unspecified if no objects given
	if (objects.size() == 0)
	{
		SetValueToUnspecified();
		return;
	}

	// Check flag against first object
	bool first = Game::configuration().thingFlagSet(index_, (MapThing*)objects[0]);

	// Check whether all objects share the same flag setting
	for (unsigned a = 1; a < objects.size(); a++)
	{
		if (Game::configuration().thingFlagSet(index_, (MapThing*)objects[a]) != first)
		{
			// Different value found, set unspecified
			SetValueToUnspecified();
			return;
		}
	}

	// Set to common value
	noupdate_ = true;
	SetValue(first);
	updateVisibility();
	noupdate_ = false;
}

// -----------------------------------------------------------------------------
// Applies the current property value to all objects currently open in the
// parent MapObjectPropsPanel, if a value is specified
// -----------------------------------------------------------------------------
void MOPGThingFlagProperty::applyValue()
{
	// Do nothing if no parent (and thus no object list)
	if (!parent_ || noupdate_)
		return;

	// Do nothing if the value is unspecified
	if (IsValueUnspecified())
		return;

	// Go through objects and set this value
	vector<MapObject*>& objects = parent_->getObjects();
	for (unsigned a = 0; a < objects.size(); a++)
		Game::configuration().setThingFlag(index_, (MapThing*)objects[a], GetValue());
}


// -----------------------------------------------------------------------------
//
// MOPGAngleProperty Class Functions
//
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// MOPGAngleProperty class constructor
// -----------------------------------------------------------------------------
MOPGAngleProperty::MOPGAngleProperty(const wxString& label, const wxString& name) : wxEditEnumProperty(label, name)
{
	propname_ = name;
	// Set to combo box editor
	// SetEditor(wxPGEditor_ComboBox);

	// Setup combo box choices
	wxArrayString labels;
	wxArrayInt    values;
	labels.Add("0: East");
	values.Add(0);
	labels.Add("45: Northeast");
	values.Add(45);
	labels.Add("90: North");
	values.Add(90);
	labels.Add("135: Northwest");
	values.Add(135);
	labels.Add("180: West");
	values.Add(180);
	labels.Add("225: Southwest");
	values.Add(225);
	labels.Add("270: South");
	values.Add(270);
	labels.Add("315: Southeast");
	values.Add(315);

	SetChoices(wxPGChoices(labels, values));
}

// -----------------------------------------------------------------------------
// Reads the value of this angle property from [objects]
// (if the value differs between objects, it is set to unspecified)
// -----------------------------------------------------------------------------
void MOPGAngleProperty::openObjects(vector<MapObject*>& objects)
{
	// Set unspecified if no objects given
	if (objects.size() == 0)
	{
		SetValueToUnspecified();
		return;
	}

	// Get property of first object
	int first = objects[0]->intProperty(GetName());

	// Check whether all objects share the same value
	for (unsigned a = 1; a < objects.size(); a++)
	{
		if (objects[a]->intProperty(GetName()) != first)
		{
			// Different value found, set unspecified
			SetValueToUnspecified();
			return;
		}
	}

	// Set to common value
	noupdate_ = true;
	SetValue(first);
	updateVisibility();
	noupdate_ = false;
}

// -----------------------------------------------------------------------------
// Default to hiding this property if set to its default value.
// -----------------------------------------------------------------------------
void MOPGAngleProperty::updateVisibility()
{
	if (!parent_->showAll() && !IsValueUnspecified() && udmf_prop_ && !udmf_prop_->showAlways()
		&& udmf_prop_->defaultValue().getIntValue() == GetValue().GetInteger())
		Hide(true);
	else
		Hide(false);
}

// -----------------------------------------------------------------------------
// Applies the current property value to all objects currently open in the
// parent MapObjectPropsPanel, if a value is specified
// -----------------------------------------------------------------------------
void MOPGAngleProperty::applyValue()
{
	// Do nothing if no parent (and thus no object list)
	if (!parent_ || noupdate_)
		return;

	// Do nothing if the value is unspecified
	if (IsValueUnspecified())
		return;

	// Go through objects and set this value
	vector<MapObject*>& objects = parent_->getObjects();
	for (unsigned a = 0; a < objects.size(); a++)
		objects[a]->setIntProperty(GetName(), m_value.GetInteger());
}

// -----------------------------------------------------------------------------
// Returns the angle value as a string
// -----------------------------------------------------------------------------
wxString MOPGAngleProperty::ValueToString(wxVariant& value, int argFlags) const
{
	int angle = value.GetInteger();

	switch (angle)
	{
	case 0: return "0: East"; break;
	case 45: return "45: Northeast"; break;
	case 90: return "90: North"; break;
	case 135: return "135: Northwest"; break;
	case 180: return "180: West"; break;
	case 225: return "225: Southwest"; break;
	case 270: return "270: South"; break;
	case 315: return "315: Southeast"; break;
	default: return S_FMT("%d", angle); break;
	}
}


// -----------------------------------------------------------------------------
//
// MOPGColourProperty Class Functions
//
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// MOPGColourProperty class constructor
// -----------------------------------------------------------------------------
MOPGColourProperty::MOPGColourProperty(const wxString& label, const wxString& name) : wxColourProperty(label, name)
{
	propname_ = name;
}

// -----------------------------------------------------------------------------
// Reads the value of this colour property from [objects]
// (if the value differs between objects, it is set to unspecified)
// -----------------------------------------------------------------------------
void MOPGColourProperty::openObjects(vector<MapObject*>& objects)
{
	// Set unspecified if no objects given
	if (objects.size() == 0)
	{
		SetValueToUnspecified();
		return;
	}

	// Get property of first object
	int first = objects[0]->intProperty(GetName());

	// Check whether all objects share the same value
	for (unsigned a = 1; a < objects.size(); a++)
	{
		if (objects[a]->intProperty(GetName()) != first)
		{
			// Different value found, set unspecified
			SetValueToUnspecified();
			return;
		}
	}

	// Set to common value
	noupdate_ = true;
	wxColor col(first);
	col.Set(col.Blue(), col.Green(), col.Red());
	wxVariant var_value;
	var_value << col;
	SetValue(var_value);
	updateVisibility();
	noupdate_ = false;
}

// -----------------------------------------------------------------------------
// Colours have no default and are always visible.
// -----------------------------------------------------------------------------
void MOPGColourProperty::updateVisibility()
{
	Hide(false);
}

// -----------------------------------------------------------------------------
// Applies the current property value to all objects currently open in the
// parent MapObjectPropsPanel, if a value is specified
// -----------------------------------------------------------------------------
void MOPGColourProperty::applyValue()
{
	// Do nothing if no parent (and thus no object list)
	if (!parent_ || noupdate_)
		return;

	// Do nothing if the value is unspecified
	if (IsValueUnspecified())
		return;

	// Go through objects and set this value
	vector<MapObject*>& objects = parent_->getObjects();
	wxColor             col;
	col << m_value;
	col.Set(col.Blue(), col.Green(), col.Red());
	for (unsigned a = 0; a < objects.size(); a++)
		objects[a]->setIntProperty(GetName(), col.GetRGB());
}


// -----------------------------------------------------------------------------
//
// MOPGTextureProperty Class Functions
//
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// MOPGTextureProperty class constructor
// -----------------------------------------------------------------------------
MOPGTextureProperty::MOPGTextureProperty(int textype, const wxString& label, const wxString& name) :
	MOPGStringProperty(label, name)
{
	// Init variables
	this->textype_ = textype;

	// Set to text+button editor
	SetEditor(wxPGEditor_TextCtrlAndButton);
}

// -----------------------------------------------------------------------------
// Reads the value of this texture property from [objects]
// (if the value differs between objects, it is set to unspecified)
// -----------------------------------------------------------------------------
void MOPGTextureProperty::openObjects(vector<MapObject*>& objects)
{
	// Set unspecified if no objects given
	if (objects.size() == 0)
	{
		SetValueToUnspecified();
		return;
	}

	// Get property of first object
	string first = objects[0]->stringProperty(GetName());

	// Check whether all objects share the same value
	for (unsigned a = 1; a < objects.size(); a++)
	{
		if (objects[a]->stringProperty(GetName()) != first)
		{
			// Different value found, set unspecified
			SetValueToUnspecified();
			return;
		}
	}

	// Set to common value
	noupdate_ = true;
	SetValue(first);
	updateVisibility();
	noupdate_ = false;
}

// -----------------------------------------------------------------------------
// Called when an event is raised for the control
// -----------------------------------------------------------------------------
bool MOPGTextureProperty::OnEvent(wxPropertyGrid* propgrid, wxWindow* window, wxEvent& e)
{
	// '...' button clicked
	if (e.GetEventType() == wxEVT_BUTTON)
	{
		// Get current texture (if any)
		string tex_current = "";
		if (!IsValueUnspecified())
			tex_current = GetValueAsString();

		// Open map texture browser
		MapTextureBrowser browser(MapEditor::windowWx(), textype_, tex_current, &(MapEditor::editContext().map()));
		if (browser.ShowModal() == wxID_OK && browser.getSelectedItem())
			GetGrid()->ChangePropertyValue(this, browser.getSelectedItem()->name());

		// Refresh text
		RefreshEditor();
	}

	return wxStringProperty::OnEvent(propgrid, window, e);
}


// -----------------------------------------------------------------------------
//
// MOPGSPACTriggerProperty Class Functions
//
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// MOPGSPACTriggerProperty class constructor
// -----------------------------------------------------------------------------
MOPGSPACTriggerProperty::MOPGSPACTriggerProperty(const wxString& label, const wxString& name) :
	wxEnumProperty(label, name)
{
	propname_ = name;

	// Set to combo box editor
	SetEditor(wxPGEditor_ComboBox);

	// Setup combo box choices
	wxArrayString labels = Game::configuration().allSpacTriggers();
	SetChoices(wxPGChoices(labels));
}

// -----------------------------------------------------------------------------
// Reads the value of this SPAC trigger property from [objects]
// (if the value differs between objects, it is set to unspecified)
// -----------------------------------------------------------------------------
void MOPGSPACTriggerProperty::openObjects(vector<MapObject*>& objects)
{
	// Set unspecified if no objects given
	if (objects.size() == 0)
	{
		SetValueToUnspecified();
		return;
	}

	// Get property of first object
	int    map_format = MapEditor::editContext().mapDesc().format;
	string first      = Game::configuration().spacTriggerString((MapLine*)objects[0], map_format);

	// Check whether all objects share the same value
	for (unsigned a = 1; a < objects.size(); a++)
	{
		if (Game::configuration().spacTriggerString((MapLine*)objects[a], map_format) != first)
		{
			// Different value found, set unspecified
			SetValueToUnspecified();
			return;
		}
	}

	// Set to common value
	noupdate_ = true;
	SetValue(first);
	updateVisibility();
	noupdate_ = false;
}

// -----------------------------------------------------------------------------
// Default to hiding this property if set to its default value.
// -----------------------------------------------------------------------------
void MOPGSPACTriggerProperty::updateVisibility()
{
	if (!parent_->showAll() && !IsValueUnspecified() && udmf_prop_ && !udmf_prop_->showAlways()
		&& udmf_prop_->defaultValue().getIntValue() == GetValue().GetInteger())
		Hide(true);
	else
		Hide(false);
}

// -----------------------------------------------------------------------------
// Applies the current property value to all objects currently open in the
// parent MapObjectPropsPanel, if a value is specified
// -----------------------------------------------------------------------------
void MOPGSPACTriggerProperty::applyValue()
{
	// Do nothing if no parent (and thus no object list)
	if (!parent_ || noupdate_)
		return;

	// Do nothing if the value is unspecified
	if (IsValueUnspecified())
		return;

	// Go through objects and set this value
	vector<MapObject*>& objects = parent_->getObjects();
	for (unsigned a = 0; a < objects.size(); a++)
		Game::configuration().setLineSpacTrigger(GetChoiceSelection(), (MapLine*)objects[a]);
	// objects[a]->setIntProperty(GetName(), m_value.GetInteger());
}


// -----------------------------------------------------------------------------
// MOPGTagProperty Class Functions
//
// Property grid cell to handle tag properties, the '...' button will set it to
// the next free tag or id depending on the object type
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// MOPGTagProperty class constructor
// -----------------------------------------------------------------------------
MOPGTagProperty::MOPGTagProperty(IdType id_type, const wxString& label, const wxString& name) :
	MOPGIntProperty(label, name)
{
	this->id_type_ = id_type;

	// Set to text+button editor
	SetEditor(wxPGEditor_TextCtrlAndButton);
}

// -----------------------------------------------------------------------------
// Reads the value of this tag/id property from [objects]
// (if the value differs between objects, it is set to unspecified)
// -----------------------------------------------------------------------------
void MOPGTagProperty::openObjects(vector<MapObject*>& objects)
{
	// Set unspecified if no objects given
	if (objects.size() == 0)
	{
		SetValueToUnspecified();
		return;
	}

	// Get property of first object
	int first = objects[0]->intProperty(GetName());

	// Check whether all objects share the same value
	for (unsigned a = 1; a < objects.size(); a++)
	{
		if (objects[a]->intProperty(GetName()) != first)
		{
			// Different value found, set unspecified
			SetValueToUnspecified();
			return;
		}
	}

	// Set to common value
	noupdate_ = true;
	SetValue(first);
	updateVisibility();
	noupdate_ = false;
}

// -----------------------------------------------------------------------------
// Called when an event is raised for the control
// -----------------------------------------------------------------------------
bool MOPGTagProperty::OnEvent(wxPropertyGrid* propgrid, wxWindow* window, wxEvent& e)
{
	if (e.GetEventType() == wxEVT_BUTTON)
	{
		vector<MapObject*>& objects = parent_->getObjects();
		if (objects.size() == 0)
			return false;
		if (!objects[0]->getParentMap())
			return false;

		// Get unused tag/id depending on object type
		int tag = GetValue().GetInteger();
		if (id_type_ == IdType::Sector)
			tag = objects[0]->getParentMap()->findUnusedSectorTag();
		else if (id_type_ == IdType::Line)
			tag = objects[0]->getParentMap()->findUnusedLineId();
		else if (id_type_ == IdType::Thing)
			tag = objects[0]->getParentMap()->findUnusedThingId();

		GetGrid()->ChangePropertyValue(this, tag);
		return true;
	}

	return wxIntProperty::OnEvent(propgrid, window, e);
}


// -----------------------------------------------------------------------------
//
// MOPGSectorSpecialProperty Class Functions
//
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// MOPGSectorSpecialProperty class constructor
// -----------------------------------------------------------------------------
MOPGSectorSpecialProperty::MOPGSectorSpecialProperty(const wxString& label, const wxString& name) :
	MOPGIntProperty(label, name)
{
	// Set to text+button editor
	SetEditor(wxPGEditor_TextCtrlAndButton);
}

// -----------------------------------------------------------------------------
// Reads the value of this sector special property from [objects]
// (if the value differs between objects, it is set to unspecified)
// -----------------------------------------------------------------------------
void MOPGSectorSpecialProperty::openObjects(vector<MapObject*>& objects)
{
	// Set unspecified if no objects given
	if (objects.size() == 0)
	{
		SetValueToUnspecified();
		return;
	}

	// Get property of first object
	int first = objects[0]->intProperty(GetName());

	// Check whether all objects share the same value
	for (unsigned a = 1; a < objects.size(); a++)
	{
		if (objects[a]->intProperty(GetName()) != first)
		{
			// Different value found, set unspecified
			SetValueToUnspecified();
			return;
		}
	}

	// Set to common value
	noupdate_ = true;
	SetValue(first);
	updateVisibility();
	noupdate_ = false;
}

// -----------------------------------------------------------------------------
// Returns the sector special value as a string
// -----------------------------------------------------------------------------
wxString MOPGSectorSpecialProperty::ValueToString(wxVariant& value, int argFlags) const
{
	int type = value.GetInteger();

	return S_FMT("%d: %s", type, Game::configuration().sectorTypeName(type));
}

// -----------------------------------------------------------------------------
// Called when an event is raised for the control
// -----------------------------------------------------------------------------
bool MOPGSectorSpecialProperty::OnEvent(wxPropertyGrid* propgrid, wxWindow* window, wxEvent& e)
{
	// '...' button clicked
	if (e.GetEventType() == wxEVT_BUTTON)
	{
		SectorSpecialDialog dlg(MapEditor::windowWx());
		dlg.setup(m_value.GetInteger());
		if (dlg.ShowModal() == wxID_OK)
			GetGrid()->ChangePropertyValue(this, dlg.getSelectedSpecial());

		return true;
	}

	return wxIntProperty::OnEvent(propgrid, window, e);
}
