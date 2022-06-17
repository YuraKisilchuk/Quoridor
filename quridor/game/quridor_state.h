#pragma once

#include <Onyx/Onyx.h>

#include "quridor/game/object/cell.h"
#include "quridor/game/object/player.h"
#include "wallPlaceMode.h"
#include "transition.h"

namespace quridor
{
    class QuridorState
    {
    private:
        friend class QuridorListener;
        friend class Quridor;

        Onyx::dumb_ptr<Onyx::Scene::OnyxScene>     activeScene;
        Onyx::dumb_ptr<Onyx::ECS::ECSManager>      ecsManager;
        Onyx::dumb_ptr<Onyx::Events::EventManager> eventManager;

        static std::vector<std::vector<Cell>> board;

        std::vector<std::unique_ptr<Player>> players;
        static std::vector<Wall> walls;
        Onyx::dumb_ptr<Player> currentPlayer;

        Onyx::ECS::Entity floor;

        Transition currentTransition;

        int maxPlayers = 4;
        int maxWalls   = 20;

        int playerCounter = 0;

        bool isGameWon   = false;
        bool winGameFlag = false;

        Onyx::ECS::Entity selectedCellEntity;

        bool cycleFlag      = false;
        bool moveFlag       = false;
        bool checkMovesFlag = true;
        bool wallPlaced     = false;
        bool wallModeFlag   = true;

        bool withAi = false;
        bool stepBot = false;

        WallPlaceMode wallMode;

    public:
        QuridorState() = default;
        QuridorState(const Onyx::dumb_ptr<Onyx::Scene::OnyxScene>& activeScene,
            const Onyx::dumb_ptr<Onyx::ECS::ECSManager>& ecsManager,
            const Onyx::dumb_ptr<Onyx::Events::EventManager>& eventManager, int numberOfPlayers, bool withAi);

        void OnCreate();
        void OnUpdate(double deltaTime);
        void OnDispose();
        void Step(double deltaTime);

    private:
        void CreateCells();
        void WaitForMove();
        Onyx::dumb_ptr<Player> CyclePlayer();
        Cell& GetCellByTag(std::string_view tag);
        bool CheckWinCondition();
        inline void CellOutlines(bool draw);
    };
}
