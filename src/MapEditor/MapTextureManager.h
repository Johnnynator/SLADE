#pragma once

#include "General/ListenerAnnouncer.h"
#include "OpenGL/GLTexture.h"

class ArchiveTreeNode;
class Archive;
class Palette;

class MapTextureManager : public Listener
{
public:
	enum class Category
	{
		// Texture categories
		None = 0,
		TextureX,
		Tx,
		ZDTextures,
		HiRes
	};

	struct Texture
	{
		GLTexture* texture;
		Texture() { texture = nullptr; }
		~Texture()
		{
			if (texture && texture != &(GLTexture::missingTex()))
				delete texture;
		}
	};
	typedef std::map<string, Texture> MapTexHashMap;

	struct TexInfo
	{
		string   short_name;
		Category category;
		Archive* archive;
		string   path;
		unsigned index;
		string   long_name;

		TexInfo(
			string   short_name,
			Category category,
			Archive* archive,
			string   path,
			unsigned index    = 0,
			string   long_name = "") :
			short_name(short_name),
			category(category),
			archive(archive),
			path(path),
			index(index),
			long_name(long_name)
		{
		}
	};

	MapTextureManager(Archive* archive = nullptr);
	~MapTextureManager();

	void init();
	void setArchive(Archive* archive);
	void refreshResources();
	void buildTexInfoList();

	Palette*   getResourcePalette();
	GLTexture* getTexture(string name, bool mixed);
	GLTexture* getFlat(string name, bool mixed);
	GLTexture* getSprite(string name, string translation = "", string palette = "");
	GLTexture* getEditorImage(string name);
	int        getVerticalOffset(string name);

	vector<TexInfo>& getAllTexturesInfo() { return tex_info_; }
	vector<TexInfo>& getAllFlatsInfo() { return flat_info_; }

	void onAnnouncement(Announcer* announcer, string event_name, MemChunk& event_data);

private:
	Archive*        archive_;
	MapTexHashMap   textures_;
	MapTexHashMap   flats_;
	MapTexHashMap   sprites_;
	MapTexHashMap   editor_images_;
	bool            editor_images_loaded_;
	Palette*        palette_;
	vector<TexInfo> tex_info_;
	vector<TexInfo> flat_info_;

	void importEditorImages(MapTexHashMap& map, ArchiveTreeNode* dir, string path);
};
