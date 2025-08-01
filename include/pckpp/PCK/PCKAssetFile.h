#pragma once

#include <algorithm>
#include <fstream>
#include <vector>
#include <filesystem>

// PCK Asset File and Asset File Types research done by NessieHax/Miku666/nullptr, myself (May/MattNL), and many others over the years.

// Files used inside of PCK Files
class PCKAssetFile
{
public:
	using Property = std::pair<std::string, std::u16string>;

	enum class Type
	{
		// *.png for Skins; used for Skin initialization
		SKIN,
		// *.png for Capes; used for Cape initialization
		CAPE,
		// *.png for generic textures and resources
		TEXTURE,
		// Unused; likely a PCK file used for UI sprites at some point in development
		UI_DATA,
		// "0" file for Pack Initialization
		INFO,
		// .pck data file for Texture Pack Initialization
		TEXTURE_PACK_INFO,
		// languages.loc/localisation.loc for string localization
		LOCALISATION,
		// GameRules.grf compressed file for World Template initialization
		GAME_RULES,
		// .pck data file for Custom Texture Pack Music Cues
		AUDIO_DATA,
		// colours.col file for Texture Pack Custom Colo(u)r Palettes
		COLOUR_TABLE,
		// GameRules.grh non compressed file for World Template initialization
		GAME_RULES_HEADER,
		// .pck data file for Skin initalization and extended custom skin features
		SKIN_DATA,
		// models.bin file for Custom Entity Models for Texture Packs
		MODELS,
		// behaviours.bin file for... only changing the position of where a player is positioned when sitting lol
		BEHAVIOURS,
		// entityMaterials.bin file for changing the rendering properties of certain Entity textures for Texture Packs
		MATERIALS,
		// Just a total value for loops
		PCK_ASSET_TYPES_TOTAL
	};

	// Gets a asset type as a string
	constexpr const char* getAssetTypeString() const
	{
		return getAssetTypeString(mAssetType);
	}

	// Gets a asset type as a string
	static constexpr const char* getAssetTypeString(Type type)
	{
		switch (type)
		{
		case Type::SKIN: return "SKIN";
		case Type::CAPE: return "CAPE";
		case Type::TEXTURE: return "TEXTURE";
		case Type::UI_DATA: return "UI_DATA";
		case Type::INFO: return "INFO";
		case Type::TEXTURE_PACK_INFO: return "TEXTURE_PACK_INFO";
		case Type::LOCALISATION: return "LOCALISATION";
		case Type::GAME_RULES: return "GAME_RULES";
		case Type::AUDIO_DATA: return "AUDIO_DATA";
		case Type::COLOUR_TABLE: return "COLOUR_TABLE";
		case Type::GAME_RULES_HEADER: return "GAME_RULES_HEADER";
		case Type::SKIN_DATA: return "SKIN_DATA";
		case Type::MODELS: return "MODELS";
		case Type::BEHAVIOURS: return "BEHAVIOURS";
		case Type::MATERIALS: return "MATERIALS";
		default: return "UNKNOWN";
		}
	}

	// Gets a asset type as a string for dialogs
	constexpr const char* getAssetTypeStringDisplay() const
	{
		return getAssetTypeStringDisplay(mAssetType);
	}

	// Gets a asset type as a string for dialogs
	static constexpr const char* getAssetTypeStringDisplay(Type type)
	{
		switch (type)
		{
		case Type::SKIN: return "Skin";
		case Type::CAPE: return "Cape";
		case Type::TEXTURE: return "Texture";
		case Type::UI_DATA: return "UI Data";
		case Type::INFO: return "Info";
		case Type::TEXTURE_PACK_INFO: return "Texture Pack Info";
		case Type::LOCALISATION: return "Localisation";
		case Type::GAME_RULES: return "Game Rules";
		case Type::AUDIO_DATA: return "Audio Data";
		case Type::COLOUR_TABLE: return "Colour Table";
		case Type::GAME_RULES_HEADER: return "Game Rules Header";
		case Type::SKIN_DATA: return "Skin Data";
		case Type::MODELS: return "Models";
		case Type::BEHAVIOURS: return "Behaviours";
		case Type::MATERIALS: return "Materials";
		default: return "Unknown";
		}
	}

