#pragma once

class ChunkInfo
{
public:
	enum class ChunkType : unsigned char
	{
		Empty,
		Enemy,
		Boss
	};

	ChunkInfo(ChunkType chunkType, unsigned int iconImageId);

	void discover();

	void setType(ChunkType type);
	void setIconImageId(unsigned int iconImageId);

	ChunkType getChunkType() const;
	unsigned int getIconImageId() const;
	bool isDiscovered() const;
private:
	ChunkType chunkType_;
	unsigned int iconImageId_;
	bool isDiscovered_;
};