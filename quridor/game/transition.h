#pragma once

#include <Onyx/Onyx.h>

namespace quridor
{
    struct Transition
    {
        Onyx::ECS::Entity entity;
        glm::vec3 startPos = {};
        glm::vec3 endPos   = {};
        bool isFinished    = true;
        double timeCounter = 0.0;
        bool overObstacle  = false;

        Transition() = default;
        Transition(Onyx::ECS::Entity entity, const glm::vec3& startPos, const glm::vec3& endPos, bool isOverObstacle) :
            entity(entity),
            startPos(startPos),
            endPos(endPos),
            isFinished(false),
            overObstacle(isOverObstacle) {}

        void OnUpdate(double deltaTime);
    };
}
