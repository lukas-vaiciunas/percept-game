#include <cmath>
#include "MobileEnemy.h"
#include "Player.h"
#include "BulletPool.h"
#include "ParticlePool.h"
#include "CollidableTile.h"
#include "EnemySpawner.h"

MobileEnemy::MobileEnemy(
	float x, float y,
	float width, float height,
	float speed,
	unsigned int health,
	float idleRadius,
	float chaseRadius,
	float holdRadius,
	float retreatRadius,
	unsigned int itemLootTableId,
	const char *animationSheetPath,
	unsigned int numAttackTicks,
	unsigned int numStunTicks,
	const GridPartition<Enemy> &enemyPartition,
	EnemySpawner *const spawnerSource)
	:
	Enemy(x, y, width, height, speed, health, itemLootTableId),
	state_(State::Idle),
	spawnerSource_(spawnerSource),
	attackAlarm_(numAttackTicks),
	stunAlarm_(numStunTicks),
	oldCoords_(enemyPartition.posToCoords(x + width * 0.5f, y + height * 0.5f)),
	idleRadius_(idleRadius),
	chaseRadius_(chaseRadius),
	holdRadius_(holdRadius),
	retreatRadius_(retreatRadius),
	velX_(0.0f),
	velY_(0.0f),
	isMoving_(false),
	isStunned_(false),
	animationIndex_(0),
	renderFlag_(0)
{
	ALLEGRO_BITMAP *sheetBitmap = al_load_bitmap(animationSheetPath);

	animations_ = {
		Animation(
			{
				Image(sheetBitmap, 0, 0, 16, 16),
				Image(sheetBitmap, 16, 0, 16, 16),
				Image(sheetBitmap, 32, 0, 16, 16),
				Image(sheetBitmap, 48, 0, 16, 16)
			},
			10),
		Animation(
			{
				Image(sheetBitmap, 0, 16, 16, 16),
				Image(sheetBitmap, 16, 16, 16, 16),
				Image(sheetBitmap, 32, 16, 16, 16),
				Image(sheetBitmap, 48, 16, 16, 16)
			},
			10)
	};

	al_destroy_bitmap(sheetBitmap);
}

MobileEnemy::MobileEnemy(
	const MobileEnemy &other,
	float x, float y,
	const GridPartition<Enemy> &enemyPartition,
	EnemySpawner *const spawnerSource)
	:
	Enemy(
		x, y, other.width(), other.height(),
		other.getSpeed_(),
		other.maxHealth(),
		other.getItemLootTableId_()),
	state_(State::Idle),
	spawnerSource_(spawnerSource),
	animations_(other.animations_),
	attackAlarm_(other.attackAlarm_.targetNumTicks()),
	stunAlarm_(other.stunAlarm_.targetNumTicks()),
	oldCoords_(enemyPartition.posToCoords(x + other.width() * 0.5f, y + other.height() * 0.5f)),
	idleRadius_(other.idleRadius_),
	chaseRadius_(other.chaseRadius_),
	holdRadius_(other.holdRadius_),
	retreatRadius_(other.retreatRadius_),
	velX_(0.0f),
	velY_(0.0f),
	isMoving_(false),
	isStunned_(false),
	animationIndex_(0),
	renderFlag_(0)
{}

