#pragma once

#include <Onyx/Onyx.h>

#include "quridor/quridor.h"

namespace quridor
{
    class QuridorApp : public Onyx::Core::App
    {
    private:
        std::unique_ptr<Quridor> quridor;

    public:
        QuridorApp();
    };
}
