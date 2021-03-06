
// -----------------------------------------------------------------------------
// SLADE - It's a Doom Editor
// Copyright(C) 2008 - 2017 Simon Judd
//
// Email:       sirjuddington@gmail.com
// Web:         http://slade.mancubus.net
// Filename:    LineTextureOverlay.cpp
// Description: LineTextureOverlay class - A full screen map editor overlay that
//              shows (a) lines textures and allows the user to click a texture
//              to browse for it
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
#include "LineTextureOverlay.h"
#include "Game/Configuration.h"
#include "General/ColourConfiguration.h"
#include "MapEditor/MapEditContext.h"
#include "MapEditor/MapTextureManager.h"
#include "MapEditor/SLADEMap/MapLine.h"
#include "MapEditor/SLADEMap/MapSide.h"
#include "MapEditor/UI/Dialogs/MapTextureBrowser.h"
#include "OpenGL/Drawing.h"


// -----------------------------------------------------------------------------
//
// LineTextureOverlay Class Functions
//
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// LineTextureOverlay class constructor
// -----------------------------------------------------------------------------
LineTextureOverlay::LineTextureOverlay()
{
	// Init variables
	tex_size_     = 0;
	last_width_   = 0;
	last_height_  = 0;
	selected_side = 0;
}

// -----------------------------------------------------------------------------
// LineTextureOverlay class destructor
// -----------------------------------------------------------------------------
LineTextureOverlay::~LineTextureOverlay() {}

// -----------------------------------------------------------------------------
// Adds [texture] to the overlay if it doesn't already exist at the target part
// (front upper, etc)
// -----------------------------------------------------------------------------
void LineTextureOverlay::addTexture(TexInfo& inf, string texture)
{
	// Ignore if texture is blank ("-")
	if (texture == "-")
		return;

	// Add texture if it doesn't exist already
	bool exists = false;
	for (unsigned a = 0; a < inf.textures.size(); a++)
	{
		if (inf.textures[a] == texture)
		{
			exists = true;
			break;
		}
	}
	if (!exists)
		inf.textures.push_back(texture);
}

// -----------------------------------------------------------------------------
// 'Opens' all lines in [list], adds all textures from each
// -----------------------------------------------------------------------------
void LineTextureOverlay::openLines(vector<MapLine*>& list)
{
	// Clear current lines
	lines.clear();
	this->side1_  = false;
	this->side2_  = false;
	selected_side = 0;
	for (unsigned a = 0; a < 6; a++)
	{
		textures_[a].textures.clear();
		textures_[a].hover   = false;
		textures_[a].changed = false;
	}

	// Go through list
	for (unsigned a = 0; a < list.size(); a++)
	{
		// Add to lines list
		lines.push_back(list[a]);

		// Process first side
		MapSide* side1 = list[a]->s1();
		if (side1)
		{
			// Add textures
			addTexture(textures_[FrontUpper], side1->getTexUpper());
			addTexture(textures_[FrontMiddle], side1->getTexMiddle());
			addTexture(textures_[FrontLower], side1->getTexLower());

			this->side1_ = true;
		}

		// Process second side
		MapSide* side2 = list[a]->s2();
		if (side2)
		{
			// Add textures
			addTexture(textures_[BackUpper], side2->getTexUpper());
			addTexture(textures_[BackMiddle], side2->getTexMiddle());
			addTexture(textures_[BackLower], side2->getTexLower());

			this->side2_ = true;
		}
	}

	if (!side1_)
		selected_side = 1;
}

