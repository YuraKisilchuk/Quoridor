#include "quridor_state.h"

#include "quridor/game/common.h"
#include "quridor/game/object/wall.h"
#include "quridor/game/ai/quridor_bot.h"

using namespace Onyx;

namespace quridor
{
    std::vector<std::vector<Cell>> QuridorState::board = {};
    std::vector<Wall> QuridorState::walls = {};

    QuridorState::QuridorState(const dumb_ptr<Scene::OnyxScene>& activeScene, const dumb_ptr<ECS::ECSManager>&
        ecsManager, const dumb_ptr<Events::EventManager>& eventManager, int numberOfPlayers, bool withAi) :
        activeScene(activeScene),
        ecsManager(ecsManager),
        eventManager(eventManager),
        wallMode(activeScene, ecsManager),
        withAi(withAi)
    {
        if (numberOfPlayers > maxPlayers)
            numberOfPlayers = maxPlayers;
        players.reserve(numberOfPlayers);

        CreateCells();
        OnCreate();
    }

    dumb_ptr<Player> QuridorState::CyclePlayer()
    {
        if (players.size() == 1)
            return players[playerCounter].get();

        ++playerCounter;
        if (playerCounter >= players.size())
            playerCounter = 0;
        currentPlayer->Outline(false);
        if (players[playerCounter].get()->isAi)
            stepBot = true;
        return players[playerCounter].get();
    }

    Cell& QuridorState::GetCellByTag(std::string_view tag)
    {
        for (auto& cellRow : board)
        {
            for (auto& cell : cellRow)
            {
                if (cell.GetComponent<ECS::TagComponent>().Tag == tag)
                    return cell;
            }
        }
    }

    void QuridorState::OnCreate()
    {
        const int numOfPlayers = players.capacity();
        int wallsPerPlayer;

        glm::vec3 color = { 0, 0, 1 };

        Direction winDirection = Direction::NORTH;

        switch (numOfPlayers)
        {
        case 1: wallsPerPlayer = 2; break;
        case 2: wallsPerPlayer = 10; break;
        case 3: wallsPerPlayer = 6;  break;
        case 4: wallsPerPlayer = 5;  break;
        }   

        for (size_t i = 0; i < numOfPlayers; ++i)
        {
            BoardPos pos = {};
            glm::vec3 transformPos = {};

            switch (i)
            {
            case 0:
                winDirection = Direction::NORTH;
                transformPos = board.at(4).at(8).GetComponent<ECS::TransformComponent>().Position;
                pos = { 4, 8 };
                break;
            case 1:
                winDirection = Direction::SOUTH;
                transformPos = board.at(4).at(0).GetComponent<ECS::TransformComponent>().Position;
                pos   = { 4, 0 };
                color = { 1, 1, 0 };
                break;
            case 2:
                winDirection = Direction::WEST;
                transformPos = board.at(8).at(4).GetComponent<ECS::TransformComponent>().Position;
                pos   = { 8, 4 };
                color = { 1, 0, 0 };
                break;
            case 3:
                winDirection = Direction::EAST;
                transformPos = board.at(0).at(4).GetComponent<ECS::TransformComponent>().Position;
                pos   = { 0, 4 };
                color = { 0, 1, 0 };
                break;
            }

            auto pawnEntity = ecsManager->CreateEntity(fmt::format("pawn{}", i));
            {
                pawnEntity.AddComponent<ECS::ShaderComponent>(Graphics::ShaderType::PHONG1)
                    .shader
                    .properties
                    .diffuse = color;
                pawnEntity.AddComponent<ECS::MeshComponent>();
                pawnEntity.AddComponent<ECS::ImportedModelComponent>("pawn.gltf");
                auto& transformComp = pawnEntity.GetComponent<ECS::TransformComponent>();
                transformComp.Position = { transformPos.x, 1.2f, transformPos.z };
                transformComp.Scale = { 0.75f, 0.75f, 0.75f };
                activeScene->AddSceneEntity(pawnEntity);
            }

            bool isAi = false;
            if (withAi)
                isAi = i > 0;

            auto player = std::make_unique<Player>(fmt::format("player{}", i + 1), Pawn(pawnEntity, color, pos),
                wallsPerPlayer, winDirection, isAi);
            if (i == 0) currentPlayer = player.get();
            board.at(pos.x).at(pos.y).isOccupied = true;
            players.push_back(std::move(player));
        }
    }

