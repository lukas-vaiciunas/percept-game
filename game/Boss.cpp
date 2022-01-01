#include "Boss.h"
#include "Animation.h"
#include "Player.h"
#include "BulletPool.h"
#include "EventQueue.h"
#include "GameEvents.h"

Boss::Boss(
	float x, float y,
	float width, float height,
	float speed,
	unsigned int health,
	unsigned int itemLootTableId,
	unsigned int numPhaseTicks,
	unsigned int numRestTicks,
	unsigned int numSafeTicks,
	unsigned int numHealTicks,
	const std::string &nextLevelPath,
	const char *const animationPath,
	const GridPartition<Enemy> &enemyPartition)
	:
	Enemy(x, y, width, height, speed, health, itemLootTableId),
	nextLevelPath_(nextLevelPath),
	animation_(nullptr),
	coords_(enemyPartition.posToCoords(x + width * 0.5f, y + height * 0.5f)),
	phase_(Phase::Circle),
	phaseAlarm_(numPhaseTicks),
	restAlarm_(numRestTicks),
	safeAlarm_(numSafeTicks),
	healAlarm_(numHealTicks),
	isResting_(false),
	isSafe_(false)
{
	ALLEGRO_BITMAP *sheetBitmap = al_load_bitmap(animationPath);

	animation_ = new Animation(
		{
			Image(sheetBitmap, 0, 0, width, height),
			Image(sheetBitmap, width, 0, width, height),
			Image(sheetBitmap, width * 2, 0, width, height),
			Image(sheetBitmap, width * 3, 0, width, height),
		},
		10);

	al_destroy_bitmap(sheetBitmap);
}

Boss::Boss(const Boss &other) :
	Enemy(other),
	nextLevelPath_(other.nextLevelPath_),
	animation_(nullptr),
	coords_(other.coords_),
	phase_(Phase::Circle),
	phaseAlarm_(other.phaseAlarm_.targetNumTicks()),
	restAlarm_(other.restAlarm_.targetNumTicks()),
	safeAlarm_(other.safeAlarm_.targetNumTicks()),
	healAlarm_(other.healAlarm_.targetNumTicks()),
	isResting_(false),
	isSafe_(false)
{
	animation_ = new Animation(*other.animation_);
}

Boss &Boss::operator=(const Boss &other)
{
	delete animation_;

	Enemy::operator=(other);
	nextLevelPath_ = other.nextLevelPath_;
	animation_ = new Animation(*other.animation_);
	coords_ = other.coords_;
	phase_ = Phase::Circle;
	phaseAlarm_ = other.phaseAlarm_.targetNumTicks();
	restAlarm_ = other.restAlarm_.targetNumTicks();
	safeAlarm_ = other.safeAlarm_.targetNumTicks();
	healAlarm_ = other.healAlarm_.targetNumTicks();
	isResting_ = false;
	isSafe_ = false;

	return *this;
}

Boss::~Boss()
{
	delete animation_;
}

