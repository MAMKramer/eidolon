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

#include <thread>
#include "elSkeleton.h"
#include "elStateSender.h"


class elApp : public irr::IEventReceiver
{
public:
    elApp();
    ~elApp();

    bool OnEvent(const irr::SEvent& event);
    void execute();

private:
    bool igNiTE();  //well, that was obvious.
    void saveScreenshot();
    void drawDriverStuff();
    void evalStateInfo(irr::core::stringw& drawString);
    bool parseConfig();

    //state-keepers
    bool devOK;
    bool newFrame;
    bool showDepth;

    //threading
    void run();
    bool terminateExec;
    std::thread thread;


    //buffers the pixels from the camera
    //necessary as we can't update the tex from a thread
    openni::DepthPixel pixBuffer[640*480];
    int pixBufferCount;
    nite::UserTracker userTracker;

    //graphics stuff
    irr::IrrlichtDevice* dev;
    irr::scene::ISceneManager* sman;
    irr::video::IVideoDriver* drv;
    irr::scene::ICameraSceneNode* cam;
    irr::video::ITexture* depthTex;
    irr::gui::IGUIFont* font;
    irr::scene::ISceneNode* hitTestNode, *hitTestNodeHead;
    std::map<irr::s32, irr::scene::ISceneNode*> userPositions;
    void updateUserPositions(const nite::Array<nite::UserData>& users);

    //eval-related
    irr::core::vector3df evalPos;
    irr::f32 evalRadius;
    elSkeleton* skeleton;
    class elIUserStateEvaluator* stateEvaluator;
    class elInteractionPlane* interactionPlane;
    irr::core::vector3df iPlaneUL, iPlaneLR;
    irr::s32 userId;    //nite-assigned userid of the currently-active user
    irr::f32 heightOffset;

    //network
    elStateSender stateSender;
    elUserState lastUserState;
    elInteractionState lastInteractionState;
};