    void QuridorState::OnDispose()
    {
        board.clear();
        walls.clear();
    }

    void QuridorState::CreateCells()
    {
        constexpr glm::vec3 cellColor = { 0.25f, 0 , 0.1f };
        constexpr float posY    = 1.2f;
        constexpr float padding = 2.f;
        constexpr float offset  = 8.f;

        constexpr int boardSizeX = 9;
        constexpr int boardSizeY = 9;

        for (size_t i = 0; i < boardSizeX; ++i)
        {
            std::vector<Cell> cellVec = {};
            for (size_t j = 0; j < boardSizeY; ++j)
            {
                auto cell = ecsManager->CreateEntity(fmt::format("cell{}|{}", i, j));
                {
                    cell.GetComponent<ECS::PrefabComponent>().prefabMeshType = ECS::PrefabMeshType::STATIC_CUBE;
                    auto& properties = cell.AddComponent<ECS::ShaderComponent>(Graphics::ShaderType::PHONG1).shader
                        .properties;
                    properties.diffuse = cellColor;
                    properties.specular = glm::vec3(0.2f);
                    cell.AddComponent<ECS::MeshComponent>();
                    auto& transformComp = cell.GetComponent<ECS::TransformComponent>();
                    transformComp.Position = { (i * padding) - offset, posY, (j * padding) - offset };
                    transformComp.Scale = { 0.75f, 0.2f, 0.75f };
                    auto& collisionComp = cell.AddComponent<ECS::BoxColliderComponent>();
                    cell.AddComponent<ECS::Rigidbody3DComponent>(transformComp, collisionComp.boxCollider, 0.f);
                    activeScene->AddSceneEntity(cell);
                }
                cellVec.push_back({ cell, cellColor, { static_cast<int>(i), static_cast<int>(j) } });
            }
            board.push_back(cellVec);
        }
    }

    void QuridorState::WaitForMove()
    {
        if (checkMovesFlag)
        {
            CellOutlines(false);
            currentPlayer->CheckMoves(board);
            checkMovesFlag = false;
        }

        if (wallMode.modeOn && !wallPlaced && currentPlayer->wallAmount > 0)
        {
            static bool firstFlag  = true;
            static bool secondFlag = true;
            static bool thirdFlag  = true;
            if (wallMode.firstCellSelected && firstFlag)
            {
                wallMode.CalculateFirstStage(board);
                firstFlag = false;
            }
            else if (wallMode.secondCellSelected && secondFlag)
            {
                wallMode.CalculateSecondStage(board);
                secondFlag = false;
            }
            else if (wallMode.thirdCellSelected && thirdFlag)
            {
                std::optional<Wall> wall = wallMode.CalculateThirdStage(board);
                if (wall.has_value())
                {
                    --currentPlayer->wallAmount;
                    walls.push_back(wall.value());
                }
                else CellOutlines(false);

                wallMode.wallPlaced = true;
                thirdFlag = false;
            }

            if (wallMode.wallPlaced)
            {
                wallMode.ResetWallMode();
                checkMovesFlag  = true;
                firstFlag       = true;
                secondFlag      = true;
                thirdFlag       = true;
                wallPlaced      = false;
                moveFlag        = false;
                wallMode.modeOn = false;
                wallModeFlag    = true;
                cycleFlag       = true;
                stepBot         = true;
            }
        }

        if (moveFlag)
        {
            {/// Clear occupied cell.
                const BoardPos& pos = currentPlayer->pawn.pos;
                Cell& cell = board.at(pos.x).at(pos.y);
                cell.isOccupied = false;
                cell.pawnColor = {};
            }

            Cell& cell = GetCellByTag(selectedCellEntity.GetComponent<ECS::TagComponent>().Tag);
            
            double distance = std::hypot(cell.pos.x - currentPlayer->pawn.pos.x, cell.pos.y - currentPlayer->pawn.pos.y);
            bool isObstacle = distance > 1.5 ? true : false;

            currentPlayer->SetBoardPosByCell(cell);
            currentTransition = Transition(
                currentPlayer->pawn.entity,
                currentPlayer->pawn.entity.GetComponent<ECS::TransformComponent>().Position,
                cell.entity.GetComponent<ECS::TransformComponent>().Position,
                isObstacle
            );

            CellOutlines(false);
            checkMovesFlag = true;
            moveFlag       = false;
            wallPlaced     = false;
            cycleFlag      = true;  
            stepBot        = true;
        }
    }

