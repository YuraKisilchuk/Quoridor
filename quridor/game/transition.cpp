#include "transition.h"

using namespace Onyx;

namespace quridor
{
    void Transition::OnUpdate(double deltaTime)
    {
        auto& position = entity.GetComponent<ECS::TransformComponent>().Position;

        if (Math::compare_vec3(position, endPos, 0.01f))
            isFinished = true;
        else
        {
            if (overObstacle)
            {
                const glm::vec3 position_1 = { startPos.x, startPos.y + 2.5f, startPos.z };
                const glm::vec3 position_2 = { endPos.x,   endPos.y + 2.5f,   endPos.z   };

                if (timeCounter < 0.0)
                    position = startPos;
                if (timeCounter < 1.0)
                    position = Math::lerp_vec3(startPos, position_1, timeCounter);
                else if (timeCounter < 2.0)
                    position = Math::lerp_vec3(position_1, position_2, timeCounter - 1.0);
                else if (timeCounter < 3.0)
                    position = Math::lerp_vec3(position_2, endPos, timeCounter - 2.0);
                else position = endPos;

                timeCounter += deltaTime;
            }
            else position = Math::lerp_vec3(position, endPos, deltaTime * 7.0);
        }
    }
}
