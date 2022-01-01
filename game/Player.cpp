#include <cmath>
#include "Player.h"
#include "BulletPool.h"
#include "CollidableTile.h"
#include "HealthDisplay.h"
#include "EventQueue.h"
#include "GameEvents.h"

Player::Player(float x, float y, float width, float height, float speed, unsigned int health, const char *animationSheetPath, unsigned int numSafeTicks) :
	Entity(x, y, width, height, speed, health, 1),
	animationType_(AnimationType::Idle),
	animationFace_(AnimationFace::Down),
	safeAlarm_(numSafeTicks),
	isSafe_(false),
	movingUp_(false),
	movingDown_(false),
	movingLeft_(false),
	movingRight_(false),
	velX_(0.0f),
	velY_(0.0f)
{
	ALLEGRO_BITMAP *sheetBitmap = al_load_bitmap(animationSheetPath);

	animations_ = {
		{
			AnimationType::Idle | AnimationFace::Down,
			Animation(
			{
				Image(sheetBitmap, 0, 0, 16, 16),
				Image(sheetBitmap, 16, 0, 16, 16),
				Image(sheetBitmap, 32, 0, 16, 16),
				Image(sheetBitmap, 48, 0, 16, 16)
			},
			10),
		},
		{
			AnimationType::Moving | AnimationFace::Down,
			Animation(
			{
				Image(sheetBitmap, 0, 16, 16, 16),
				Image(sheetBitmap, 16, 16, 16, 16),
				Image(sheetBitmap, 32, 16, 16, 16),
				Image(sheetBitmap, 48, 16, 16, 16)
			},
			10),
		},
		{
			AnimationType::Idle | AnimationFace::Up,
			Animation(
			{
				Image(sheetBitmap, 0, 32, 16, 16),
				Image(sheetBitmap, 16, 32, 16, 16),
				Image(sheetBitmap, 32, 32, 16, 16),
				Image(sheetBitmap, 48, 32, 16, 16)
			},
			10),
		},
		{
			AnimationType::Moving | AnimationFace::Up,
			Animation(
			{
				Image(sheetBitmap, 0, 48, 16, 16),
				Image(sheetBitmap, 16, 48, 16, 16),
				Image(sheetBitmap, 32, 48, 16, 16),
				Image(sheetBitmap, 48, 48, 16, 16)
			},
			10),
		},
		{
			AnimationType::Idle | AnimationFace::Right,
			Animation(
			{
				Image(sheetBitmap, 0, 64, 16, 16),
				Image(sheetBitmap, 16, 64, 16, 16),
				Image(sheetBitmap, 32, 64, 16, 16),
				Image(sheetBitmap, 48, 64, 16, 16)
			},
			10),
		},
		{
			AnimationType::Moving | AnimationFace::Right,
			Animation(
			{
				Image(sheetBitmap, 0, 80, 16, 16),
				Image(sheetBitmap, 16, 80, 16, 16),
				Image(sheetBitmap, 32, 80, 16, 16),
				Image(sheetBitmap, 48, 80, 16, 16)
			},
			10),
		},
		{
			AnimationType::Idle | AnimationFace::Left,
			Animation(
			{
				Image(sheetBitmap, 0, 96, 16, 16),
				Image(sheetBitmap, 16, 96, 16, 16),
				Image(sheetBitmap, 32, 96, 16, 16),
				Image(sheetBitmap, 48, 96, 16, 16)
			},
			10),
		},
		{
			AnimationType::Moving | AnimationFace::Left,
			Animation(
			{
				Image(sheetBitmap, 0, 112, 16, 16),
				Image(sheetBitmap, 16, 112, 16, 16),
				Image(sheetBitmap, 32, 112, 16, 16),
				Image(sheetBitmap, 48, 112, 16, 16)
			},
			10),
		}
	};

	al_destroy_bitmap(sheetBitmap);
}

