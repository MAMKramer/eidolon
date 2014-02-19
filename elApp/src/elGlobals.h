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

#include <irrlicht.h>

class elGlobals
{
public:
    static irr::IrrlichtDevice* getIrrlichtDevice();
    static irr::scene::ISceneManager* getSceneManager();
    static irr::video::IVideoDriver* getVideoDriver();
    static irr::ITimer* getTimer();

    static void shutdown();

private:
    elGlobals();
    elGlobals(const elGlobals&);
    ~elGlobals();

    static elGlobals* getInstance();
    static elGlobals* instance;

    irr::IrrlichtDevice* dev;
    irr::scene::ISceneManager* sman;
    irr::video::IVideoDriver* drv;
    irr::ITimer* timer;

};
