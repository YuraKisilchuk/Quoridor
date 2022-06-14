#pragma once

#include <Onyx/Onyx.h>

#include "cell.h"
#include "player.h"
#include "wallPlaceMode.h"

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

        int maxPlayers = 4;
        int maxWalls   = 20;

        int playerCounter = 0;

        bool isGameWon   = false;
        bool winGameFlag = false;

        Onyx::ECS::Entity selectedCellEntity;

        bool moveFlag       = false;
        bool checkMovesFlag = true;
        bool wallPlaced     = false;
        bool wallModeFlag   = true;

        WallPlaceMode wallMode;

    public:
        QuridorState() = default;
        QuridorState(const Onyx::dumb_ptr<Onyx::Scene::OnyxScene>& activeScene,
            const Onyx::dumb_ptr<Onyx::ECS::ECSManager>& ecsManager,
            const Onyx::dumb_ptr<Onyx::Events::EventManager>& eventManager, int numberOfPlayers);

        void OnCreate();
        void OnUpdate();
        void OnDispose();

    private:
        void CreateCells();
        bool WaitForMove();
        Onyx::dumb_ptr<Player> CyclePlayer();
        Cell& GetCellByTag(std::string_view tag);
        bool CheckWinCondition();
        inline void ClearCellOutlines();
    };
}
