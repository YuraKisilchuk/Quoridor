#pragma once

#include <Onyx/Onyx.h>

#include "quridor/game/common.h"
#include "quridor/game/object/cell.h"

namespace quridor
{
    enum class WallOrientation
    {
        HORIZONTAL,
        VERTICAL,
        NONE
    };

    class Wall;

    struct WallPlaceMode
    {
        bool modeOn             = false;
        bool wallPlaced         = false;
        bool firstCellSelected  = false;
        bool secondCellSelected = false;
        bool thirdCellSelected  = false;

        Cell firstCell;
        Cell secondCell;
        Cell thirdCell;
        Cell forthCell;

        WallOrientation wallOrientation = WallOrientation::NONE;

        Onyx::dumb_ptr<Onyx::ECS::ECSManager> ecsManager;
        Onyx::dumb_ptr<Onyx::Scene::OnyxScene> activeScene;
        
        WallPlaceMode() = default;
        WallPlaceMode(const Onyx::dumb_ptr<Onyx::Scene::OnyxScene>& activeScene,
            const Onyx::dumb_ptr<Onyx::ECS::ECSManager>& ecsManager) :
            ecsManager(ecsManager),
            activeScene(activeScene) {}

        void CalculateFirstStage(std::vector<std::vector<Cell>>& board);
        void CalculateSecondStage(std::vector<std::vector<Cell>>& board);
        std::optional<Wall> CalculateThirdStage(std::vector<std::vector<Cell>>& board);
        void ResetWallMode();

    private:
        bool CheckPlacement(Cell& first, Direction direction);
        bool SetCellBounds(std::vector<std::vector<Cell>>& board);
        Cell CalculateForthCell(std::vector<std::vector<Cell>>& board);
    };
}
