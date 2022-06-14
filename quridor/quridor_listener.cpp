#include "quridor_listener.h"

#include "quridor.h"

using namespace Onyx;

namespace quridor
{
    void QuridorListener::SetupEvents()
    {
        using namespace Onyx::Events;

        eventManager = quridor->eventManager;
        eventManager->Sink<KeyEvent_Pressed>()        .connect<&QuridorListener::OnKeyPressed>(this);
        eventManager->Sink<MouseEvent_Pressed>()      .connect<&QuridorListener::OnMouseBtnPressed>(this);
        eventManager->Sink<MouseEvent_Moved>()        .connect<&QuridorListener::OnMouseMove>(this);
        eventManager->Sink<PhysicsEvent_RayCallback>().connect<&QuridorListener::OnRayCallback>(this);
        eventManager->Sink<FramebufferEvent_Resized>().connect<&QuridorListener::OnViewportResize>(this);
        eventManager->Sink<WindowEvent_Resized>()     .connect<&QuridorListener::OnWindowResized>(this);

        eventManager->Sink<QuridorEvent_Move>()          .connect<&QuridorListener::OnPlayerMove>(this);
        eventManager->Sink<QuridorEvent_WallPlaceMode>() .connect<&QuridorListener::OnWallPlace>(this);
        eventManager->Sink<QuridorEvent_WallFirstCell>() .connect<&QuridorListener::OnFirstCellPlaced>(this);
        eventManager->Sink<QuridorEvent_WallSecondCell>().connect<&QuridorListener::OnSecondCellPlaced>(this);
        eventManager->Sink<QuridorEvent_WallThirdCell>() .connect<&QuridorListener::OnThirdCellPlaced>(this);
    }

    void QuridorListener::OnPlayerMove(const QuridorEvent_Move& qEv)
    {
        quridor->quridorState.moveFlag = true;
    }

    void QuridorListener::OnWallPlace(const QuridorEvent_WallPlaceMode& qEv)
    {
        quridor->quridorState.wallMode.modeOn = qEv.flag;
        quridor->quridorState.ClearCellOutlines();
        auto& board = quridor->quridorState.board;
        for (size_t i = 1; i < board.size() - 1; ++i)
        {
            auto& cellRow = board.at(i);
            for (size_t j = 1; j < cellRow.size() - 1; ++j)
            {
                cellRow.at(j).GetComponent<ECS::ShaderComponent>().drawOutline = qEv.flag;
            }
        }
    }

    void QuridorListener::OnFirstCellPlaced(const QuridorEvent_WallFirstCell& qEv)
    {
        auto& wallMode = quridor->quridorState.wallMode;
        wallMode.firstCell = quridor->quridorState.GetCellByTag(qEv.cellTag);
        wallMode.firstCellSelected = true;
    }

    void QuridorListener::OnSecondCellPlaced(const QuridorEvent_WallSecondCell& qEv)
    {
        for (auto& cellRow : quridor->quridorState.board)
        {
            for (auto& cell : cellRow)
            {
                cell.GetComponent<ECS::ShaderComponent>().drawOutline = false;
            }
        }
        auto& wallMode = quridor->quridorState.wallMode;
        wallMode.secondCell = quridor->quridorState.GetCellByTag(qEv.cellTag);
        wallMode.secondCellSelected = true;
    }

    void QuridorListener::OnThirdCellPlaced(const QuridorEvent_WallThirdCell& qEv)
    {
        auto& wallMode = quridor->quridorState.wallMode;
        wallMode.thirdCell = quridor->quridorState.GetCellByTag(qEv.cellTag);
        wallMode.thirdCellSelected = true;
    }

    void QuridorListener::OnKeyPressed(const Events::KeyEvent_Pressed& kEv)
    {
        auto& eventManager = quridor->eventManager;

        const int& numberOfPlayers = quridor->quridorState.players.size();

        using enum Core::Key;
        switch (kEv.keyCode)
        {
        case Escape:
            eventManager->EnqueueEvent<Events::WindowEvent_Closed>();
            break;
        case R:
        {
            if (quridor->quridorState.wallModeFlag)
            {
                eventManager->TriggerEvent<QuridorEvent_WallPlaceMode>(true);
                quridor->quridorState.wallModeFlag = false;
            }
            else
            {
                eventManager->TriggerEvent<QuridorEvent_WallPlaceMode>(false);
                quridor->quridorState.wallMode.ResetWallMode();
                quridor->quridorState.currentPlayer->CheckMoves(quridor->quridorState.board);
                quridor->quridorState.wallModeFlag = true;
            }
            break;
        }
        case N:  quridor->InitRepeat(numberOfPlayers); break;
        case F1: quridor->InitRepeat(1);               break;
        case F2: quridor->InitRepeat(2);               break;
        case F3: quridor->InitRepeat(3);               break;
        case F4: quridor->InitRepeat(4);               break;
        }
    }

    void QuridorListener::OnMouseMove(const Events::MouseEvent_Moved& mEv)
    {
        quridor->mouseCoords = { mEv.posX, mEv.posY };
    }

    void QuridorListener::OnMouseBtnPressed(const Events::MouseEvent_Pressed& mEv)
    {
        if (mEv.mouseCode != Core::Mouse::Button0) return;

        const auto& mouseCoords = quridor->mouseCoords;
        const Physics::Ray ray = quridor->quridorCamera.camera->CreateRay(mouseCoords.x, mouseCoords.y);
        quridor->bullet3Physics->RayTest(ray);
    }

    void QuridorListener::OnRayCallback(const Events::PhysicsEvent_RayCallback& pEv)
    {
        const auto& rayCallback = pEv.callback;
        const bool isEntitySelected = quridor->selectedEntity.GetEntityId() != entt::null;

        if (rayCallback.hasHit())
        {
            entt::entity entityId = quridor->bullet3Physics->GetEntityFromRayCast(rayCallback);
            ECS::Entity entity(entityId, quridor->ecsManager.get());
            std::string_view tag = entity.GetComponent<ECS::TagComponent>().Tag;
            if (Util::ContainsString(tag, "pawn")) return;
            else if (Util::ContainsString(tag, "cell"))
            {
                if (entity.GetComponent<ECS::ShaderComponent>().drawOutline)
                {
                    if (quridor->quridorState.wallMode.modeOn)
                    {
                        if (!quridor->quridorState.wallMode.firstCellSelected)
                        {
                            eventManager->TriggerEvent<QuridorEvent_WallFirstCell>(tag.data());
                            return;
                        }
                        if (!quridor->quridorState.wallMode.secondCellSelected)
                        {
                            eventManager->TriggerEvent<QuridorEvent_WallSecondCell>(tag.data());
                            return;
                        }
                        if (!quridor->quridorState.wallMode.thirdCellSelected)
                        {
                            eventManager->TriggerEvent<QuridorEvent_WallThirdCell>(tag.data());
                            return;
                        }
                    }
                    quridor->selectedEntity = entity;
                    quridor->quridorState.selectedCellEntity = entity;
                    eventManager->TriggerEvent<QuridorEvent_Move>();
                    return;
                }
            }
        }
    }

    void QuridorListener::OnWindowResized(const Events::WindowEvent_Resized& wEv)
    {
        quridor->quridorCamera.camera->ResizeCamera(wEv.width, wEv.height);
        eventManager->TriggerEvent<Events::FramebufferEvent_Resized>(wEv.width, wEv.height);
    }
}
