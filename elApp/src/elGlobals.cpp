/*
CADET - Center for Advances in Digital Entertainment Technologies
Copyright 2014 Ars Electronica Futurelab
http://www.cadet.at

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "elGlobals.h"

using namespace irr;

elGlobals* elGlobals::instance = 0;

elGlobals::elGlobals()
{
    dev = createDevice(video::EDT_OPENGL, core::dimension2du(1024, 768), 32, false, false, true);
    sman = dev->getSceneManager();
    drv = dev->getVideoDriver();
    timer = dev->getTimer();
    dev->setWindowCaption(L"Eidolon");
}

elGlobals::~elGlobals()
{
    dev->drop();
}

elGlobals* elGlobals::getInstance()
{
    if (!instance)
    {
        instance = new elGlobals();
    }

    return instance;
}

IrrlichtDevice* elGlobals::getIrrlichtDevice()
{
    return getInstance()->dev;
}

scene::ISceneManager* elGlobals::getSceneManager()
{
    return getInstance()->sman;
}

video::IVideoDriver* elGlobals::getVideoDriver()
{
    return getInstance()->drv;
}

ITimer* elGlobals::getTimer()
{
    return getInstance()->timer;
}

void elGlobals::shutdown()
{
    delete getInstance();
}
