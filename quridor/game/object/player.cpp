#include "player.h"

#include "cell.h"

using namespace Onyx;

namespace quridor
{
    bool Player::CheckWin(std::vector<std::vector<Cell>>& board)
    {
        bool win = false;

        using enum Direction;
        switch (winDirection)
        {
        case NORTH:
        {
            std::vector<Cell> cellVec;
            for (size_t i = 0; i < board.size(); ++i)
            {
                cellVec.push_back(board.at(i).at(0));
            }
            for (size_t i = 0; i < cellVec.size(); ++i)
            {
                auto& cell = cellVec.at(i);
                win = cell.isOccupied && (cell.pos == pawn.pos);
                if (win) break;
            }
            break;
        }
        case SOUTH:
        {
            std::vector<Cell> cellVec;
            for (size_t i = 0; i < board.size(); ++i)
            {
                cellVec.push_back(board.at(i).at(board.size() - 1));
            }
            for (size_t i = 0; i < cellVec.size(); ++i)
            {
                auto& cell = cellVec.at(i);
                win = cell.isOccupied && (cell.pos == pawn.pos);
                if (win) break;
            }
            break;
        }
        case WEST:
        {
            auto& cellVec = board.at(0);
            for (size_t i = 0; i < cellVec.size(); ++i)
            {
                auto& cell = cellVec.at(i);
                win = cell.isOccupied && (cell.pos == pawn.pos);
                if (win) break;
            }
            break;
        }
        case EAST:
        {
            auto& cellVec = board.at(board.size() - 1);
            for (size_t i = 0; i < cellVec.size(); ++i)
            {
                auto& cell = cellVec.at(i);
                win = cell.isOccupied && (cell.pos == pawn.pos);
                if (win) break;
            }
            break;
        }
        }

        return win;
    }

    void Player::SetBoardPosByCell(Cell& cell)
    {
        cell.pawnColor = pawn.GetComponent<ECS::ShaderComponent>().shader.properties.diffuse;
        cell.isOccupied = true;
        pawn.pos = cell.pos;
    }

    void Player::CheckMoves(std::vector<std::vector<Cell>>& board)
    {
        const BoardPos pawnPos = pawn.pos;
        const Cell& playerCell = board.at(pawnPos.x).at(pawnPos.y);

        using enum WalledState;
        {/// North cell.
            if (pawnPos.y - 1 >= 0)
            {
                Cell& cell = board.at(pawnPos.x).at(pawnPos.y - 1);

                if (cell.getWallState().south == FREE && playerCell.getWallState().north == FREE)
                {
                    if (cell.isCellOccupied())
                    {
                        if (board.at(pawnPos.x).at(pawnPos.y - 2).getWallState().south == WALLED
                            && board.at(cell.pos.x).at(cell.pos.y).getWallState().north == WALLED)
                        {
                            if (pawnPos.x + 1 <= board.size())
                                board.at(pawnPos.x + 1).at(pawnPos.y - 1).Outline(true);
                            if (pawnPos.x - 1 >= 0)
                                board.at(pawnPos.x - 1).at(pawnPos.y - 1).Outline(true);
                        }
                        else if (!board.at(pawnPos.x).at(pawnPos.y - 2).isCellOccupied() && pawnPos.y - 2 >= 0)
                            board.at(pawnPos.x).at(pawnPos.y - 2).Outline(true);
                    }
                    else cell.Outline(true);
                }
                else cell.Outline(false);
            }
        }
        {/// South cell.
            if (pawnPos.y + 1 <= board.size() - 1)
            {
                Cell& cell = board.at(pawnPos.x).at(pawnPos.y + 1);

                if (cell.getWallState().north == FREE && playerCell.getWallState().south == FREE)
                {
                    if (cell.isCellOccupied())
                    {
                        if (board.at(pawnPos.x).at(pawnPos.y + 2).getWallState().north == WALLED
                            && board.at(cell.pos.x).at(cell.pos.y).getWallState().south == WALLED)
                        {
                            if (pawnPos.x + 1 <= board.size())
                                board.at(pawnPos.x + 1).at(pawnPos.y + 1).Outline(true);
                            if (pawnPos.x - 1 >= 0)
                                board.at(pawnPos.x - 1).at(pawnPos.y + 1).Outline(true);
                        }
                        else if (!board.at(pawnPos.x).at(pawnPos.y + 2).isCellOccupied() && pawnPos.y + 2 >= 0)
                            board.at(pawnPos.x).at(pawnPos.y + 2).Outline(true);
                    }
                    else cell.Outline(true);
                }
                else cell.Outline(false);
            }
        }
        {/// West cell.
            if (pawnPos.x - 1 >= 0)
            {
                Cell& cell = board.at(pawnPos.x - 1).at(pawnPos.y);

                if (cell.getWallState().east == FREE && playerCell.getWallState().west == FREE)
                {
                    if (cell.isCellOccupied())
                    {
                        if (board.at(pawnPos.x - 2).at(pawnPos.y).getWallState().east == WALLED
                            && board.at(cell.pos.x).at(cell.pos.y).getWallState().west == WALLED)
                        {
                            if (pawnPos.y + 1 <= board.size())
                                board.at(pawnPos.x - 1).at(pawnPos.y + 1).Outline(true);
                            if (pawnPos.y - 1 >= 0)
                                board.at(pawnPos.x - 1).at(pawnPos.y - 1).Outline(true);
                        }
                        else if (!board.at(pawnPos.x - 2).at(pawnPos.y).isCellOccupied() && pawnPos.x - 2 >= 0)
                            board.at(pawnPos.x - 2).at(pawnPos.y).Outline(true);
                    }
                    else cell.Outline(true);
                }
                else cell.Outline(false);
            }
        }
        {/// East cell.
            if (pawnPos.x + 1 <= board.at(0).size() - 1)
            {
                Cell& cell = board.at(pawnPos.x + 1).at(pawnPos.y);

                if (cell.getWallState().west == FREE && playerCell.getWallState().east == FREE)
                {
                    if (cell.isCellOccupied())
                    {
                        if (board.at(pawnPos.x + 2).at(pawnPos.y).getWallState().west == WALLED
                            && board.at(cell.pos.x).at(cell.pos.y).getWallState().east == WALLED)
                        {
                            if (pawnPos.y + 1 <= board.size())
                                board.at(pawnPos.x + 1).at(pawnPos.y + 1).Outline(true);
                            if (pawnPos.y - 1 >= 0)
                                board.at(pawnPos.x + 1).at(pawnPos.y - 1).Outline(true);
                        }
                        else if (!board.at(pawnPos.x + 2).at(pawnPos.y).isCellOccupied() && pawnPos.x + 2 >= 0)
                            board.at(pawnPos.x + 2).at(pawnPos.y).Outline(true);
                    }
                    else cell.Outline(true);
                }
                else cell.Outline(false);
            }
        }
    }
}
