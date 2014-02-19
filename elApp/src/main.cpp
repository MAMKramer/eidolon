#include "elApp.h"


int main()
{
    elApp* app = new elApp();
    app->execute();

    delete app;
}