	const std::vector<const char*> getPreferredExtension() const
	{
		return getPreferredExtension(mAssetType);
	}

	static const std::vector<const char*> getPreferredExtension(Type type)
	{
		switch (type)
		{
		case Type::SKIN:
		case Type::CAPE:
		case Type::TEXTURE:
			return { "png", "tga" };
		case Type::TEXTURE_PACK_INFO:
		case Type::AUDIO_DATA:
		case Type::SKIN_DATA:
			return { "pck", nullptr };
		case Type::LOCALISATION:
			return { "loc", nullptr };
		case Type::GAME_RULES:
			return { "grf", nullptr };
		case Type::COLOUR_TABLE:
			return { "col", nullptr };
		case Type::GAME_RULES_HEADER:
			return { "grh", nullptr };
		case Type::MODELS:
		case Type::BEHAVIOURS:
		case Type::MATERIALS:
			return { "bin", nullptr };
		default:
			return { "*" }; // any file type
		}
	}

	static PCKAssetFile::Type getPreferredType(const std::string& filepath)
	{
		std::filesystem::path path(filepath);
		std::string filename = path.filename().string();
		std::string ext = path.extension().string();

		if (!ext.empty() && ext[0] == '.')
			ext.erase(0, 1);
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
		std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);

		if (filename == "0")
			return Type::INFO;
		if (ext == "png" || ext == "tga")
		{
			if (filename._Starts_with("dlcskin"))
				return Type::SKIN;
			else if (filename._Starts_with("dlccape"))
				return Type::CAPE;
		}
		if (ext == "pck")
		{
			if (filename == "audio.pck")
				return Type::AUDIO_DATA;
			else if (filename == "skins.pck")
				return Type::SKIN_DATA;
			else
				return Type::TEXTURE_PACK_INFO;
		}
		if (ext == "loc")
			return Type::LOCALISATION;
		if (ext == "grf")
			return Type::GAME_RULES;
		if (ext == "col")
			return Type::COLOUR_TABLE;
		if (ext == "grh")
			return Type::GAME_RULES_HEADER;
		if (ext == "bin")
		{
			if (filename == "models.bin")
				return Type::MODELS;
			else if (filename == "behaviours.bin")
				return Type::BEHAVIOURS;
			else if (filename == "entitymaterials.bin")
				return Type::MATERIALS;
		}

		return Type::TEXTURE;
	}

	PCKAssetFile(const std::string& path, const std::vector<unsigned char>& data, Type assetType)
		: mAssetType(assetType), mData(data), mPath(path) {
	}

	PCKAssetFile(const std::string& path, Type assetType)
		: PCKAssetFile(path, {}, assetType) {
	}

	~PCKAssetFile() {
		mData.clear();
		mPath.clear();
	}

	// Gets the file size, in bytes
	std::size_t getFileSize() const;

	// Gets the file data as a const unsigned char vector
	const std::vector<unsigned char>& getData() const;

	// Sets the file data with a const unsigned char vector
	void setData(const std::vector<unsigned char>& data);

	// Gets the file path
	const std::string& getPath() const;

	// Sets the file path
	void setPath(const std::string& inpath);

	// Get the asset type
	Type getAssetType() const;

	// Checks if the file is a Skin, Cape or Texture Asset Type
	bool isImageType() const;

	// Adds a property to the file
	void addProperty(const std::string& key, const std::u16string& value);

	// Removes a property from the file
	void removeProperty(int index);

	// Sets a property's key and value
	void setPropertyAtIndex(int index, const std::string& key, const std::u16string& value);

	// Clears the file's properties
	void clearProperties();

	// Returns the files properties as a... vector of a pair of a string and u16string
	const std::vector<Property>& getProperties() const;

private:
	Type mAssetType{ Type::SKIN };
	std::vector<unsigned char> mData;
	std::string mPath;
	std::vector<Property> mProperties;
};