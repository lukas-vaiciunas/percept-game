#pragma once

class ChunkInfo;
class ImageData;

#include <vector>
#include "Image.h"

class MinimapDisplay
{
private:
	Image background_;
	Image iconQuestionMark_;
	Image iconPlayerBlip_;

	unsigned int levelWidthInChunks_;
	unsigned int levelHeightInChunks_;

	float x_;
	float y_;

	float playerRelX_;
	float playerRelY_;
public:
	MinimapDisplay(
		float x,
		float y,
		unsigned int levelWidthInChunks,
		unsigned int levelHeightInChunks);

	void updateOnPlayerMove(float x, float y);

	void render(const std::vector<std::vector<ChunkInfo>> &chunks, const ImageData &imageData) const;
};