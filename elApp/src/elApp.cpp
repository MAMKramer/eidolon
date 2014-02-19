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

#include <iostream>
#include "elApp.h"
#include "elUserStateEvaluatorTest.h"
#include "elInteractionPlane.h"
#include "elGlobals.h"

using namespace nite;
using namespace irr;

elApp::elApp() : devOK(false),
                 newFrame(false),
                 showDepth(false),
                 terminateExec(false),
                 pixBufferCount(0),
                 evalRadius(0.5f),
                 userId(-1),
                 stateSender(),
                 lastUserState(US_NONE),
                 lastInteractionState(IS_NONE)
{
    dev = elGlobals::getIrrlichtDevice();
    sman = dev->getSceneManager();
    drv = dev->getVideoDriver();

    cam = sman->addCameraSceneNodeMaya(0, -1500.f, 200.f, 25.f);
    cam->setFOV(0.1f);
    cam->setTarget(core::vector3df(0.f, 0.f, 0.f));
    depthTex = drv->addTexture(core::dimension2du(320, 240), "depthMap", video::ECF_A8R8G8B8);

    devOK = igNiTE();
    font = dev->getGUIEnvironment()->getFont("../res/fontlucida.png");
    dev->setEventReceiver(this);
    dev->setWindowCaption(L"Eidolon");

    if (!parseConfig())
    {
        printf("ERROR: Unable to parse config file!\n");
    }

    stateEvaluator = new elUserStateEvaluatorTest();
    interactionPlane = new elInteractionPlane(iPlaneUL, iPlaneLR);
    skeleton = new elSkeleton(interactionPlane);
    skeleton->setSkeletonHeightOffset(heightOffset);

    hitTestNode = sman->addSphereSceneNode(0.05f);
    hitTestNode->getMaterial(0).AmbientColor = video::SColor(255, 255, 0, 255);
    hitTestNode->getMaterial(0).DiffuseColor = video::SColor(255, 255, 0, 255);
    hitTestNode->getMaterial(0).ColorMaterial = video::ECM_NONE;
    hitTestNode->setMaterialFlag(video::EMF_LIGHTING, false);
    hitTestNode->setVisible(false);

    hitTestNodeHead = sman->addSphereSceneNode(0.05f);
    hitTestNodeHead->getMaterial(0).AmbientColor = video::SColor(255, 255, 255, 0);
    hitTestNodeHead->getMaterial(0).DiffuseColor = video::SColor(255, 255, 255, 0);
    hitTestNodeHead->getMaterial(0).ColorMaterial = video::ECM_NONE;
    hitTestNodeHead->setMaterialFlag(video::EMF_LIGHTING, false);
    hitTestNodeHead->setVisible(false);

    thread = std::thread(&elApp::run, this);
}

elApp::~elApp()
{
    delete stateEvaluator;
    delete skeleton;
    delete interactionPlane;
    if (devOK)
    {
        NiTE::shutdown();
    }
}

bool elApp::OnEvent(const SEvent& event)
{
    if (event.EventType == EET_KEY_INPUT_EVENT && event.KeyInput.PressedDown == 1)
    {
        switch (event.KeyInput.Key)
        {
        case KEY_KEY_D:
            showDepth = !showDepth;
            return true;

        case KEY_KEY_T:
            saveScreenshot();
            return true;

        case KEY_KEY_R:
            cam->setTarget(core::vector3df(0.f, 0.f, 0.f));
            return true;

        default:
            return false;
        }
    }

    return false;
}

