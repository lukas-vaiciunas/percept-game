#pragma once

class BulletPool;
class CollidableTile;
class HealthDisplay;
class EventQueue;

#include <unordered_map>
#include "TickAlarm.h"
#include "Entity.h"
#include "Animation.h"

class Player : public Entity
{
private:
	enum AnimationType : unsigned char
	{
		Idle = 1,
		Moving = 2
	};
	enum AnimationFace : unsigned char
	{
		Up = 4,
		Down = 8,
		Left = 16,
		Right = 32
	};

	std::unordered_map<unsigned char, Animation> animations_;
	AnimationType animationType_;
	AnimationFace animationFace_;

	TickAlarm safeAlarm_;
	bool isSafe_;

	bool movingUp_;
	bool movingDown_;
	bool movingLeft_;
	bool movingRight_;

	float velX_;
	float velY_;

	void updateBulletCollisions_(
		const BulletPool &bulletPool,
		ParticlePool &particlePool,
		HealthDisplay &healthDisplay,
		EventQueue &gameEventQueue);

	void updateTileCollisionsX_(
		const std::vector<CollidableTile *> &possibleCollidableTiles,
		const std::vector<size_t> &visibleCollidableTileIndices,
		float oldX,
		float levelWidth);

	void updateTileCollisionsY_(
		const std::vector<CollidableTile *> &possibleCollidableTiles,
		const std::vector<size_t> &visibleCollidableTileIndices,
		float oldY,
		float levelHeight);

	void hurt_(
		unsigned int damage,
		ParticlePool &particlePool,
		HealthDisplay &healthDisplay,
		EventQueue &gameEventQueue);
public:
	Player(float x, float y, float width, float height, float speed, unsigned int health, const char *animationSheetPath, unsigned int numSafeTicks);

	void updateOnTick(
		const BulletPool &bulletPool,
		ParticlePool &particlePool,
		HealthDisplay &healthDisplay,
		const std::vector<CollidableTile *> &possibleCollidableTiles,
		const std::vector<size_t> &visibleCollidableTileIndices,
		EventQueue &gameEventQueue,
		float levelWidth, float levelHeight);

	void updateOnMousePress(unsigned int allegroMouseButton, int worldMouseX, int worldMouseY, BulletPool &bulletPool);
	void updateOnKeyPress(int allegroKeycode);
	void updateOnKeyRelease(int allegroKeycode);

	void render() const override;

	void spawn(float x, float y);

	void addMaxHealth(unsigned int amount);
	void subMaxHealth(unsigned int amount);
	void setMaxHealth(unsigned int amount);

	void addDamage(unsigned int amount);
	void subDamage(unsigned int amount);
	void setDamage(unsigned int amount);

	bool isMoving() const;
};