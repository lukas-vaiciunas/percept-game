#include "MinimapDisplay.h"
#include "ImageData.h"
#include "ChunkInfo.h"

MinimapDisplay::MinimapDisplay(
	float x,
	float y,
	unsigned int levelWidthInChunks,
	unsigned int levelHeightInChunks)
	:
	background_("img/minimap-background.png"),
	iconQuestionMark_("img/icon-question-mark.png"),
	iconPlayerBlip_("img/icon-player-blip.png"),
	levelWidthInChunks_(levelWidthInChunks),
	levelHeightInChunks_(levelHeightInChunks),
	x_(x),
	y_(y),
	playerRelX_(0.0f),
	playerRelY_(0.0f)
{}

void MinimapDisplay::updateOnPlayerMove(float x, float y)
{
	playerRelX_ = x / ((levelWidthInChunks_ + 1) * 8.0f + 1.0f);
	playerRelY_ = y / ((levelHeightInChunks_ + 1) * 8.0f + 1.0f);
}

void MinimapDisplay::render(const std::vector<std::vector<ChunkInfo>> &chunks, const ImageData &imageData) const
{
	background_.render(x_, y_);

	for (unsigned int row = 0; row < levelHeightInChunks_; ++row)
	{
		for (unsigned int col = 0; col < levelWidthInChunks_; ++col)
		{
			if (chunks.at(row).at(col).isDiscovered())
			{
				imageData.getImage(chunks.at(row).at(col).getIconImageId()).render(
					x_ + 1 + col * 8.0f, y_ + 1 + row * 8.0f);
			}
			else
			{
				iconQuestionMark_.render(x_ + 1 + col * 8.0f, y_ + 1 + row * 8.0f);
			}
		}
	}

	iconPlayerBlip_.render(x_ + 1.0f + playerRelX_, y_ + 1.0f + playerRelY_);
}