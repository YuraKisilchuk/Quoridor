#pragma once

#include <Onyx/Onyx.h>

#include "common.h"

namespace quridor
{
    enum class WalledState
    {
        FREE,
        WALLED
    };

    struct CellWalls
    {
        WalledState north = WalledState::FREE;
        WalledState south = WalledState::FREE;
        WalledState west  = WalledState::FREE;
        WalledState east  = WalledState::FREE;
    };

    class Cell : public Onyx::ECS::ScriptableEntity
    {
    private:
        friend class QuridorState;
        friend class Player;
        friend struct WallPlaceMode;

        glm::vec3 color = {};
        CellWalls walls = {};
        bool isOccupied = false;
        glm::vec3 pawnColor = {};
        BoardPos pos;

    public:
        Cell() = default;
        Cell(Onyx::ECS::Entity entity, const glm::vec3& color, BoardPos pos) :
            Onyx::ECS::ScriptableEntity(entity),
            color(color),
            pos(pos) {}

        inline const bool isCellOccupied() const { return isOccupied; }
        inline const CellWalls getWallState() const { return walls; }
        inline void Outline(bool draw) { GetComponent<Onyx::ECS::ShaderComponent>().drawOutline = draw; }
    };
}