void MobileEnemy::updateOnTick(
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
	isMoving_ = (velX_ != 0.0f || velY_ != 0.0f);
	if (isMoving_)
	{
		float oldX = this->x();
		this->moveX_(velX_);
		this->updateTileCollisionsX_(possibleCollidableTiles, oldX, levelWidth);

		float oldY = this->y();
		this->moveY_(velY_);
		this->updateTileCollisionsY_(possibleCollidableTiles, oldY, levelHeight);

		this->updateOnMove_(enemyPartition);

		animationIndex_ = 1;

		if (!isStunned_)
			renderFlag_ = (velX_ < 0.0f ? ALLEGRO_FLIP_HORIZONTAL : 0);
	}
	else
	{
		animationIndex_ = 0;
	}

	animations_.at(animationIndex_).updateOnTick();

	if (isStunned_)
	{
		unsigned int numStunTicksLeft = stunAlarm_.targetNumTicks() - stunAlarm_.numTicks();

		velX_ -= velX_ / numStunTicksLeft;
		velY_ -= velY_ / numStunTicksLeft;

		stunAlarm_.updateOnTick();
		if (stunAlarm_.isAlarm())
		{
			stunAlarm_.reset();
			isStunned_ = false;
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
			itemDropPartition);

		if (!isStunned_) this->updateState_(player, bulletPool);
	}
}

void MobileEnemy::render() const
{
	animations_.at(animationIndex_).currentFrame().render(this->x(), this->y(), renderFlag_);
}

void MobileEnemy::hurt_(
	unsigned int damage,
	ParticlePool &particlePool,
	GridPartition<Enemy> &enemyPartition,
	const ItemData &itemData,
	const ItemLootTableData &itemLootTableData,
	GridPartition<ItemDrop> &itemDropPartition)
{
	this->onHurt_(particlePool);

	if (this->isFatal_(damage))
	{
		this->onDeath_(particlePool, enemyPartition, itemData, itemLootTableData, itemDropPartition);
	}
	else
	{
		Entity::hurt_(damage);
	}
}

void MobileEnemy::onDeath_(
	ParticlePool &particlePool,
	GridPartition<Enemy> &enemyPartition,
	const ItemData &itemData,
	const ItemLootTableData &itemLootTableData,
	GridPartition<ItemDrop> &itemDropPartition)
{
	Entity::onDeath_(particlePool);

	enemyPartition.removeAtCoords(oldCoords_, this);

	if (spawnerSource_)
		spawnerSource_->decrementNumSpawned();

	this->dropItem_(itemData, itemLootTableData, itemDropPartition);
}

void MobileEnemy::updateState_(const Player &player, BulletPool &bulletPool)
{
	float dX = (player.x() + player.width() * 0.5f) - (this->x() + this->width() * 0.5f);
	float dY = (player.y() + player.height() * 0.5f) - (this->y() + this->height() * 0.5f);
	float distanceToPlayer = std::sqrtf(std::powf(dX, 2.0f) + std::powf(dY, 2.0f));

	switch (state_)
	{
	case State::Idle:
		if (distanceToPlayer <= chaseRadius_) this->changeState_(State::Chasing);
		break;
	case State::Chasing:
	{
		float angleRadToPlayer = std::atan2f(dY, dX);

		this->updateAttacking_(bulletPool, angleRadToPlayer);

		velX_ = this->getSpeed_() * std::cosf(angleRadToPlayer);
		velY_ = this->getSpeed_() * std::sinf(angleRadToPlayer);

		if (distanceToPlayer >= idleRadius_) this->changeState_(State::Idle);
		else if (distanceToPlayer <= holdRadius_) this->changeState_(State::Holding);

		break;
	}
	case State::Holding:
	{
		float angleRadToPlayer = std::atan2f(dY, dX);

		this->updateAttacking_(bulletPool, angleRadToPlayer);

		if (distanceToPlayer > holdRadius_) this->changeState_(State::Chasing);
		else if (distanceToPlayer <= retreatRadius_) this->changeState_(State::Retreating);

		break;
	}
	case State::Retreating:
	{
		float angleRadToPlayer = std::atan2f(dY, dX);

		this->updateAttacking_(bulletPool, angleRadToPlayer);

		velX_ = this->getSpeed_() * -std::cosf(angleRadToPlayer);
		velY_ = this->getSpeed_() * -std::sinf(angleRadToPlayer);

		if (distanceToPlayer >= holdRadius_) this->changeState_(State::Holding);

		break;
	}
	}
}