void Player::updateOnTick(
	const BulletPool &bulletPool,
	ParticlePool &particlePool,
	HealthDisplay &healthDisplay,
	const std::vector<CollidableTile *> &possibleCollidableTiles,
	const std::vector<size_t> &visibleCollidableTileIndices,
	EventQueue &gameEventQueue,
	float levelWidth, float levelHeight)
{
	animations_.at(animationType_ | animationFace_).updateOnTick();

	if (velX_ < 0.0f)
	{
		animationType_ = AnimationType::Moving;
		animationFace_ = AnimationFace::Left;
	}
	else if (velX_ > 0.0f)
	{
		animationType_ = AnimationType::Moving;
		animationFace_ = AnimationFace::Right;
	}
	else if (velY_ < 0.0f)
	{
		animationType_ = AnimationType::Moving;
		animationFace_ = AnimationFace::Up;
	}
	else if (velY_ > 0.0f)
	{
		animationType_ = AnimationType::Moving;
		animationFace_ = AnimationFace::Down;
	}
	else
	{
		animationType_ = AnimationType::Idle;
	}

	float oldX = this->x();
	this->moveX_(velX_);
	this->updateTileCollisionsX_(possibleCollidableTiles, visibleCollidableTileIndices, oldX, levelWidth);

	float oldY = this->y();
	this->moveY_(velY_);
	this->updateTileCollisionsY_(possibleCollidableTiles, visibleCollidableTileIndices, oldY, levelHeight);

	if (isSafe_)
	{
		safeAlarm_.updateOnTick();
		if (safeAlarm_.isAlarm())
		{
			safeAlarm_.reset();
			isSafe_ = false;
		}
	}
	else this->updateBulletCollisions_(bulletPool, particlePool, healthDisplay, gameEventQueue);
}

void Player::updateOnMousePress(unsigned int allegroMouseButton, int mouseWorldX, int mouseWorldY, BulletPool &bulletPool)
{
	if (!(allegroMouseButton & 1) || bulletPool.full())
		return;

	const float speed = 3.0f;
	float cX = this->x() + this->width() * 0.5f - 4.0f;
	float cY = this->y() + this->height() * 0.5f - 4.0f;
	float angleRad = atan2f(mouseWorldY - cY, mouseWorldX - cX);

	if (!bulletPool.full())
		bulletPool.add(
			new Bullet(
				cX,
				cY,
				speed * std::cosf(angleRad) + velX_,
				speed * std::sinf(angleRad) + velY_,
				this->damage(),
				Image("img/dagger.png"),
				20,
				true));
}

void Player::updateOnKeyPress(int allegroKeycode)
{
	if (allegroKeycode == ALLEGRO_KEY_W)
	{
		movingUp_ = true;

		velY_ = (movingDown_ ? 0.0f : -this->getSpeed_());
	}
	else if (allegroKeycode == ALLEGRO_KEY_A)
	{
		movingLeft_ = true;

		velX_ = (movingRight_ ? 0.0f : -this->getSpeed_());
	}
	else if (allegroKeycode == ALLEGRO_KEY_S)
	{
		movingDown_ = true;

		velY_ = (movingUp_ ? 0.0f : this->getSpeed_());
	}
	else if (allegroKeycode == ALLEGRO_KEY_D)
	{
		movingRight_ = true;

		velX_ = (movingLeft_ ? 0.0f : this->getSpeed_());
	}
}

void Player::updateOnKeyRelease(int allegroKeycode)
{
	if (allegroKeycode == ALLEGRO_KEY_W)
	{
		movingUp_ = false;

		velY_ = (movingDown_ ? this->getSpeed_() : 0.0f);
	}
	else if (allegroKeycode == ALLEGRO_KEY_A)
	{
		movingLeft_ = false;

		velX_ = (movingRight_ ? this->getSpeed_() : 0.0f);
	}
	else if (allegroKeycode == ALLEGRO_KEY_S)
	{
		movingDown_ = false;

		velY_ = (movingUp_ ? -this->getSpeed_() : 0.0f);
	}
	else if (allegroKeycode == ALLEGRO_KEY_D)
	{
		movingRight_ = false;

		velX_ = (movingLeft_ ? -this->getSpeed_() : 0.0f);
	}
}

