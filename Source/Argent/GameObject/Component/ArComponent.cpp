#include "ArComponent.h"

Argent::Component::ArComponent::ArComponent(const ArComponent&& com) noexcept
{
	this->name = com.name;
	this->owner = com.owner;
}
