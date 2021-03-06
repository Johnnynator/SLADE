#pragma once

#include "MapEditor/Edit/Edit3D.h"

class SLADEMap;
class GLTexture;
class MapObject;

class InfoOverlay3D
{
public:
	InfoOverlay3D();
	~InfoOverlay3D();

	void update(int item_index, MapEditor::ItemType item_type, SLADEMap* map);
	void draw(int bottom, int right, int middle, float alpha = 1.0f);
	void drawTexture(float alpha, int x, int y);
	void reset()
	{
		texture_ = nullptr;
		object_  = nullptr;
	}

private:
	vector<string>      info_;
	vector<string>      info2_;
	MapEditor::ItemType current_type_;
	string              texname_;
	GLTexture*          texture_;
	bool                thing_icon_;
	MapObject*          object_;
	long                last_update_;
};