void Player::render() const
{
	if (isSafe_)
	{
		animations_
			.at(animationType_ | animationFace_)
			.currentFrame()
			.renderTinted(this->x(), this->y(), al_map_rgba_f(1.0f, 0.5f, 0.5f, safeAlarm_.numTicks() % 10 / 10.0f));
	}
	else
	{
		animations_
			.at(animationType_ | animationFace_)
			.currentFrame()
			.render(this->x(), this->y());
	}
}

void Player::spawn(float x, float y)
{
	this->setX_(x);
	this->setY_(y);

	safeAlarm_.reset();
	isSafe_ = false;

	movingUp_ = false;
	movingDown_ = false;
	movingLeft_ = false;
	movingRight_ = false;

	velX_ = 0.0f;
	velY_ = 0.0f;
}

void Player::addMaxHealth(unsigned int amount)
{
	this->addMaxHealth_(amount);
}

void Player::subMaxHealth(unsigned int amount)
{
	this->subMaxHealth_(amount);
}

void Player::setMaxHealth(unsigned int amount)
{
	this->setMaxHealth_(amount);
}

void Player::addDamage(unsigned int amount)
{
	this->addDamage_(amount);
}

void Player::subDamage(unsigned int amount)
{
	this->subDamage_(amount);
}

void Player::setDamage(unsigned int amount)
{
	this->setDamage_(amount);
}

bool Player::isMoving() const
{
	return velX_ != 0.0f || velY_ != 0.0f;

	//return movingUp_ || movingDown_ || movingLeft_ || movingRight_;
}

void Player::updateBulletCollisions_(
	const BulletPool &bulletPool,
	ParticlePool &particlePool,
	HealthDisplay &healthDisplay,
	EventQueue &gameEventQueue)
{
	for (auto it = bulletPool.cbegin(); it != bulletPool.cend(); ++it)
	{
		if (!(*it)->isFromPlayer() && this->isCollidingWith(**it))
		{
			this->hurt_((*it)->damage(), particlePool, healthDisplay, gameEventQueue);
			break;
		}
	}
}

void Player::updateTileCollisionsX_(
	const std::vector<CollidableTile *> &possibleCollidableTiles,
	const std::vector<size_t> &visibleCollidableTileIndices,
	float oldX,
	float levelWidth)
{
	for (const size_t &i : visibleCollidableTileIndices)
	{
		const CollidableTile &target = *possibleCollidableTiles.at(i);

		if (!this->isCollidingWith(target))
			continue;

		if (oldX <= target.x() - this->width() && this->x() > target.x() - this->width())
		{
			this->setX_(target.x() - this->width());
		}
		else if (oldX >= target.x() + target.width() && this->x() < target.x() + target.width())
		{
			this->setX_(target.x() + target.width());
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

void Player::updateTileCollisionsY_(
	const std::vector<CollidableTile *> &possibleCollidableTiles,
	const std::vector<size_t> &visibleCollidableTileIndices,
	float oldY,
	float levelHeight)
{
	for (const size_t &i : visibleCollidableTileIndices)
	{
		const CollidableTile &target = *possibleCollidableTiles.at(i);

		if (!this->isCollidingWith(target))
			continue;

		if (oldY <= target.y() - this->height() && this->y() > target.y() - this->height())
		{
			this->setY_(target.y() - this->height());
		}
		else if (oldY >= target.y() + target.height() && this->y() < target.y() + target.height())
		{
			this->setY_(target.y() + target.height());
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

void Player::hurt_(
	unsigned int damage,
	ParticlePool &particlePool,
	HealthDisplay &healthDisplay,
	EventQueue &gameEventQueue)
{
	isSafe_ = true;
	this->onHurt_(particlePool);

	if (this->isFatal_(damage))
	{
		this->onDeath_(particlePool);
		gameEventQueue.send(new EventPlayerDeath());
	}
	else
	{
		Entity::hurt_(damage);
	}

	healthDisplay.update(this->health(), this->maxHealth());
}