// -----------------------------------------------------------------------------
// Called when the user closes the overlay. Applies changes if [cancel] is false
// -----------------------------------------------------------------------------
void LineTextureOverlay::close(bool cancel)
{
	// Apply texture changes if not cancelled
	if (!cancel)
	{
		MapEditor::editContext().beginUndoRecord("Change Line Texture", true, false, false);

		// Go through lines
		for (unsigned a = 0; a < lines.size(); a++)
		{
			// Front Upper
			if (textures_[FrontUpper].changed && !textures_[FrontUpper].textures.empty())
				lines[a]->setStringProperty("side1.texturetop", textures_[FrontUpper].textures[0]);

			// Front Middle
			if (textures_[FrontMiddle].changed && !textures_[FrontMiddle].textures.empty())
				lines[a]->setStringProperty("side1.texturemiddle", textures_[FrontMiddle].textures[0]);

			// Front Lower
			if (textures_[FrontLower].changed && !textures_[FrontLower].textures.empty())
				lines[a]->setStringProperty("side1.texturebottom", textures_[FrontLower].textures[0]);


			// Back Upper
			if (textures_[BackUpper].changed && !textures_[BackUpper].textures.empty())
				lines[a]->setStringProperty("side2.texturetop", textures_[BackUpper].textures[0]);

			// Back Middle
			if (textures_[BackMiddle].changed && !textures_[BackMiddle].textures.empty())
				lines[a]->setStringProperty("side2.texturemiddle", textures_[BackMiddle].textures[0]);

			// Back Lower
			if (textures_[BackLower].changed && !textures_[BackLower].textures.empty())
				lines[a]->setStringProperty("side2.texturebottom", textures_[BackLower].textures[0]);
		}

		MapEditor::editContext().endUndoRecord();
	}

	// Deactivate
	active_ = false;
}

// -----------------------------------------------------------------------------
// Updates the overlay (animations, etc.)
// -----------------------------------------------------------------------------
void LineTextureOverlay::update(long frametime) {}

// -----------------------------------------------------------------------------
// Updates the layout of the overlay to fit properly within [width],[height]
// -----------------------------------------------------------------------------
void LineTextureOverlay::updateLayout(int width, int height)
{
	// Determine layout stuff
	int rows = 1;
	if (side1_ && side2_)
		rows = 2;
	int middlex = width * 0.5;
	int middley = height * 0.5;
	int maxsize = min(width / 3, height / rows);
	tex_size_   = maxsize - 64;
	if (tex_size_ > 256)
		tex_size_ = 256;
	int border = (maxsize - tex_size_) * 0.5;
	if (border > 48)
		border = 48;

	// Set texture positions
	int ymid = middley;
	if (rows == 2)
		ymid = middley - (border * 0.5) - (tex_size_ * 0.5);
	if (side1_)
	{
		// Front side textures
		int xmid                         = middlex - border - tex_size_;
		textures_[FrontUpper].position.x = xmid;
		textures_[FrontUpper].position.y = ymid;

		xmid += border + tex_size_;
		textures_[FrontMiddle].position.x = xmid;
		textures_[FrontMiddle].position.y = ymid;

		xmid += border + tex_size_;
		textures_[FrontLower].position.x = xmid;
		textures_[FrontLower].position.y = ymid;

		ymid += border + tex_size_;
	}
	if (side2_)
	{
		// Back side textures
		int xmid                        = middlex - border - tex_size_;
		textures_[BackUpper].position.x = xmid;
		textures_[BackUpper].position.y = ymid;

		xmid += border + tex_size_;
		textures_[BackMiddle].position.x = xmid;
		textures_[BackMiddle].position.y = ymid;

		xmid += border + tex_size_;
		textures_[BackLower].position.x = xmid;
		textures_[BackLower].position.y = ymid;
	}

	last_width_  = width;
	last_height_ = height;
}

