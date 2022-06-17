#include "wallPlaceMode.h"

#include <Onyx/Onyx.h>

#include "quridor/game/object/cell.h"
#include "quridor/game/object/wall.h"

using namespace Onyx;

namespace quridor
{
    void WallPlaceMode::ResetWallMode()
    {
        modeOn     = false;
        wallPlaced = false;
        firstCellSelected  = false;
        secondCellSelected = false;
        thirdCellSelected  = false;
        wallOrientation = WallOrientation::NONE;
        firstCell  = Cell();
        secondCell = Cell();
        thirdCell  = Cell();
        forthCell  = Cell();
    }

    void WallPlaceMode::CalculateFirstStage(std::vector<std::vector<Cell>>& board)
    {
        for (auto& cellRow : board)
        {
            for (auto& cell : cellRow)
            {
                cell.GetComponent<ECS::ShaderComponent>().drawOutline = false;
            }
        }

        const BoardPos& firstCellPos = firstCell.pos;
        using enum WalledState;
        {/// North cell.
            if (firstCellPos.y - 1 >= 0)
            {
                Cell& cell = board.at(firstCellPos.x).at(firstCellPos.y - 1);

                if (cell.getWallState().north == FREE)
                    cell.Outline(true);
                else
                    cell.Outline(false);
            }
        }
        {/// South cell.
            if (firstCellPos.y + 1 <= board.size() - 1)
            {
                Cell& cell = board.at(firstCellPos.x).at(firstCellPos.y + 1);

                if (cell.getWallState().south == FREE)
                    cell.Outline(true);
                else
                    cell.Outline(false);
            }
        }
        {/// West cell.
            if (firstCellPos.x - 1 >= 0)
            {
                Cell& cell = board.at(firstCellPos.x - 1).at(firstCellPos.y);

                if (cell.getWallState().west == FREE)
                    cell.Outline(true);
                else
                    cell.Outline(false);
            }
        }
        {/// East cell.
            if (firstCellPos.x + 1 <= board.at(0).size() - 1)
            {
                Cell& cell = board.at(firstCellPos.x + 1).at(firstCellPos.y);

                if (cell.getWallState().east == FREE)
                    cell.Outline(true);
                else
                    cell.Outline(false);
            }
        }
    }

    void WallPlaceMode::CalculateSecondStage(std::vector<std::vector<Cell>>& board)
    {
        for (auto& cellRow : board)
        {
            for (auto& cell : cellRow)
            {
                cell.GetComponent<ECS::ShaderComponent>().drawOutline = false;
            }
        }

        const BoardPos& firstCellPos  = firstCell.pos;
        const BoardPos& secondCellPos = secondCell.pos;
        
        using enum WalledState;
        using enum Direction;
        using enum WallOrientation;
        {/// North cell relative to second cell.
            if (secondCellPos.y - 1 >= 0)
            {
                Cell& cell = board.at(secondCellPos.x).at(secondCellPos.y - 1);

                if (cell.pos != firstCellPos && CheckPlacement(firstCell, SOUTH))
                {
                    board.at(secondCellPos.x).at(secondCellPos.y + 1).GetComponent<ECS::ShaderComponent>()
                        .drawOutline = true;
                    board.at(secondCellPos.x).at(secondCellPos.y - 1).GetComponent<ECS::ShaderComponent>()
                        .drawOutline = true;
                    wallOrientation = HORIZONTAL;
                    return;
                }
            }
        }
        {/// South cell relative to second cell.
            if (secondCellPos.y + 1 <= board.size() - 1)
            {
                Cell& cell = board.at(secondCellPos.x).at(secondCellPos.y + 1);

                if (cell.pos != firstCellPos && CheckPlacement(firstCell, NORTH))
                {
                    board.at(secondCellPos.x).at(secondCellPos.y + 1).GetComponent<ECS::ShaderComponent>()
                        .drawOutline = true;
                    board.at(secondCellPos.x).at(secondCellPos.y - 1).GetComponent<ECS::ShaderComponent>()
                        .drawOutline = true;
                    wallOrientation = HORIZONTAL;
                    return;
                }
            }
        }
        {/// West cell relative to second cell.
            if (secondCellPos.x - 1 >= 0)
            {
                Cell& cell = board.at(secondCellPos.x - 1).at(secondCellPos.y);

                if (cell.pos != firstCellPos && CheckPlacement(firstCell, EAST))
                {
                    board.at(secondCellPos.x + 1).at(secondCellPos.y).GetComponent<ECS::ShaderComponent>()
                        .drawOutline = true;
                    board.at(secondCellPos.x - 1).at(secondCellPos.y).GetComponent<ECS::ShaderComponent>()
                        .drawOutline = true;
                    wallOrientation = VERTICAL;
                    return;
                }
            }
        }
        {/// East cell relative to second cell.
            if (secondCellPos.x + 1 <= board.size() - 1)
            {
                Cell& cell = board.at(secondCellPos.x + 1).at(secondCellPos.y);

                if (cell.pos != firstCellPos && CheckPlacement(firstCell, WEST))
                {
                    board.at(secondCellPos.x + 1).at(secondCellPos.y).GetComponent<ECS::ShaderComponent>()
                        .drawOutline = true;
                    board.at(secondCellPos.x - 1).at(secondCellPos.y).GetComponent<ECS::ShaderComponent>()
                        .drawOutline = true;
                    wallOrientation = VERTICAL;
                    return;
                }
            }
        }
    }