void elApp::run()
{
    while (!terminateExec)
    {
        if (devOK)
        {
            UserTrackerFrameRef frame;
            if (userTracker.readFrame(&frame) != STATUS_OK)
            {
                printf("Unable to obtain next frame!\n");
            }
            else
            {
                const nite::Array<nite::UserData>& users = frame.getUsers();
                const s32 len = users.getSize();
                // if multiple users exist AND are active, take the closest one
                s32 userIdx = -1;
                for (s32 i = 0; i < len; i++)
                {
                    const nite::UserData& user = users[i];
                    if (user.isNew())
                    {
                        userTracker.startSkeletonTracking(user.getId());
                    }
                    else if (user.isLost())
                    {
                        if (user.getId() == userId) //the active player was lost, set back info, look for another player in the next frame
                        {
                            userId = -1;
                            userIdx = -1;
                            i = len;    //break from loop
                        }
                        userTracker.stopSkeletonTracking(user.getId());
                    }
                    else if (user.getId() == userId)
                    {
                        const Point3f& tPos = user.getSkeleton().getJoint(nite::JOINT_TORSO).getPosition();
                        //create temp pos for skeleton putting it LH (irrlicht)
                        core::vector3df targetPos(-tPos.x, tPos.y, tPos.z);
                        //set eval height to same level to neglect three-dimensionality
                        evalPos.Y = tPos.y;
                        if (evalPos.getDistanceFrom(targetPos) < evalRadius)
                        {
                            userIdx = i;
                            i = len;
                        }
                    }
                    else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED)
                    {
                        const Point3f& tPos = user.getSkeleton().getJoint(nite::JOINT_TORSO).getPosition();
                        //create temp pos for skeleton putting it LH (irrlicht)
                        core::vector3df targetPos(-tPos.x, tPos.y, tPos.z);
                        //set eval height to same level to neglect three-dimensionality
                        evalPos.Y = tPos.y;
                        if (evalPos.getDistanceFrom(targetPos) < evalRadius)
                        {
                            userIdx = i;
                            userId = user.getId();
                            i = len;
                        }
                    }
                }

                if (userIdx != -1)
                {
                    const nite::UserData& user = users[userIdx];
                    skeleton->parseSkeleton(user.getSkeleton());
                    stateEvaluator->evaluateSkeleton(*skeleton);
                }
                else
                {
                    //no active tracking, inform stateEval
                    stateEvaluator->evaluateSkeleton(*skeleton, false);
                }

                if (showDepth)
                {
                    openni::VideoFrameRef depthFrameRef = frame.getDepthFrame();
                    memcpy(pixBuffer, depthFrameRef.getData(), depthFrameRef.getDataSize());
                    pixBufferCount = depthFrameRef.getDataSize();
                    newFrame = true;
                }
                updateUserPositions(users);
            }
        }
    }
}

void elApp::execute()
{
    while (dev->run())
    {
        if (newFrame)   //this will only be true once showDepth is true beforehand, no need to double-check
        {
            u32* texContent = (u32*)(depthTex->lock());
            const u32 len = pixBufferCount/2;  //u16
            for (u32 i = 0; i < len; i++)
            {
                u16 curPixel = (pixBuffer[i]);//255;
                u8 pixVal = 0;
                if (curPixel < 8000 && curPixel > 300)
                    pixVal = 255 - (u8) (( (f32)(curPixel) / 8000.f) * 255.f);
                texContent[i] = video::SColor(255, pixVal, pixVal, pixVal).color;
            }
            depthTex->unlock();
            newFrame = false;
        }

        core::stringw drawString = L"Person Height: ";
        drawString += skeleton->getSkeletonHeight();
        evalStateInfo(drawString);


        drv->beginScene(true, true, video::SColor(255, 0, 0, 0));
        if (showDepth)
            drv->draw2DImage(depthTex, core::position2di(10, 100));

        skeleton->drawSkeleton();

        drawDriverStuff();
        sman->drawAll();
        font->draw(drawString, core::recti(10, 10, 200, 200), video::SColor(255, 255, 255, 255));
        drv->endScene();
    }
    terminateExec = true;
    thread.join();
    elGlobals::shutdown();
}

bool elApp::igNiTE()
{
    if (NiTE::initialize() != STATUS_OK)
    {
        printf("ERROR: Unable to init NiTE!\n");
        return false;
    }
    if (userTracker.create() != STATUS_OK)
    {
        printf("ERROR: Unable to create user tracker!\n");
        NiTE::shutdown();
        return false;
    }
    return true;
}

