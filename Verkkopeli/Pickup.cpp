#include <Book/Pickup.hpp>
#include <Book/DataTables.hpp>
#include <Book/Category.hpp>
#include <Book/CommandQueue.hpp>
#include <Book/Utility.hpp>
#include <Book/ResourceHolder.hpp>

#include <SFML/Graphics/RenderTarget.hpp>


namespace
{
	const std::vector<PickupData> Table = initializePickupData();
}

Pickup::Pickup(Type type, const TextureHolder& textures)
: Entity(1)
, mType(type)
, mSprite(textures.get(Table[type].texture), Table[type].textureRect)
{
	centerOrigin(mSprite);
}

unsigned int Pickup::getCategory() const
{
	return Category::Pickup;
}

sf::FloatRect Pickup::getBoundingRect() const
{
	return getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

void Pickup::apply(PlayerBat& player) const
{
	Table[mType].action(player);
}

void Pickup::setWorldBounds(sf::FloatRect bounds)
{
	worldBounds = bounds;
}

void Pickup::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mSprite, states);
}

void Pickup::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	if (mType == Ball)
	{
		sf::FloatRect boundingRect = this->getBoundingRect();

		if (0 >= boundingRect.left
			|| worldBounds.width <= boundingRect.left + boundingRect.width)
		{
			if (-10 < this->getVelocity().x && this->getVelocity().x < 10)
				this->setVelocity(this->getVelocity().x * -1.25, this->getVelocity().y);
			else
				this->setVelocity(this->getVelocity().x * -1.0, this->getVelocity().y);
		}
		if (0 >= boundingRect.top ||
			worldBounds.height <= boundingRect.top + boundingRect.height)
		{
			if (-10 < this->getVelocity().y && this->getVelocity().y < 10)
				this->setVelocity(this->getVelocity().x, this->getVelocity().y * -1.25);
			else
				this->setVelocity(this->getVelocity().x, this->getVelocity().y * -1.0);
		}
	}
	move(this->getVelocity() * dt.asSeconds());
}