    void QuridorState::Step(double deltaTime)
    {
        {/// Clear occupied cell.
            const BoardPos& pos = currentPlayer->pawn.pos;
            Cell& cell = board.at(pos.x).at(pos.y);
            cell.isOccupied = false;
            cell.pawnColor = {};
        }

        BoardPos endPos;
        using enum Direction;
        switch (currentPlayer->winDirection)
        {
        case NORTH:
            endPos.x = currentPlayer->pawn.pos.x;
            endPos.y = 0;
            break;
        case SOUTH:
            endPos.x = currentPlayer->pawn.pos.x;
            endPos.y = board.size() - 1;
            break;
        case WEST:
            endPos.x = 0;
            endPos.y = currentPlayer->pawn.pos.y;
            break;
        case EAST:
            endPos.x = board.size() - 1;
            endPos.y = currentPlayer->pawn.pos.y;
            break;
        }

        Cell& endCell = board.at(endPos.x).at(endPos.y);
        Cell& startCell = board.at(currentPlayer->pawn.pos.x).at(currentPlayer->pawn.pos.y);
        QuridorBot bot(startCell, endCell);
        auto path = bot.FindShortestPath(board);
        Cell& nextCell = board.at(path[1].pos.x).at(path[1].pos.y);

        if (path.size() == 2)
            nextCell = board.at(endCell.pos.x).at(endCell.pos.y);

        //bool isObstacle = nextCell.isCellOccupied();

        //nextCell

        currentPlayer->SetBoardPosByCell(nextCell);
        currentTransition = Transition(
            currentPlayer->pawn.entity,
            currentPlayer->pawn.entity.GetComponent<ECS::TransformComponent>().Position,
            nextCell.entity.GetComponent<ECS::TransformComponent>().Position,
            false
        );

        board.at(nextCell.pos.x).at(nextCell.pos.y).isOccupied = nextCell.isCellOccupied();

        if (currentPlayer->pawn.pos == endCell.pos)
        {
            currentPlayer->isWinner = true;
            stepBot = false;
        }

        CellOutlines(false);
        cycleFlag = true;
    }

    void QuridorState::OnUpdate(double deltaTime)
    {
        if (!isGameWon)
        {
            currentPlayer->Outline(true);

            if (!currentTransition.isFinished)
            {
                checkMovesFlag = false;
                currentTransition.OnUpdate(deltaTime);
            }
            
            if (!currentPlayer->isAi)
                WaitForMove();
            else if (stepBot && currentTransition.isFinished)
            {
                stepBot = false;
                Step(deltaTime);
            }

            if (cycleFlag)
            {
                if (currentTransition.isFinished)
                {
                    if (currentPlayer->isWinner) isGameWon = true;
                    cycleFlag = false;
                    checkMovesFlag = true;
                    isGameWon = CheckWinCondition();
                    if (!isGameWon)
                        currentPlayer = CyclePlayer();
                }
            }    
        }

        if (winGameFlag)
        {
            for (auto& player : players)
            {
                const glm::vec3 color = player->pawn.GetComponent<ECS::ShaderComponent>().shader.properties.diffuse;
                floor.GetComponent<ECS::ShaderComponent>().shader.properties.diffuse = color;
                if (player->isWinner) break;
            }
            for (auto& cellRow : board)
            {
                for (auto& cell : cellRow)
                {
                    cell.GetComponent<ECS::ShaderComponent>().drawOutline = false;
                }
            }
            winGameFlag = false;
        }
    }

    bool QuridorState::CheckWinCondition()
    {
        bool win = false;
        for (auto& player : players)
        {
            win = player->CheckWin(board);
            player->isWinner = win;
            winGameFlag = win;
            if (win) break;
        }

        return win;
    }

    void QuridorState::CellOutlines(bool draw)
    {
        for (auto& cellRow : board)
        {
            for (auto& cell : cellRow)
            {
                cell.GetComponent<ECS::ShaderComponent>().drawOutline = false;
            }
        }
    }
}
