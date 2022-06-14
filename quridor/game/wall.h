#pragma once

#include <Onyx/Onyx.h>

namespace quridor
{
    class Wall : public Onyx::ECS::ScriptableEntity
    {
    private:
        glm::vec3 color = { 0, 1, 1 };

    public:
        Wall() = default;
        Wall(Onyx::ECS::Entity entity) : Onyx::ECS::ScriptableEntity(entity) {}
    };
}