void elApp::saveScreenshot()
{
    video::IImage* scrn = 0;
    core::stringc fname = "";
    fname += dev->getTimer()->getRealTime();
    io::path filename = fname.c_str();
    filename += ".png";

    scrn = drv->createScreenShot();
    drv->writeImageToFile(scrn, filename);
    scrn->drop();
}

void elApp::drawDriverStuff()
{
    video::SMaterial matA;
    matA.AmbientColor = video::SColor(255, 255, 0, 0);
    matA.DiffuseColor = video::SColor(255, 255, 0, 0);
    matA.ColorMaterial = video::ECM_NONE;
    matA.AntiAliasing = video::EAAM_FULL_BASIC;
    matA.Lighting = false;
    matA.Thickness = 1.5f;
    matA.MaterialType = video::EMT_TRANSPARENT_ALPHA_CHANNEL;
    drv->setMaterial(matA);

    drv->setTransform(video::ETS_WORLD, core::IdentityMatrix);

    drv->draw3DLine(core::vector3df(0.f, 0.f, 0.f), core::vector3df(1.f, 0.f, 0.f), video::SColor(255, 255, 0, 0));
    drv->draw3DLine(core::vector3df(0.f, 0.f, 0.f), core::vector3df(0.f, 1.f, 0.f), video::SColor(255, 0, 255, 0));
    drv->draw3DLine(core::vector3df(0.f, 0.f, 0.f), core::vector3df(0.f, 0.f, 1.f), video::SColor(255, 0, 0, 255));

    //debug
    if (hitTestNode->isVisible())
    {
        drv->draw3DLine(interactionPlane->getDebugAimLine().start, interactionPlane->getDebugAimLine().end, video::SColor(255, 128, 0, 128));
    }
    if (hitTestNodeHead->isVisible())
    {
        drv->draw3DLine(interactionPlane->getDebugHeadLine().start, interactionPlane->getDebugHeadLine().end, video::SColor(255, 128, 128, 0));
    }
    drv->draw3DBox(interactionPlane->getDebugSpace());
}

void elApp::evalStateInfo(core::stringw& drawString)
{
    drawString += L"\nUSER STATE: ";
    core::vector3df outPos;
    core::position2df outPosRel;

    elUserState curUserState = stateEvaluator->getUserState();
    elInteractionState curInteractionState = stateEvaluator->getInteractionState();

    //if smth has changed, inform the viz:
    if (curUserState != lastUserState)
    {
        stateSender.newUserState(curUserState);
        lastUserState = curUserState;
    }

    if (curInteractionState != lastInteractionState)
    {
        stateSender.newInteractionState(curInteractionState);
        lastInteractionState = curInteractionState;
    }

    /*if (curInteractionState != IS_NONE)
    {
        core::position2df screenPosL(-1.f, -1.f), screenPosR(-1.f, -1.f);
        if (stateEvaluator->isLeftHandInteracting())
            projMapping->getScreenSpaceForHand(stateEvaluator->getLeftHandPosition(), screenPosL);
        if (stateEvaluator->isRightHandInteracting())
            projMapping->getScreenSpaceForHand(stateEvaluator->getRightHandPosition(), screenPosR);

        projectorSender.newScreenPosition(elVec2f(screenPosL.X, screenPosL.Y), elVec2f(screenPosR.X, screenPosR.Y));
    }
    else
    {
        projectorSender.newScreenPosition(elVec2f(-1.f, -1.f), elVec2f(-1.f, -1.f));
    }*/

    //first check: is user != none, calc head pos on screen
    if (curUserState != US_NONE)
    {
        if(interactionPlane->getCollisionPointHead(stateEvaluator->getHeadPosition(), outPos, outPosRel))
        {
            stateSender.newHeadPosition(outPosRel.X, outPosRel.Y);
            stateSender.newSkeletonMap(skeleton->getTransmissionSkeletonMap());
            stateSender.newProjectedSkeletonMap(skeleton->getTransmissionProjectedSkeletonMap());
            hitTestNodeHead->setVisible(true);
            hitTestNodeHead->setPosition(outPos);
        }
        else
        {
            hitTestNodeHead->setVisible(false);
        }
    }

    switch (curUserState)
    {
    case US_NONE:
        drawString += L"NONE\n";
        skeleton->setSkeletonVisible(false);
        hitTestNodeHead->setVisible(false);
        break;

    case US_IDLE:
        drawString += L"IDLE\n";
        skeleton->setSkeletonVisible(true);
        break;

    case US_INTERACTING:
        skeleton->setSkeletonVisible(true);
        drawString += L"INTERACTING\n";
        if (interactionPlane->getCollisionPoint(stateEvaluator->getHeadPosition(), stateEvaluator->getHandPosition(), outPos, outPosRel))
        {
            stateSender.newInteractionPosition(outPosRel.X, outPosRel.Y);
            hitTestNode->setVisible(true);
            hitTestNode->setPosition(outPos);
        }
        else
        {
            hitTestNode->setVisible(false);
        }
        break;
    }

    drawString += L"INT STATE : ";
    switch (curInteractionState)
    {
    case IS_NONE:
        drawString += L"NONE\n";
        break;

    case IS_SELECTING:
        drawString += L"SELECTING\n";
        break;

    case IS_CLICK_START:
        drawString += L"CLICK_START\n";
        break;

    case IS_CLICK_END:
        drawString += L"CLICK_END\n";
        break;
    }
}

