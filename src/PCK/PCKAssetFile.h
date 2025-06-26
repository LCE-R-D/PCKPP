#pragma once
#include <vector>
#include <string>
#include <fstream>

// PCK Asset File and Asset File Types research done by NessieHax/Miku666/nullptr, myself (May/MattNL), and many others over the years.

class PCKAssetFile
{
public:
	enum class Type
	{
		// *.png for Skins; used for Skin initialization
		SKIN,
		// *.png for Capes; used for Cape initialization
		CAPE,
		// *.png for generic textures and resources
		TEXTURE,
		/// <summary>
		/// Unused; likely a PCK file used for UI sprites at some point in development
		/// </summary>
		UI_DATA,
		/// <summary>
		/// "0" file for Pack Initialization
		/// </summary>
		INFO,
		/// <summary>
		/// .pck data file for Texture Pack Initialization
		/// </summary>
		TEXTURE_PACK_INFO,
		/// <summary>
		/// languages.loc/localisation.loc for string localization
		/// </summary>
		LOCALISATION,
		/// <summary>
		/// GameRules.grf compressed file for World Template initialization
		/// </summary>
		GAME_RULES,
		/// <summary>
		/// .pck data file for Custom Texture Pack Music Cues
		/// </summary>
		AUDIO,
		/// <summary>
		/// colours.col file for Texture Pack Custom Colo(u)r Palettes
		/// </summary>
		COLOUR_TABLE,
		/// <summary>
		/// GameRules.grh non compressed file for World Template initialization
		/// </summary>
		GAME_RULES_HEADER,
		/// <summary>
		/// .pck data file for Skin initalization and extended custom skin features
		/// </summary>
		SKIN_DATA,
		/// <summary>
		/// models.bin file for Custom Entity Models for Texture Packs
		/// </summary>
		MODELS,
		/// <summary>
		/// behaviours.bin file for... only changing the position of where a player is positioned when sitting lol
		/// </summary>
		BEHAVIOURS,
		/// <summary>
		/// entityMaterials.bin file for changing the rendering properties of certain Entity textures for Texture Packs
		/// </summary>
		MATERIALS,
		PCK_ASSET_TYPES_TOTAL
	};

	PCKAssetFile(const std::string& path, const std::vector<unsigned char>& data, Type assetType)
		: mAssetType(assetType), mData(data), mPath(path) {}

	~PCKAssetFile() {
		mData.clear();
		mPath.clear();
	}

	std::size_t getFileSize() const;

	const std::vector<unsigned char>& getData() const;

	void setData(const std::vector<unsigned char>& data);

	const std::string& getPath() const;

	void setPath(const std::string& inpath);

	Type getAssetType() const;

	// Checks if the file is a Skin, Cape or Texture Asset Type
	bool isImageType() const;

	void addProperty(const std::string& key, const std::string& value);

private:
	Type mAssetType{ Type::SKIN };
	std::vector<unsigned char> mData;
	std::string mPath;
	std::vector<std::pair<std::string, std::string>> mProperties;
};