#pragma once

#include <Onyx/Onyx.h>

#include "quridor/game/common.h"
#include "pawn.h"

namespace quridor
{
    class Cell;

    class Player
    {
    private:
        friend class QuridorState;
        Pawn pawn = {};
        bool isAi = false;
        std::string name = {};
        int wallAmount = 0;
        Direction winDirection = Direction::NORTH;
        bool isWinner = false;

    public:
        Player() = default;
        Player(std::string_view name, Pawn pawn, int wallAmount, Direction winDirection, bool isAi) :
            name(name),
            pawn(pawn),
            wallAmount(wallAmount),
            winDirection(winDirection),
            isAi(isAi) {}

        void SetBoardPosByCell(Cell& cell);
        void CheckMoves(std::vector<std::vector<Cell>>& board);
        bool CheckWin(std::vector<std::vector<Cell>>& board);

        inline const std::string Name() const { return name; }
        inline void Outline(bool draw) { pawn.GetComponent<Onyx::ECS::ShaderComponent>().drawOutline = draw; }
    };
}