bool elApp::parseConfig()
{
    io::IrrXMLReader* xmldoc = io::createIrrXMLReader("../cfg/config.xml");

    if (!xmldoc)
        return false;

    core::stringc host;
    s32 port;

    while (xmldoc->read())
    {
        if (strcmp(xmldoc->getNodeName(), "sender") == 0)
        {
            host = xmldoc->getAttributeValue("host");
            port = xmldoc->getAttributeValueAsInt("port");
            stateSender.setupConnection(host.c_str(), (u16)port);
        }
        else if (strcmp(xmldoc->getNodeName(), "evaluator") == 0)
        {
            evalPos.Z = xmldoc->getAttributeValueAsFloat("evaldist") * 1000.f;
            evalRadius = xmldoc->getAttributeValueAsFloat("radius") * 1000.f;
            heightOffset = xmldoc->getAttributeValueAsFloat("heightoffset");
            printf("Max evaluation distance from cam: %2.3f\n", evalPos.Z);
        }
        else if (strcmp(xmldoc->getNodeName(), "interaction") == 0)
        {
            iPlaneUL.X = xmldoc->getAttributeValueAsFloat("ulx");
            iPlaneUL.Y = xmldoc->getAttributeValueAsFloat("uly");
            iPlaneUL.Z = 0.f;

            iPlaneLR.X = xmldoc->getAttributeValueAsFloat("lrx");
            iPlaneLR.Y = xmldoc->getAttributeValueAsFloat("lry");
            iPlaneLR.Z = 0.f;
        }
    }
    return true;
}

void elApp::updateUserPositions(const nite::Array<nite::UserData>& users)
{
    return;
    const u32 len = users.getSize();

    for (u32 i = 0; i < len; i++)
    {
        const nite::UserData& user = users[i];
        if (user.isNew())
        {
            scene::ISceneNode* sn = sman->addMeshSceneNode(sman->getMesh("../res/user.obj"));
            sn->setVisible(false);
            sn->setMaterialFlag(video::EMF_LIGHTING, false);
            sn->setMaterialFlag(video::EMF_WIREFRAME, true);
            userPositions.insert(std::pair<s32, scene::ISceneNode*>(user.getId(), sn));
        }
        else if (user.isLost())
        {
            sman->addToDeletionQueue(userPositions.find(user.getId())->second);
            userPositions.erase(user.getId());
        }
        else
        {
            if (user.getSkeleton().getState() == SKELETON_TRACKED)
            {
                scene::ISceneNode* sn = userPositions.find(user.getId())->second;
                sn->setVisible(true);
                const Point3f tPos = user.getSkeleton().getJoint(nite::JOINT_TORSO).getPosition();
                sn->setPosition(core::vector3df(tPos.x * -0.001f, tPos.y * 0.001f, tPos.z * 0.001f));
            }
        }
    }
}
