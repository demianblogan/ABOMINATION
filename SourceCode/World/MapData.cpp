#include "World/MapData.h"

namespace Abomination::World
{
    const std::string* FindProperty(const MapEntity& entity, const std::string& key)
    {
        const auto iterator = entity.properties.find(key);
        if (iterator == entity.properties.end())
            return nullptr;

        return &iterator->second;
    }
}
