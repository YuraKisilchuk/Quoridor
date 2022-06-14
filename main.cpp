#include "quridor_app.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int exit_code = 0;
    {
        auto app = std::make_unique<quridor::QuridorApp>();
        exit_code = app->OnExecute();
    }

    return exit_code;
}
