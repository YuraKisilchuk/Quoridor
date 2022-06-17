#include "cell.h"

namespace quridor
{
    const bool Cell::isCellWalled() const
    {
        using enum WalledState;
        if (walls.north == WALLED) return true;
        if (walls.south == WALLED) return true;
        if (walls.east  == WALLED) return true;
        if (walls.west  == WALLED) return true;
        return false;
    }

    std::vector<Cell> Cell::GetNeighbors(std::vector<std::vector<Cell>>& board) const
    {
        /// 4 - way.
        std::vector<Cell> neighbors = {};
        neighbors.reserve(4);

        using enum WalledState;
        /// North.
        if (pos.y - 1 >= 0)
        {
            auto& cell = board.at(pos.x).at(pos.y - 1);
            if (cell.getWallState().south == FREE)
                neighbors.push_back(cell);
        }
        /// South.
        if (pos.y + 1 <= board.size() - 1)
        {
            auto& cell = board.at(pos.x).at(pos.y + 1);
            if (cell.getWallState().north == FREE)
                neighbors.push_back(cell);
        }
        /// West.
        if (pos.x - 1 >= 0)
        {
            auto& cell = board.at(pos.x - 1).at(pos.y);
            if (cell.getWallState().east == FREE)
                neighbors.push_back(cell);
        }
        /// East.
        if (pos.x + 1 <= board.at(0).size() - 1)
        {
            auto& cell = board.at(pos.x + 1).at(pos.y);
            if (cell.getWallState().west == FREE)
                neighbors.push_back(cell);
        }

        return neighbors;
    }
}
