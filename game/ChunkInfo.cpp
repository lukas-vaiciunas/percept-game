#include "ChunkInfo.h"

ChunkInfo::ChunkInfo(ChunkType chunkType, unsigned int iconImageId) :
	chunkType_(chunkType),
	iconImageId_(iconImageId),
	isDiscovered_(false)
{}

void ChunkInfo::discover()
{
	isDiscovered_ = true;
}

void ChunkInfo::setType(ChunkType type)
{
	chunkType_ = type;
}

void ChunkInfo::setIconImageId(unsigned int iconImageId)
{
	iconImageId_ = iconImageId;
}

ChunkInfo::ChunkType ChunkInfo::getChunkType() const
{
	return chunkType_;
}

unsigned int ChunkInfo::getIconImageId() const
{
	return iconImageId_;
}

bool ChunkInfo::isDiscovered() const
{
	return isDiscovered_;
}