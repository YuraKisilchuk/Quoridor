#pragma once

#include <Onyx/Onyx.h>

#include "quridor/game/common.h"

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

    struct BotHook
    {
        BoardPos parentPos = {};
        bool visited = false;
        int distance = 0;
    };

    class Cell : public Onyx::ECS::ScriptableEntity
    {
    private:
        friend class  QuridorState;
        friend class  Player;
        friend struct WallPlaceMode;
        friend class  QuridorBot;

        glm::vec3 color = {};
        CellWalls walls = {};
        bool isOccupied = false;
        glm::vec3 pawnColor = {};
        BoardPos pos = { 0, 0 };
        BotHook botHook = {};

    public:
        Cell() = default;
        Cell(Onyx::ECS::Entity entity, const glm::vec3& color, BoardPos pos) :
            Onyx::ECS::ScriptableEntity(entity),
            color(color),
            pos(pos) {}

        std::vector<Cell> GetNeighbors(std::vector<std::vector<Cell>>& board) const;

        inline const bool isCellOccupied() const { return isOccupied; }
        const bool isCellWalled() const;
        inline const int getBotHookDistance() const { return botHook.distance; }
        inline const CellWalls getWallState() const { return walls; }
        inline void Outline(bool draw) { GetComponent<Onyx::ECS::ShaderComponent>().drawOutline = draw; }
    };

    class CellComparator
    {
    public:
        bool operator()(const Cell& cell1, const Cell& cell2)
        {
            return cell1.getBotHookDistance() < cell2.getBotHookDistance();
        }
    };
}
