#include "quridor.h"

using namespace Onyx;

namespace quridor
{
    bool Quridor::running = true;

    void Quridor::OnSetupEvents()
    {
        quridorListener.SetupEvents();
    }

    void Quridor::SetupScene()
    {
        activeScene->sceneData.dirLight.dirLightDirection = { -5.f, -10.f, 5.f };

        auto floor = ecsManager->CreateEntity("floor");
        {
            floor.GetComponent<ECS::PrefabComponent>().prefabMeshType = ECS::PrefabMeshType::STATIC_CUBE;
            floor.AddComponent<ECS::ShaderComponent>(Graphics::ShaderType::PHONG1)
                .shader
                .properties
                .diffuse = { 1, 0.75f, 0.25f };
            floor.AddComponent<ECS::MeshComponent>();
            auto& transformComp = floor.GetComponent<ECS::TransformComponent>();
            transformComp.Position = { 0, 0.5f, 0 };
            transformComp.Scale = { 10, 0.5f, 10 };
            activeScene->AddSceneEntity(floor);
        }
        quridorState.floor = floor;
    }

    void Quridor::OnInit()
    {
        TimerScope timer{};
        {
            ecsManager  = std::make_unique<ECS::ECSManager>(eventManager);
            activeScene = std::make_unique<Scene::OnyxScene>(ecsManager.get(), eventManager, metricsCollector,
                bullet3Physics);
            quridorListener = QuridorListener(this);
            quridorState    = QuridorState(activeScene.get(), ecsManager.get(), eventManager, 2);

            SetupCamera();
            SetupScene();

            activeScene->OnInitScene();
        }
        timer.Log("Quridor init.");
    }

    void Quridor::OnAttach()
    {
        running = true;
    }

    void Quridor::OnDetach()
    {
        activeScene->ClearSceneEntities();

        activeScene.reset();
        ecsManager.reset();
    }

    bool Quridor::OnUpdate(const double deltaTime)
    {
        auto& cameraTransform = quridorCamera.entity.GetComponent<ECS::TransformComponent>();
        auto& camera = quridorCamera.camera;
        auto& cameraController = quridorCamera.controller;

        cameraTransform.Rotation = camera->GetDirection();
        cameraTransform.Position = camera->GetPosition();

        {/// Camera constraints.
            /// Camera Y-Axis Constraint.
            if (cameraTransform.Position.y <= 5.f)
            {
                cameraTransform.Position.y = 5.f;
                camera->SetPosition(cameraTransform.Position);
            }
            if (cameraTransform.Position.y >= 12.f)
            {
                cameraTransform.Position.y = 12.f;
                camera->SetPosition(cameraTransform.Position);
            }
            /// Camera X-Axis Constraint.
            if (cameraTransform.Position.x >= 15.f)
            {
                cameraTransform.Position.x = 15.f;
                camera->SetPosition(cameraTransform.Position);
            }
            /// Camera Z-Axis Constraint.
            if (cameraTransform.Position.z >= 15.f)
            {
                cameraTransform.Position.z = 15.f;
                camera->SetPosition(cameraTransform.Position);
            }
        }

        if (cameraController->arcControls) camera->UpdateCameraVarsArc();
        else camera->UpdateCameraVarsFps();

        HandleInput(deltaTime, *cameraController.get(), *camera.get());

        cameraController->MouseHandler(mouseCoords, *camera.get());

        activeScene->OnUpdateRuntime(deltaTime);

        quridorState.OnUpdate();

        return running;
    }

    void Quridor::OnFixedUpdate(const double deltaTime)
    {
        if (activeScene->sceneState.updateRuntime) activeScene->OnUpdatePhysics(deltaTime);
    }

    void Quridor::OnRender(const double deltaTime)
    {
        activeScene->OnRenderRuntime(deltaTime, *quridorCamera.camera);
    }

    void Quridor::StartFrame()
    {
        Graphics::Renderer::StartRender(*quridorCamera.camera.get(), activeScene->sceneState);
    }

    void Quridor::EndFrame()
    {
        Graphics::Renderer::EndRender(*quridorCamera.camera.get());
    }

    void Quridor::HandleInput(const double deltaTime, Scene::EditorCameraController& cameraController,
        Scene::EditorCamera& camera)
    {
        cameraController.mouseMoveFlag = inputManager->CheckMouseButton(Core::Mouse::Button1);

        using enum Core::Key;
        if (inputManager->CheckKey(W)) cameraController.TranslateForward(deltaTime, camera);
        if (inputManager->CheckKey(S)) cameraController.TranslateBackwards(deltaTime, camera);
        if (inputManager->CheckKey(A)) cameraController.TranslateLeft(deltaTime, camera);
        if (inputManager->CheckKey(D)) cameraController.TranslateRight(deltaTime, camera);
        if (inputManager->CheckKey(Q)) cameraController.TranslateDown(deltaTime, camera);
        if (inputManager->CheckKey(E)) cameraController.TranslateUp(deltaTime, camera);
        if (inputManager->CheckKey(LeftShift))
        {
            if (inputManager->CheckKey(Space)) cameraController.SetCameraSpeed(10.f);
            else cameraController.SetCameraSpeed(5.f);
        }
        else cameraController.SetCameraSpeed(1.f);
    }

    void Quridor::InitRepeat(const int numberOfPlayers)
    {
        activeScene->ClearSceneEntities();
        quridorState.OnDispose();
        quridorState = QuridorState(activeScene.get(), ecsManager.get(),
            eventManager, numberOfPlayers);
        SetupCamera();
        SetupScene();
        activeScene->OnInitScene();
    }

    void Quridor::SetupCamera()
    {
        auto& cameraEntity = quridorCamera.entity;

        cameraEntity = ecsManager->CreateEntity("QuridorCamera", ECS::PrefabType::ENGINE);
        auto& cameraComp = cameraEntity.AddComponent<ECS::EditorCameraComponent>(70.f, 1600.f / 900.f, 0.0001f, 1000.f);
        cameraComp.primary = true;
        cameraComp.camera->isOrtho = true;
        cameraComp.camera->SetPosition(glm::vec3(15, 15, 15));
        cameraEntity.GetComponent<ECS::TransformComponent>().Position = glm::vec3(15, 15, 15);

        quridorCamera.camera = cameraComp.camera.get();
        quridorCamera.controller = cameraEntity.GetComponent<ECS::EditorCameraControllerComponent>().controller.get();
        quridorCamera.controller->arcControls = true;
        quridorCamera.camera->ResizeCamera(1600, 900);
    }
}
