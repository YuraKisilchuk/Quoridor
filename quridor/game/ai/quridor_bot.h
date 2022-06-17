#pragma once

#include <limits>

#include "quridor/game/object/cell.h"

namespace quridor
{
    enum class Algorithm
    {
        DIJKSTRA,
        A_STAR
    };

    struct CellRep
    {
        int distance = 0;
        bool visited = false;
    };

    struct CellPriority
    {
        Cell cell = {};
        int priority = std::numeric_limits<int>::max();
    };

    class custom_priority_queue
    {
    private:
        std::vector<CellPriority> cells;

    public:
        custom_priority_queue() = default;

        inline const size_t size() const { return cells.size(); }
        inline const bool empty() const  { return cells.empty(); }
        void emplace(const Cell& cell, int priority)
        {
            cells.emplace(cells.begin() + priority, cell);
            sort();
        }

        void push(const CellPriority& cell)
        {
            cells.push_back(cell);
            sort();
        }

        CellPriority pop()
        {
            CellPriority cell = cells.front();
            const auto res = std::find_if(cells.begin(), cells.end(), [&cell](CellPriority& cellPr)
            {
                return cellPr.cell.GetComponent<Onyx::ECS::TagComponent>().Tag ==
                    cell.cell.GetComponent<Onyx::ECS::TagComponent>().Tag;
            });
            cells.erase(res);
            sort();
            return cell;
        }

        bool has(Cell& cell)
        {
            const auto res = std::find_if(cells.begin(), cells.end(), [&cell](CellPriority& cellPr)
            {
                return cellPr.cell.GetComponent<Onyx::ECS::TagComponent>().Tag ==
                    cell.GetComponent<Onyx::ECS::TagComponent>().Tag;
            });

            return res != cells.end();
        }

        void setPriority(Cell& cell, int priority)
        {
            for (auto& cellPr : cells)
            {
                if (cellPr.cell.GetComponent<Onyx::ECS::TagComponent>().Tag ==
                    cell.GetComponent<Onyx::ECS::TagComponent>().Tag)
                {
                    cellPr.priority = priority;
                }
            }
            sort();
        }

    private:
        void sort()
        {
            std::sort(cells.begin(), cells.end(), [](const CellPriority& cellPr_1, const CellPriority& cellPr_2)
            {
                return cellPr_1.priority > cellPr_2.priority;
            });
        }
    };

    class QuridorBot
    {
    private:
        Algorithm algorithm = Algorithm::DIJKSTRA;
        Cell start = {};
        Cell end   = {};
        std::vector<BoardPos> path = {};

    public:
        QuridorBot(const Cell& start, const Cell& end) : start(start), end(end) {}

        std::vector<Cell> FindShortestPath(std::vector<std::vector<Cell>>& board);

    private:
        std::vector<Cell> Dijkstra(std::vector<std::vector<Cell>>& board);
        //BoardPos MinimumDistance(const std::map<BoardPos, CellRep>& distances);
    };
}