void MobileEnemy::updateBulletCollisions_(
	const BulletPool &bulletPool,
	ParticlePool &particlePool,
	GridPartition<Enemy> &enemyPartition,
	const ItemData &itemData,
	const ItemLootTableData &itemLootTableData,
	GridPartition<ItemDrop> &itemDropPartition)
{
	for (auto it = bulletPool.cbegin(); it != bulletPool.cend(); ++it)
	{
		if ((*it)->isFromPlayer() && this->isCollidingWith(**it))
		{
			isStunned_ = true;
			velX_ = std::cosf((*it)->angleRad()) * 3.0f;
			velY_ = std::sinf((*it)->angleRad()) * 3.0f;

			this->hurt_(
				(*it)->damage(),
				particlePool,
				enemyPartition,
				itemData,
				itemLootTableData,
				itemDropPartition);

			break;
		}
	}
}

void MobileEnemy::updateTileCollisionsX_(
	const std::vector<CollidableTile *> &possibleCollidableTiles,
	float oldX,
	float levelWidth)
{
	for (const CollidableTile *const tile : possibleCollidableTiles)
	{
		if (!this->isCollidingWith(*tile))
			continue;

		if (oldX <= tile->x() - this->width() && this->x() > tile->x() - this->width())
		{
			this->setX_(tile->x() - this->width());
		}
		else if (oldX >= tile->x() + tile->width() && this->x() < tile->x() + tile->width())
		{
			this->setX_(tile->x() + tile->width());
		}
	}

	if (this->x() < 0.0f)
	{
		this->setX_(0.0f);
	}
	else if (this->x() > levelWidth - this->width())
	{
		this->setX_(levelWidth - this->width());
	}
}

void MobileEnemy::updateTileCollisionsY_(
	const std::vector<CollidableTile *> &possibleCollidableTiles,
	float oldY,
	float levelHeight)
{
	for (const CollidableTile *const tile : possibleCollidableTiles)
	{
		if (!this->isCollidingWith(*tile))
			continue;

		if (oldY <= tile->y() - this->height() && this->y() > tile->y() - this->height())
		{
			this->setY_(tile->y() - this->height());
		}
		else if (oldY >= tile->y() + tile->height() && this->y() < tile->y() + tile->height())
		{
			this->setY_(tile->y() + tile->height());
		}
	}

	if (this->y() < 0.0f)
	{
		this->setY_(0.0f);
	}
	else if (this->y() > levelHeight - this->height())
	{
		this->setY_(levelHeight - this->height());
	}
}

void MobileEnemy::updateAttacking_(BulletPool &bulletPool, float angleRadToPlayer)
{
	attackAlarm_.updateOnTick();
	if (attackAlarm_.isAlarm())
	{
		const float bulletSpeed = 3.0f;

		attackAlarm_.reset();

		if(!bulletPool.full())
			bulletPool.add(
				new Bullet(
					this->x() + this->width() * 0.5f - 4.0f,
					this->y() + this->height() * 0.5f - 4.0f,
					bulletSpeed * std::cosf(angleRadToPlayer),
					bulletSpeed * std::sinf(angleRadToPlayer),
					this->damage(),
					Image("img/dagger.png"),
					20,
					false));
	}
}

void MobileEnemy::updateOnMove_(GridPartition<Enemy> &enemyPartition)
{
	std::pair<unsigned int, unsigned int> newCoords = enemyPartition.posToCoords(
		this->x() + this->width() * 0.5f,
		this->y() + this->height() * 0.5f);

	if (newCoords != oldCoords_)
	{
		enemyPartition.removeAtCoords(oldCoords_, this);
		enemyPartition.addAtCoords(newCoords, this);
		oldCoords_ = newCoords;
	}
}

void MobileEnemy::changeState_(State toState)
{
	velX_ = 0.0f;
	velY_ = 0.0f;

	state_ = toState;
}