    std::optional<Wall> WallPlaceMode::CalculateThirdStage(std::vector<std::vector<Cell>>& board)
    {
        if (!SetCellBounds(board)) return std::nullopt;

        board.at(firstCell.pos.x) .at(firstCell.pos.y).walls   = firstCell.walls;
        board.at(secondCell.pos.x).at(secondCell.pos.y).walls  = secondCell.walls;
        board.at(thirdCell.pos.x) .at(thirdCell.pos.y).walls   = thirdCell.walls;
        board.at(forthCell.pos.x) .at(forthCell.pos.y).walls   = forthCell.walls;

        const glm::vec3& firstCellPos = firstCell.GetComponent<ECS::TransformComponent>().Position;
        const glm::vec3& thirdCellPos = thirdCell.GetComponent<ECS::TransformComponent>().Position;

        glm::vec3 averagePos = {
            (firstCellPos.x + thirdCellPos.x) / 2,
            1.f,
            (firstCellPos.z + thirdCellPos.z) / 2
        };

        auto wallEntity = ecsManager->CreateEntity("wall");
        {
            wallEntity.GetComponent<ECS::PrefabComponent>().prefabMeshType = ECS::PrefabMeshType::STATIC_CUBE;
            auto& properties = wallEntity.AddComponent<ECS::ShaderComponent>(Graphics::ShaderType::PHONG1).shader
                .properties;
            properties.diffuse = { 0, 1, 0 };
            properties.specular = glm::vec3(0.2f);
            auto& meshComp = wallEntity.AddComponent<ECS::MeshComponent>();
            auto& transformComp = wallEntity.GetComponent<ECS::TransformComponent>();
            transformComp.Position = averagePos;
            transformComp.Scale = { 1.65f, 1.f, 0.15f };
            transformComp.Rotation.y = wallOrientation == WallOrientation::VERTICAL 
                ? glm::radians(90.f) 
                : glm::radians(0.f);           
            activeScene->AddSceneEntity(wallEntity);
            Scene::ModelLoader::LoadModel_ByPath(fmt::format("{}{}", "assets", "/static_cube.obj"), meshComp, true);
        }

        return Wall(wallEntity);
    }

    bool WallPlaceMode::CheckPlacement(Cell& first, Direction direction)
    {
        BoardPos check;
        bool flag = false;
        using enum Direction;
        switch (direction)
        {
        case NORTH:
            check = first.pos;
            check.y += 2;
            flag = check != first.pos ? false : true;
            break;
        case SOUTH:
            check = first.pos;
            check.y -= 2;
            flag = check != first.pos ? false : true;
            break;
        case WEST:
            check = first.pos;
            check.x += 2;
            flag = check != first.pos ? false : true;
            break;
        case EAST:
            check = first.pos;
            check.x -= 2;
            flag = check != first.pos ? false : true;
            break;
        }

        return flag;
    }

