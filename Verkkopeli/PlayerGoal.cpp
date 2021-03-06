#include <Book/PlayerGoal.hpp>
#include <Book/DataTables.hpp>
#include <Book/Utility.hpp>
#include <Book/CommandQueue.hpp>
#include <Book/SoundNode.hpp>
#include <Book/NetworkNode.hpp>
#include <Book/ResourceHolder.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <cmath>


using namespace std::placeholders;

namespace
{
	const std::vector<PlayerGoalData> Table = initializePlayerGoalData();
}

PlayerGoal::PlayerGoal(Type type, const TextureHolder& textures, const FontHolder& fonts)
	: Entity(Table[type].hitpoints)
	, mType(type)
	, mSprite(textures.get(Table[type].texture), Table[type].textureRect)
	, mExplosion(textures.get(Textures::Explosion))
	, mShowExplosion(false)
	, mExplosionBegan(false)
	, mDirectionIndex(0)
	, mIdentifier(0)
{
	mExplosion.setFrameSize(sf::Vector2i(256, 256));
	mExplosion.setNumFrames(16);
	mExplosion.setDuration(sf::seconds(1));

	centerOrigin(mSprite);
	centerOrigin(mExplosion);

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	mHealthDisplay = healthDisplay.get();
	attachChild(std::move(healthDisplay));

	updateTexts();
}

void PlayerGoal::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (isDestroyed() || mShowExplosion)
		target.draw(mExplosion, states);
	else
		target.draw(mSprite, states);
}

void PlayerGoal::setShowExplosion(bool show)
{
	mShowExplosion = show;
}

void PlayerGoal::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	// Update texts and roll animation
	updateTexts();

	if (mExplosion.isFinished())
	{
		setShowExplosion(false);
		mExplosion.restart();
	}

	// Entity has been destroyed: Possibly drop pickup, mark for removal
	if (mShowExplosion)
	{
		mExplosion.update(dt);

		// Play explosion sound only once
		if (!mExplosionBegan)
		{
			// Play sound effect
			SoundEffect::ID soundEffect = (randomInt(2) == 0) ? SoundEffect::Explosion1 : SoundEffect::Explosion2;
			playLocalSound(commands, soundEffect);

			mExplosionBegan = true;
		}
		return;
	}

	//updateMovement(dt);
	Entity::updateCurrent(dt, commands);
}

//void PlayerGoal::updateMovement(sf::Time dt)
//{
//	const std::vector<Direction>& directions = Table[mType].directions;
//	if (!directions.empty())
//	{
//		// Compute velocity from direction
//		float radians = toRadian(directions[mDirectionIndex].angle + 90.f);
//		float vx = getMaxSpeed() * std::cos(radians);
//		float vy = getMaxSpeed() * std::sin(radians);
//
//		setVelocity(vx, vy);
//
//		mTravelledDistance += getMaxSpeed() * dt.asSeconds();
//	}
//}

unsigned int PlayerGoal::getCategory() const
{
	return Category::Goal;
}

sf::FloatRect PlayerGoal::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

bool PlayerGoal::isMarkedForRemoval() const
{
	return isDestroyed() && (mExplosion.isFinished() || !mShowExplosion);
}

void PlayerGoal::remove()
{
	Entity::remove();
	mShowExplosion = false;
}

bool PlayerGoal::isAllied() const
{
	return mType == Goal1 || mType == Goal2;
}

//float PlayerGoal::getMaxSpeed() const
//{
//	return Table[mType].speed;
//}

void PlayerGoal::playLocalSound(CommandQueue& commands, SoundEffect::ID effect)
{
	sf::Vector2f worldPosition = getWorldPosition();

	Command command;
	command.category = Category::SoundEffect;
	command.action = derivedAction<SoundNode>(
		[effect, worldPosition](SoundNode& node, sf::Time)
	{
		node.playSound(effect, worldPosition);
	});

	commands.push(command);
}

int	PlayerGoal::getIdentifier()
{
	return mIdentifier;
}

void PlayerGoal::setIdentifier(int identifier)
{
	mIdentifier = identifier;
}

void PlayerGoal::updateTexts()
{
	// Display hitpoints
	//if (isDestroyed())
	//mHealthDisplay->setString("");
	//else
	mHealthDisplay->setString(toString(getHitpoints()) + " HP");
	mHealthDisplay->setPosition(150.f, 0.f);
	mHealthDisplay->setRotation(-getRotation());
}