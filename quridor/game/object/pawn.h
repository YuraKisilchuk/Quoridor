#pragma once

#include <Onyx/Onyx.h>

#include "quridor/game/common.h"

namespace quridor
{
    class Pawn : public Onyx::ECS::ScriptableEntity
    {
    private:
        friend class QuridorState;
        friend class Player;

        BoardPos pos = {};
        glm::vec3 color = { 0, 0, 1 };

    public:
        Pawn() = default;
        Pawn(Onyx::ECS::Entity entity, const glm::vec3& color, BoardPos pos) :
            Onyx::ECS::ScriptableEntity(entity),
            color(color),
            pos(pos) {}
    };
}
