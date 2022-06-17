#pragma once

#include <Onyx/Onyx.h>

#include "quridor/game/common.h"

namespace quridor
{
    class Quridor;

    class QuridorListener : public Onyx::Core::AppLayerListener
    {
    private:
        using quridor_ptr = Onyx::dumb_ptr<Quridor>;
        using eventManager_ptr = Onyx::dumb_ptr<Onyx::Events::EventManager>;
        
    private:
        quridor_ptr quridor;
        eventManager_ptr eventManager;

        //bool flag = true;

    public:
        QuridorListener() = default;
        QuridorListener(const quridor_ptr& quridor) : quridor(quridor) {}

        void SetupEvents() override;

    private:
        void OnKeyPressed(const Onyx::Events::KeyEvent_Pressed& key_event) override;
        void OnMouseMove(const Onyx::Events::MouseEvent_Moved& mouse_event) override;
        void OnMouseBtnPressed(const Onyx::Events::MouseEvent_Pressed& mouse_event) override;
        void OnMouseScrolled(const Onyx::Events::MouseEvent_Scrolled& mouse_event) override {}
        void OnWindowResized(const Onyx::Events::WindowEvent_Resized& window_event) override;
        void OnViewportResize(const Onyx::Events::FramebufferEvent_Resized& framebuffer_event) override {}
        void OnControlBlocked(const Onyx::Events::AppEvent_BlockControls& app_event) override {}
        void OnRayCallback(const Onyx::Events::PhysicsEvent_RayCallback& physics_event) override;

        void OnPlayerMove(const QuridorEvent_Move& quridor_event);
        void OnWallPlace(const QuridorEvent_WallPlaceMode& quridor_event);
        void OnFirstCellPlaced(const QuridorEvent_WallFirstCell& quridor_event);
        void OnSecondCellPlaced(const QuridorEvent_WallSecondCell& quridor_event);
        void OnThirdCellPlaced(const QuridorEvent_WallThirdCell& quridor_event);
        void OnStepBot(const QuridorEvent_StepAi& quridor_event);
    };
}
