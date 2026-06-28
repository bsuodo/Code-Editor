#include "core.h"

int main()
{
    App app;

    while (app.getShouldRestart())
    {
        app.Run();
    }

    return 0;
}