void Boss::updateOnTick(
	const Player &player,
	BulletPool &bulletPool,
	ParticlePool &particlePool,
	GridPartition<Enemy> &enemyPartition,
	const std::vector<CollidableTile *> &possibleCollidableTiles,
	const ItemData &itemData,
	const ItemLootTableData &itemLootTableData,
	GridPartition<ItemDrop> &itemDropPartition,
	EventQueue &gameEventQueue,
	float levelWidth, float levelHeight)
{
	animation_->updateOnTick();

	healAlarm_.updateOnTick();
	if (healAlarm_.isAlarm())
	{
		healAlarm_.reset();
		this->heal(10);
	}

	if (isResting_)
	{
		restAlarm_.updateOnTick();
		if (restAlarm_.isAlarm())
		{
			restAlarm_.reset();
			isResting_ = false;

			switch (phase_)
			{
			case Phase::Circle:
				phase_ = Phase::Spiral;
				break;
			case Phase::Spiral:
				phase_ = Phase::Target;
				break;
			case Phase::Target:
				phase_ = Phase::Circle;
				break;
			}
		}
	}
	else
	{
		const float bulletSpeed = 3.0f;
		float cX = this->x() + this->width() * 0.5f;
		float cY = this->y() + this->height() * 0.5f;

		switch (phase_)
		{
		case Phase::Circle:
			if (phaseAlarm_.numTicks() % 10 == 0)
				for(float angleRad = 0.0f; angleRad < 6.28f; angleRad += 0.7f)
					this->fireBullet_(
						bulletPool,
						cX - 4.0f,
						cY - 4.0f,
						bulletSpeed,
						6.28f * phaseAlarm_.numTicks() / phaseAlarm_.targetNumTicks() + angleRad);
			break;
		case Phase::Spiral:
			if (phaseAlarm_.numTicks() % 10 == 0)
				this->fireBullet_(
					bulletPool,
					cX - 4.0f,
					cY - 4.0f,
					bulletSpeed,
					6.28f * phaseAlarm_.numTicks() / phaseAlarm_.targetNumTicks() * 2.0f);
			break;
		case Phase::Target:
		{
			float dX = (player.x() + player.width() * 0.5f) - cX;
			float dY = (player.y() + player.height() * 0.5f) - cY;

			float angleRadToPlayer = std::atan2f(dY, dX);

			if (phaseAlarm_.numTicks() % 10 == 0)
				this->fireBullet_(bulletPool, cX - 4.0f, cY - 4.0f, bulletSpeed, angleRadToPlayer);
			break;
		}
		}

		phaseAlarm_.updateOnTick();
		if (phaseAlarm_.isAlarm())
		{
			phaseAlarm_.reset();
			isResting_ = true;
		}
	}

	if (isSafe_)
	{
		safeAlarm_.updateOnTick();
		if (safeAlarm_.isAlarm())
		{
			safeAlarm_.reset();
			isSafe_ = false;
		}
	}
	else
	{
		this->updateBulletCollisions_(
			bulletPool,
			particlePool,
			enemyPartition,
			itemData,
			itemLootTableData,
			itemDropPartition,
			gameEventQueue);
	}
}

void Boss::render() const
{
	animation_->currentFrame().render(this->x(), this->y());
}

void Boss::hurt_(
	unsigned int damage,
	ParticlePool &particlePool,
	GridPartition<Enemy> &enemyPartition,
	const ItemData &itemData,
	const ItemLootTableData &itemLootTableData,
	GridPartition<ItemDrop> &itemDropPartition,
	EventQueue &gameEventQueue)
{
	isSafe_ = true;

	this->onHurt_(particlePool);

	if (this->isFatal_(damage))
	{
		this->onDeath_(
			particlePool,
			enemyPartition,
			itemData,
			itemLootTableData,
			itemDropPartition,
			gameEventQueue);
	}
	else
	{
		Entity::hurt_(damage);
	}
}

void Boss::onDeath_(
	ParticlePool &particlePool,
	GridPartition<Enemy> &enemyPartition,
	const ItemData &itemData,
	const ItemLootTableData &itemLootTableData,
	GridPartition<ItemDrop> &itemDropPartition,
	EventQueue &gameEventQueue)
{
	Entity::onDeath_(particlePool);

	enemyPartition.removeAtCoords(coords_, this);

	this->dropItem_(itemData, itemLootTableData, itemDropPartition);

	gameEventQueue.send(new EventSpawnDoor(
		this->x() + this->width() * 0.5f - 8.0f,
		this->y() + this->height() * 0.5f - 8.0f,
		nextLevelPath_));
}

void Boss::updateBulletCollisions_(
	const BulletPool &bulletPool,
	ParticlePool &particlePool,
	GridPartition<Enemy> &enemyPartition,
	const ItemData &itemData,
	const ItemLootTableData &itemLootTableData,
	GridPartition<ItemDrop> &itemDropPartition,
	EventQueue &gameEventQueue)
{
	for (auto it = bulletPool.cbegin(); it != bulletPool.cend(); ++it)
	{
		if ((*it)->isFromPlayer() && this->isCollidingWith(**it))
		{
			this->hurt_(
				(*it)->damage(),
				particlePool,
				enemyPartition,
				itemData,
				itemLootTableData,
				itemDropPartition,
				gameEventQueue);

			break;
		}
	}
}

void Boss::fireBullet_(BulletPool &bulletPool, float x, float y, float bulletSpeed, float angleRad)
{
	if(!bulletPool.full())
		bulletPool.add(
			new Bullet(
				x,
				y,
				bulletSpeed * std::cosf(angleRad),
				bulletSpeed * std::sinf(angleRad),
				this->damage(),
				Image("img/dagger.png"),
				30,
				false));
}