#include "PCKAssetFile.h"

std::size_t PCKAssetFile::getFileSize() const {
	return mData.size();
}

const std::vector<unsigned char>& PCKAssetFile::getData() const {
	return mData;
}

void PCKAssetFile::setData(const std::vector<unsigned char>& data) {
	mData = data;
}

const std::string& PCKAssetFile::getPath() const {
	return mPath; 
}

void PCKAssetFile::setPath(const std::string& inpath) {
	mPath = inpath;
}

PCKAssetFile::Type PCKAssetFile::getAssetType() const {
	return mAssetType; 
}

void PCKAssetFile::addProperty(const std::string& key, const std::string& value) {
	mProperties.push_back(std::pair<std::string, std::string>(key, value));
}

const PCKAssetFile::Type IMAGE_ASSET_TYPES[]{ PCKAssetFile::Type::SKIN, PCKAssetFile::Type::CAPE, PCKAssetFile::Type::TEXTURE };

bool PCKAssetFile::isImageType() const
{
	return std::find(std::begin(IMAGE_ASSET_TYPES), std::end(IMAGE_ASSET_TYPES), mAssetType) != std::end(IMAGE_ASSET_TYPES);
}