// -----------------------------------------------------------------------------
// Draws the overlay to [width],[height]
// -----------------------------------------------------------------------------
void LineTextureOverlay::draw(int width, int height, float fade)
{
	// Update layout if needed
	if (width != last_width_ || height != last_height_)
		updateLayout(width, height);

	// Get colours
	rgba_t col_bg = ColourConfiguration::getColour("map_overlay_background");
	rgba_t col_fg = ColourConfiguration::getColour("map_overlay_foreground");
	col_bg.a *= fade;
	col_fg.a *= fade;

	// Draw background
	glDisable(GL_TEXTURE_2D);
	OpenGL::setColour(col_bg);
	Drawing::drawFilledRect(0, 0, width, height);

	// Draw textures
	glEnable(GL_LINE_SMOOTH);
	int cur_size = tex_size_ * fade;
	if (!active_)
		cur_size = tex_size_;
	if (side1_)
	{
		drawTexture(fade, cur_size, textures_[FrontLower], "Front Lower");
		drawTexture(fade, cur_size, textures_[FrontMiddle], "Front Middle");
		drawTexture(fade, cur_size, textures_[FrontUpper], "Front Upper");
	}
	if (side2_)
	{
		drawTexture(fade, cur_size, textures_[BackLower], "Back Lower");
		drawTexture(fade, cur_size, textures_[BackMiddle], "Back Middle");
		drawTexture(fade, cur_size, textures_[BackUpper], "Back Upper");
	}
}

// -----------------------------------------------------------------------------
// Draws the texture box from info in [tex]
// -----------------------------------------------------------------------------
void LineTextureOverlay::drawTexture(float alpha, int size, TexInfo& tex, string position)
{
	// Get colours
	rgba_t col_bg  = ColourConfiguration::getColour("map_overlay_background");
	rgba_t col_fg  = ColourConfiguration::getColour("map_overlay_foreground");
	rgba_t col_sel = ColourConfiguration::getColour("map_hilight");
	col_fg.a       = col_fg.a * alpha;

	// Draw background
	int halfsize = size * 0.5;
	glEnable(GL_TEXTURE_2D);
	OpenGL::setColour(255, 255, 255, 255 * alpha, 0);
	glPushMatrix();
	glTranslated(tex.position.x - halfsize, tex.position.y - halfsize, 0);
	GLTexture::bgTex().draw2dTiled(size, size);
	glPopMatrix();

	GLTexture* tex_first = nullptr;
	if (tex.textures.size() > 0)
	{
		// Draw first texture
		OpenGL::setColour(255, 255, 255, 255 * alpha, 0);
		tex_first = MapEditor::textureManager().getTexture(
			tex.textures[0], Game::configuration().featureSupported(Game::Feature::MixTexFlats));
		Drawing::drawTextureWithin(
			tex_first,
			tex.position.x - halfsize,
			tex.position.y - halfsize,
			tex.position.x + halfsize,
			tex.position.y + halfsize,
			0,
			2);

		// Draw up to 4 subsequent textures (overlaid)
		OpenGL::setColour(255, 255, 255, 127 * alpha, 0);
		for (unsigned a = 1; a < tex.textures.size() && a < 5; a++)
		{
			auto gl_tex = MapEditor::textureManager().getTexture(
				tex.textures[a], Game::configuration().featureSupported(Game::Feature::MixTexFlats));

			Drawing::drawTextureWithin(
				gl_tex,
				tex.position.x - halfsize,
				tex.position.y - halfsize,
				tex.position.x + halfsize,
				tex.position.y + halfsize,
				0,
				2);
		}
	}

	glDisable(GL_TEXTURE_2D);

	// Draw outline
	if (tex.hover)
	{
		OpenGL::setColour(col_sel.r, col_sel.g, col_sel.b, 255 * alpha, 0);
		glLineWidth(3.0f);
	}
	else
	{
		OpenGL::setColour(col_fg.r, col_fg.g, col_fg.b, 255 * alpha, 0);
		glLineWidth(1.5f);
	}
	Drawing::drawRect(
		tex.position.x - halfsize, tex.position.y - halfsize, tex.position.x + halfsize, tex.position.y + halfsize);

	// Draw position text
	Drawing::drawText(
		position + ":",
		tex.position.x,
		tex.position.y - halfsize - 18,
		col_fg,
		Drawing::Font::Bold,
		Drawing::Align::Center);

	// Determine texture name text
	string str_texture;
	if (tex.textures.size() == 1)
		str_texture = S_FMT("%s (%dx%d)", tex.textures[0], tex_first->getWidth(), tex_first->getHeight());
	else if (tex.textures.size() > 1)
		str_texture = S_FMT("Multiple (%lu)", tex.textures.size());
	else
		str_texture = "- (None)";

	// Draw texture name
	Drawing::drawText(
		str_texture,
		tex.position.x,
		tex.position.y + halfsize + 2,
		col_fg,
		Drawing::Font::Bold,
		Drawing::Align::Center);
}

