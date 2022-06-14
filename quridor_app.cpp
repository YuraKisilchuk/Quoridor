#include "quridor_app.h"

namespace quridor
{
    QuridorApp::QuridorApp()
    {
        quridor = std::make_unique<Quridor>();
        appLayerStack.PushLayer(std::move(quridor));
    }
}
