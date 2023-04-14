#include "object.h"
#include "core.h"


namespace ObjectModel
{
	Object::Object(std::string name = "default")
	{
		setName(name);
		wrapper = static_cast<uint8_t>(Wrapper::OBJECT);
		size += (sizeof int16_t) * 4;
	}

	void Object::addEntity(Root* r)
	{
		this->entities.push_back(r);
		count += 1;
		size += r->getSize();
	}

	void Object::pack(std::vector<uint8_t>& buffer, int16_t& iterator)
	{
		Core::encode<std::string>(buffer, iterator, name);
		Core::encode<int16_t>(buffer, iterator, nameLength);
		Core::encode<int8_t>(buffer, iterator, wrapper);
		Core::encode<int16_t>(buffer, iterator, count);

		for (const auto r : entities)
		{
			r->pack(buffer, iterator);
		}

		Core::encode<int32_t>(buffer, iterator, size);

	}

	Root* Object::findByName(std::string name)
	{
		for (const auto r : entities)
		{
			if (r->getName() == name)
			{
				return r;
			}
		}
		std::cout << "no as such" << std::endl;
		return new Object("ninjia");
	}
}