    bool WallPlaceMode::SetCellBounds(std::vector<std::vector<Cell>>& board)
    {
        const BoardPos& firstCellPos  = firstCell.pos;
        const BoardPos& secondCellPos = secondCell.pos;
        const BoardPos& thirdCellPos  = thirdCell.pos;

        forthCell = CalculateForthCell(board);

        bool firstFlag  = true;
        bool secondFlag = true;
        bool thirdFlag  = true;
        bool forthFlag  = true;

        bool finalFlag = false;

        using enum WalledState;
        using enum WallOrientation;
        {/// North/South cell relative to second cell.
            if (secondCellPos.y - 1 >= 0 && secondCellPos.y + 1 <= board.size() - 1)
            {
                BoardPos secondTempPos = secondCellPos;
                secondTempPos.y -= 1;
                if (secondTempPos == thirdCellPos && wallOrientation == HORIZONTAL)
                {
                    if (firstCell.walls.north == FREE)
                        firstCell.walls.north = WALLED;
                    else firstFlag = false;

                    if (secondCell.walls.north == FREE)
                        secondCell.walls.north = WALLED;
                    else secondFlag = false;

                    if (thirdCell.walls.south == FREE)
                        thirdCell.walls.south = WALLED;
                    else thirdFlag = false;

                    if (forthCell.walls.south == FREE)
                        forthCell.walls.south = WALLED;
                    else forthFlag = false;

                    finalFlag = firstFlag && secondFlag && thirdFlag && forthFlag;
                }
                else if (wallOrientation == HORIZONTAL)
                {
                    if (firstCell.walls.south == FREE)
                        firstCell.walls.south = WALLED;
                    else firstFlag = false;

                    if (secondCell.walls.south == FREE)
                        secondCell.walls.south = WALLED;
                    else secondFlag = false;

                    if (thirdCell.walls.north == FREE)
                        thirdCell.walls.north = WALLED;
                    else thirdFlag = false;

                    if (forthCell.walls.north == FREE)
                        forthCell.walls.north = WALLED;
                    else forthFlag = false;

                    finalFlag = firstFlag && secondFlag && thirdFlag && forthFlag;
                }
            }
        }
        {/// West/East cell relative to second cell.
            if (secondCellPos.x - 1 >= 0 && secondCellPos.x + 1 <= board.size() - 1)
            {
                BoardPos secondTempPos = secondCellPos;
                secondTempPos.x -= 1;
                if (secondTempPos == thirdCellPos && wallOrientation == VERTICAL)
                {
                    if (firstCell.walls.west == FREE)
                        firstCell.walls.west = WALLED;
                    else firstFlag = false;

                    if (secondCell.walls.west == FREE)
                        secondCell.walls.west = WALLED;
                    else secondFlag = false;

                    if (thirdCell.walls.east == FREE)
                        thirdCell.walls.east = WALLED;
                    else thirdFlag = false;

                    if (forthCell.walls.east == FREE)
                        forthCell.walls.east = WALLED;
                    else forthFlag = false;

                    finalFlag = firstFlag && secondFlag && thirdFlag && forthFlag;
                }
                else if (wallOrientation == VERTICAL)
                {
                    if (firstCell.walls.east == FREE)
                        firstCell.walls.east = WALLED;
                    else firstFlag = false;

                    if (secondCell.walls.east == FREE)
                        secondCell.walls.east = WALLED;
                    else secondFlag = false;

                    if (thirdCell.walls.west == FREE)
                        thirdCell.walls.west = WALLED;
                    else thirdFlag = false;

                    if (forthCell.walls.west == FREE)
                        forthCell.walls.west = WALLED;
                    else forthFlag = false;

                    finalFlag = firstFlag && secondFlag && thirdFlag && forthFlag;
                }
            }
        }

        return finalFlag;
    }

    Cell WallPlaceMode::CalculateForthCell(std::vector<std::vector<Cell>>& board)
    {
        const BoardPos& firstCellPos  = firstCell.pos;
        const BoardPos& secondCellPos = secondCell.pos;
        const BoardPos& thirdCellPos  = thirdCell.pos;

        BoardPos firstTempPos  = firstCellPos;
        BoardPos secondTempPos = secondCellPos;

        Cell toReturn = {};

        {
            firstTempPos.y -= 1;
            firstTempPos.x -= 1;
            if (firstTempPos == thirdCellPos)
            {
                secondTempPos.x -= 1;
                if (secondTempPos == thirdCellPos)
                    toReturn = Cell(board.at(thirdCellPos.x).at(thirdCellPos.y + 1));
                else toReturn = Cell(board.at(thirdCellPos.x + 1).at(thirdCellPos.y));
            }
            else
            {
                firstTempPos  = firstCellPos;
                secondTempPos = secondCellPos;
                firstTempPos.y += 1;
                firstTempPos.x += 1;
                if (firstTempPos == thirdCellPos)
                {
                    secondTempPos.y += 1;
                    if (secondTempPos == thirdCellPos)
                        toReturn = Cell(board.at(thirdCellPos.x - 1).at(thirdCellPos.y));
                    else toReturn = Cell(board.at(thirdCellPos.x).at(thirdCellPos.y - 1));
                }
            }
        }

        {
            firstTempPos  = firstCellPos;
            secondTempPos = secondCellPos;
            firstTempPos.y -= 1;
            firstTempPos.x += 1;
            if (firstTempPos == thirdCellPos)
            {
                secondTempPos.x += 1;
                if (secondTempPos == thirdCellPos)
                    toReturn = Cell(board.at(thirdCellPos.x).at(thirdCellPos.y + 1));
                else toReturn = Cell(board.at(thirdCellPos.x - 1).at(thirdCellPos.y));
            }
            else
            {
                firstTempPos = firstCellPos;
                secondTempPos = secondCellPos;
                firstTempPos.y += 1;
                firstTempPos.x -= 1;
                if (firstTempPos == thirdCellPos)
                {
                    secondTempPos.y -= 1;
                    if (secondTempPos == thirdCellPos)
                        toReturn = Cell(board.at(thirdCellPos.x + 1).at(thirdCellPos.y));
                    else toReturn = Cell(board.at(thirdCellPos.x).at(thirdCellPos.y - 1));
                }
            }
        }

        return toReturn;
    }
}