// -----------------------------------------------------------------------------
// Called when the mouse cursor is moved
// -----------------------------------------------------------------------------
void LineTextureOverlay::mouseMotion(int x, int y)
{
	// Check textures for hover
	int halfsize = tex_size_ * 0.5;
	if (side1_)
	{
		textures_[FrontUpper].checkHover(x, y, halfsize);
		textures_[FrontMiddle].checkHover(x, y, halfsize);
		textures_[FrontLower].checkHover(x, y, halfsize);
	}
	if (side2_)
	{
		textures_[BackUpper].checkHover(x, y, halfsize);
		textures_[BackMiddle].checkHover(x, y, halfsize);
		textures_[BackLower].checkHover(x, y, halfsize);
	}
}

// -----------------------------------------------------------------------------
// Called when the left mouse button is clicked
// -----------------------------------------------------------------------------
void LineTextureOverlay::mouseLeftClick()
{
	// Check for any hover
	if (textures_[FrontUpper].hover)
		browseTexture(textures_[FrontUpper], "Front Upper");
	else if (textures_[FrontMiddle].hover)
		browseTexture(textures_[FrontMiddle], "Front Middle");
	else if (textures_[FrontLower].hover)
		browseTexture(textures_[FrontLower], "Front Lower");
	else if (textures_[BackUpper].hover)
		browseTexture(textures_[BackUpper], "Back Upper");
	else if (textures_[BackMiddle].hover)
		browseTexture(textures_[BackMiddle], "Back Middle");
	else if (textures_[BackLower].hover)
		browseTexture(textures_[BackLower], "Back Lower");
}

// -----------------------------------------------------------------------------
// Called when the right mouse button is clicked
// -----------------------------------------------------------------------------
void LineTextureOverlay::mouseRightClick() {}

// -----------------------------------------------------------------------------
// Called when a key is pressed
// -----------------------------------------------------------------------------
void LineTextureOverlay::keyDown(string key)
{
	// 'Select' front side
	if ((key == "F" || key == "f") && side1_)
		selected_side = 0;

	// 'Select' back side
	if ((key == "B" || key == "b") && side2_)
		selected_side = 1;

	// Browse upper texture
	if (key == "U" || key == "u")
	{
		if (selected_side == 0)
			browseTexture(textures_[FrontUpper], "Front Upper");
		else
			browseTexture(textures_[BackUpper], "Back Upper");
	}

	// Browse middle texture
	if (key == "M" || key == "m")
	{
		if (selected_side == 0)
			browseTexture(textures_[FrontMiddle], "Front Middle");
		else
			browseTexture(textures_[BackMiddle], "Back Middle");
	}

	// Browse lower texture
	if (key == "L" || key == "l")
	{
		if (selected_side == 0)
			browseTexture(textures_[FrontLower], "Front Lower");
		else
			browseTexture(textures_[BackLower], "Back Lower");
	}
}

// -----------------------------------------------------------------------------
// Opens the texture browser for [tex]
// -----------------------------------------------------------------------------
void LineTextureOverlay::browseTexture(TexInfo& tex, string position)
{
	// Get initial texture
	string texture;
	if (tex.textures.size() > 0)
		texture = tex.textures[0];
	else
		texture = "-";

	// Open texture browser
	MapTextureBrowser browser(MapEditor::windowWx(), 0, texture, &(MapEditor::editContext().map()));
	browser.SetTitle(S_FMT("Browse %s Texture", position));
	if (browser.ShowModal() == wxID_OK && browser.getSelectedItem())
	{
		// Set texture
		tex.textures.clear();
		tex.textures.push_back(browser.getSelectedItem()->name());
		tex.changed = true;
		close(false);
	}
}
