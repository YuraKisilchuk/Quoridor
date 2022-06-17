#pragma once

#include <Onyx/Onyx.h>

namespace quridor
{
    enum class Direction
    {
        NORTH,
        SOUTH,
        WEST,
        EAST
    };

    struct BoardPos
    {
        int x = 0, y = 0;

        BoardPos() = default;
        BoardPos(int x, int y) : x(x), y(y) {}

        bool operator==(const BoardPos& other) const { return x == other.x && y == other.y; }
        bool operator!=(const BoardPos& other) const { return x != other.x && y != other.y; }
    };

    struct QuridorEvent_Move {};
    struct QuridorEvent_WallPlaceMode
    {
        bool flag;
    };
    struct QuridorEvent_WallFirstCell
    {
        std::string cellTag;
    };
    struct QuridorEvent_WallSecondCell
    {
        std::string cellTag;
    };
    struct QuridorEvent_WallThirdCell
    {
        std::string cellTag;
    };
    struct QuridorEvent_StepAi
    {
        bool flag;
    };
}
