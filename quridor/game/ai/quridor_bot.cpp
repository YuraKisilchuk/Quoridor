#pragma optimize("" ,off)

#include "quridor_bot.h"

#include <queue>

using namespace Onyx;

namespace quridor
{
    std::vector<Cell> QuridorBot::FindShortestPath(std::vector<std::vector<Cell>>& board)
    {
        for (auto& cellRow : board)
        {
            for (auto& cell : cellRow)
            {
                cell.botHook.distance = std::numeric_limits<int>::max();
                cell.botHook.visited = false;
                //cell.botHook.parentPos = { -1, -1 };
            }
        }

        std::vector<Cell> path;
        {
            //TimerScope timer = {};

            //int algoCounter = 0;
            while (path.empty())
            {
                path = Dijkstra(board);
                //++algoCounter;
            }

            //timer.Log("Dijkstra time");
            //ONYX_TRACE(fmt::format("Counter: {}", algoCounter));
        }

        //for (const auto& cell : path)
        //{
        //    ONYX_TRACE(fmt::format("pos: {} : {}", cell.pos.x, cell.pos.y));
        //}

        for (auto& cellRow : board)
        {
            for (auto& cell : cellRow)
            {
                cell.botHook.distance = std::numeric_limits<int>::max();
                cell.botHook.visited = false;
                //cell.botHook.parentPos = { -1, -1 };
            }
        }

        return path;
    }

    std::vector<Cell> QuridorBot::Dijkstra(std::vector<std::vector<Cell>>& board)
    {
        std::vector<Cell> path = {};

        custom_priority_queue queue = {};

        for (auto& cellRow : board)
        {
            for (auto& cell : cellRow)
            {
                cell.botHook.distance = std::numeric_limits<int>::max();
                //cell.botHook.visited = false;
                //cell.botHook.parentPos = { -1, -1 };
            }
        }

        auto& startCell = board.at(start.pos.x).at(start.pos.y);

        startCell.botHook.distance = 0;
        queue.push({ startCell, 0 });

        while (!queue.empty())
        {
            CellPriority cellPr = queue.pop();
            Cell& currCell = board.at(cellPr.cell.pos.x).at(cellPr.cell.pos.y);
            currCell.botHook.visited = true;
            for (auto& neighbor : currCell.GetNeighbors(board))
            {
                auto& boardNeighbor = board.at(neighbor.pos.x).at(neighbor.pos.y);
                const int minDistance = Math::min<int>(boardNeighbor.botHook.distance, currCell.botHook.distance + 1);
                
                if (minDistance != boardNeighbor.botHook.distance)
                {
                    boardNeighbor.botHook.distance  = minDistance;
                    boardNeighbor.botHook.parentPos = currCell.pos;

                    if (queue.has(boardNeighbor))
                    {
                        queue.setPriority(boardNeighbor, minDistance);
                    }
                }
                if (!queue.has(boardNeighbor) && !boardNeighbor.botHook.visited)
                {
                    queue.push({ boardNeighbor, boardNeighbor.getBotHookDistance() });
                }
            }
        }

        auto& endCell = board.at(end.pos.x).at(end.pos.y);

        path.push_back(endCell);

        bool done = false;
        while (!done)
        {
            auto& currentCell = board.at(static_cast<size_t>(path[path.size() - 1].pos.x))
                .at(static_cast<size_t>(path[path.size() - 1].pos.y));
            if (currentCell.pos == start.pos)
            {
                done = true;
                break;
            }
            //BoardPos parentPos = currentCell.botHook.parentPos;
            BoardPos parentPos;
            int minDistance = std::numeric_limits<int>::max();

            for (auto& neighbor : currentCell.GetNeighbors(board))
            {
                if (minDistance > neighbor.botHook.distance)
                {
                    minDistance = neighbor.botHook.distance;
                    parentPos = neighbor.pos;
                    board.at(parentPos.x).at(parentPos.y).botHook.distance = minDistance;
                    currentCell.botHook.parentPos = parentPos;
                }
            }

            path.push_back(board.at(parentPos.x).at(parentPos.y));
            if (path.size() > 100)
            {
                path.clear();
                done = true;
                break;
            }
            //assert(path.size() > 100 && "Path size is larger than 100");
        }

        if (path.size() > 2)
            std::reverse(path.begin(), path.end());

        //int distanceCheck = path[0].botHook.distance;
        //for (size_t i = 0; i < path.size(); ++i)
        //{
        //    if (i == path.size() - 1) break;
        //    if (distanceCheck + 1 == path[i + 1].botHook.distance)
        //    {
        //        distanceCheck = path[i + 1].botHook.distance;
        //    }
        //    else
        //    {
        //        distanceCheck = -1;
        //        break;
        //    }
        //}
        //
        //if (distanceCheck == -1)
        //    return {};

        return path;
    }

    //BoardPos QuridorBot::MinimumDistance(const std::map<BoardPos, CellRep>& distances)
    //{
    //    int min = std::numeric_limits<int>::max();
    //    BoardPos min_distance_pos = {};

    //    for (const auto& [boardPos, cellRep] : distances)
    //    {
    //        if (cellRep.visited == false && cellRep.distance <= min)
    //        {
    //            min = cellRep.distance;
    //            min_distance_pos = boardPos;
    //        }
    //    }

    //    return min_distance_pos;
    //}
}
