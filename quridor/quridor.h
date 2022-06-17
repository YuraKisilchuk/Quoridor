#pragma once

#include <Onyx/Onyx.h>

#include "quridor_listener.h"
#include "quridor/game/quridor_state.h"

namespace quridor
{
    struct QuridorCamera
    {
        Onyx::dumb_ptr<Onyx::Scene::EditorCamera> camera;
        Onyx::dumb_ptr<Onyx::Scene::EditorCameraController> controller;
        Onyx::ECS::Entity entity;
    };

    class Quridor : public Onyx::Core::AppLayer
    {
    private:
        friend class QuridorListener;

        std::unique_ptr<Onyx::Scene::OnyxScene> activeScene;
        std::unique_ptr<Onyx::ECS::ECSManager> ecsManager;

        QuridorListener quridorListener;
        QuridorCamera quridorCamera;

        QuridorState quridorState;

        Onyx::ECS::Entity selectedEntity;

        Onyx::Graphics::ScreenCoord2d mouseCoords = {};

    public:
        Quridor() : AppLayer("Quridor") {}

        static bool running;

        void OnInit() override;
        void OnAttach() override;
        void OnDetach() override;
        bool OnUpdate(const double deltaTime) override;
        void OnRender(const double deltaTime) override;
        void OnFixedUpdate(const double deltaTime) override;

        void StartFrame() override;
        void EndFrame() override;

        void DrawGui() override {};

    private:
        void SetupCamera();
        void OnSetupEvents() override;
        void SetupScene();

        void HandleInput(const double deltaTime, Onyx::Scene::EditorCameraController& controller,
            Onyx::Scene::EditorCamera& camera);
        void InitRepeat(const int numberOfPlayers);
        void InitWithAi(int numberOfPlayers);
    };
}
