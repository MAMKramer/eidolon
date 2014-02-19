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

#include "elInteractionPlane.h"
#include "elSkeleton.h"
#include "elGlobals.h"

using namespace nite;
using namespace irr;

elSkeleton::elSkeleton(elInteractionPlane* plane) : skeletonHeight(0.f), showSkeleton(false), interactionPlane(plane), heightOffset(0.f)
{
    sman = elGlobals::getSceneManager();
    drv = elGlobals::getVideoDriver();
    for (s32 i = 0; i < EL_NUM_JOINTS; i++)
    {
        JointInfo info;
        info.jointType = (elJointType)i;
        info.pos = core::vector3df(0.f, 0.f, 0.f);
        info.jointNode = sman->addSphereSceneNode(0.025f);
        info.jointNode->setMaterialFlag(video::EMF_LIGHTING, false);
        jointMap.insert(std::pair<elJointType, JointInfo>((elJointType)i, info));
        jointMapProjected.insert(std::pair<elJointType, JointInfo>((elJointType)i, info));
        skeletonTMap.insert(std::pair<elJointType, elVec3f>((elJointType)i, elVec3f()));
        skeletonTPMap.insert(std::pair<elJointType, elVec3f>((elJointType)i, elVec3f()));
    }
}

elSkeleton::~elSkeleton()
{

}

void elSkeleton::setSkeletonVisible(bool visible)
{
    if (showSkeleton == visible)
        return;

    for (JointMap::iterator iter = jointMap.begin(); iter != jointMap.end(); ++iter)
    {
        iter->second.jointNode->setVisible(visible);
    }

    showSkeleton = visible;
}

void elSkeleton::parseSkeleton(const Skeleton& skeleton)
{
    core::vector3df headHeight, footHeight;
    std::map<elJointType, JointInfo>::iterator iter;
    elSkeletonMap::iterator transMapIter;
    for (s32 i = 0; i < 15; i++)
    {
        const SkeletonJoint& curJoint = skeleton.getJoint((JointType)i);
//        if (curJoint.getPositionConfidence() < .5f)
//            continue;
        iter = jointMap.find((elJointType)i);
        transMapIter = skeletonTMap.find((elJointType)i);

        const Point3f pos = curJoint.getPosition();
        //invert x coord to comply with irrlicht's left-handed coord system
        core::vector3df irrPos(pos.x * -0.001f, pos.y * 0.001f, pos.z * 0.001f);
        irrPos.Y += heightOffset;
        iter->second.pos = irrPos;
        iter->second.jointNode->setPosition(irrPos);
        if ((JointType)i == JOINT_HEAD)
            headHeight = irrPos;
        if ((JointType)i == JOINT_RIGHT_FOOT)
            footHeight = irrPos;

        transMapIter->second.x = irrPos.X;
        transMapIter->second.y = irrPos.Y;
        transMapIter->second.z = irrPos.Z;

       // printf("PersHeight: %2.2f %2.2f %2.2f\n", headHeight.X - footHeight.X, headHeight.Y - footHeight.Y, headHeight.Z - footHeight.Z);
    }
    skeletonHeight = headHeight.Y - footHeight.Y;

    projectSkeleton();
}

void elSkeleton::projectSkeleton()
{
    //step 1: find head, mirror it behind the projection surface
    core::vector3df headPosMirrored = jointMap.find(EL_JOINT_HEAD)->second.pos;
    headPosMirrored.Z *= -1.f;

    core::position2df outPos;
    core::vector3df outPos3D;
    core::line3df projLine;
    projLine.start = headPosMirrored;
    elVec3f projPos;

    for (u32 i = 0; i < 15; i++)
    {
        //step 2: get the real-world joint, construct a line to mirrored head
        projLine.end = jointMap.find((elJointType)i)->second.pos;
        if (interactionPlane->getCollisionPointWithLimitedLine(projLine, outPos3D, outPos))
        {
            // we have a collision, write screen coords into proj skeleton map
            projPos.x = outPos.X;
            projPos.y = outPos.Y;
            projPos.z = 0.f;
            jointMapProjected.find((elJointType)i)->second.pos = outPos3D;
        }
        else
        {
            // no collision, write -1 for all vec elems
            /*projPos.x = -1.f;
            projPos.y = -1.f;
            projPos.z = -1.f;*/

            projPos.x = outPos.X;
            projPos.y = outPos.Y;
            projPos.z = 0.f;
            jointMapProjected.find((elJointType)i)->second.pos = outPos3D;
        }
        skeletonTPMap.find((elJointType)i)->second = projPos;

    }
}

void elSkeleton::drawSkeleton() const
{
    if (!showSkeleton)
        return;

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


    //draw head
    drv->draw3DLine(jointMap.find(EL_JOINT_HEAD)->second.pos, jointMap.find(EL_JOINT_NECK)->second.pos, video::SColor(255, 255, 0, 0));
    //draw shoulders
    drv->draw3DLine(jointMap.find(EL_JOINT_LEFT_SHOULDER)->second.pos, jointMap.find(EL_JOINT_RIGHT_SHOULDER)->second.pos, video::SColor(255, 255, 0, 0));
    //draw left arm
    drv->draw3DLine(jointMap.find(EL_JOINT_LEFT_SHOULDER)->second.pos, jointMap.find(EL_JOINT_LEFT_ELBOW)->second.pos, video::SColor(255, 255, 0, 0));
    drv->draw3DLine(jointMap.find(EL_JOINT_LEFT_ELBOW)->second.pos, jointMap.find(EL_JOINT_LEFT_HAND)->second.pos, video::SColor(255, 255, 0, 0));
    //draw right arm
    drv->draw3DLine(jointMap.find(EL_JOINT_RIGHT_SHOULDER)->second.pos, jointMap.find(EL_JOINT_RIGHT_ELBOW)->second.pos, video::SColor(255, 255, 0, 0));
    drv->draw3DLine(jointMap.find(EL_JOINT_RIGHT_ELBOW)->second.pos, jointMap.find(EL_JOINT_RIGHT_HAND)->second.pos, video::SColor(255, 255, 0, 0));
    //draw torso
    drv->draw3DLine(jointMap.find(EL_JOINT_LEFT_SHOULDER)->second.pos, jointMap.find(EL_JOINT_RIGHT_HIP)->second.pos, video::SColor(255, 255, 0, 0));
    drv->draw3DLine(jointMap.find(EL_JOINT_RIGHT_SHOULDER)->second.pos, jointMap.find(EL_JOINT_LEFT_HIP)->second.pos, video::SColor(255, 255, 0, 0));
    //draw left leg
    drv->draw3DLine(jointMap.find(EL_JOINT_LEFT_HIP)->second.pos, jointMap.find(EL_JOINT_LEFT_KNEE)->second.pos, video::SColor(255, 255, 0, 0));
    drv->draw3DLine(jointMap.find(EL_JOINT_LEFT_KNEE)->second.pos, jointMap.find(EL_JOINT_LEFT_FOOT)->second.pos, video::SColor(255, 255, 0, 0));
    //draw right leg
    drv->draw3DLine(jointMap.find(EL_JOINT_RIGHT_HIP)->second.pos, jointMap.find(EL_JOINT_RIGHT_KNEE)->second.pos, video::SColor(255, 255, 0, 0));
    drv->draw3DLine(jointMap.find(EL_JOINT_RIGHT_KNEE)->second.pos, jointMap.find(EL_JOINT_RIGHT_FOOT)->second.pos, video::SColor(255, 255, 0, 0));

    drawSkeletonProjected();
}

void elSkeleton::drawSkeletonProjected() const
{
    //draw head
    drv->draw3DLine(jointMapProjected.find(EL_JOINT_HEAD)->second.pos, jointMapProjected.find(EL_JOINT_NECK)->second.pos, video::SColor(255, 255, 0, 0));
    //draw shoulders
    drv->draw3DLine(jointMapProjected.find(EL_JOINT_LEFT_SHOULDER)->second.pos, jointMapProjected.find(EL_JOINT_RIGHT_SHOULDER)->second.pos, video::SColor(255, 255, 0, 0));
    //draw left arm
    drv->draw3DLine(jointMapProjected.find(EL_JOINT_LEFT_SHOULDER)->second.pos, jointMapProjected.find(EL_JOINT_LEFT_ELBOW)->second.pos, video::SColor(255, 255, 0, 0));
    drv->draw3DLine(jointMapProjected.find(EL_JOINT_LEFT_ELBOW)->second.pos, jointMapProjected.find(EL_JOINT_LEFT_HAND)->second.pos, video::SColor(255, 255, 0, 0));
    //draw right arm
    drv->draw3DLine(jointMapProjected.find(EL_JOINT_RIGHT_SHOULDER)->second.pos, jointMapProjected.find(EL_JOINT_RIGHT_ELBOW)->second.pos, video::SColor(255, 255, 0, 0));
    drv->draw3DLine(jointMapProjected.find(EL_JOINT_RIGHT_ELBOW)->second.pos, jointMapProjected.find(EL_JOINT_RIGHT_HAND)->second.pos, video::SColor(255, 255, 0, 0));
    //draw torso
    drv->draw3DLine(jointMapProjected.find(EL_JOINT_LEFT_SHOULDER)->second.pos, jointMapProjected.find(EL_JOINT_RIGHT_HIP)->second.pos, video::SColor(255, 255, 0, 0));
    drv->draw3DLine(jointMapProjected.find(EL_JOINT_RIGHT_SHOULDER)->second.pos, jointMapProjected.find(EL_JOINT_LEFT_HIP)->second.pos, video::SColor(255, 255, 0, 0));
    //draw left leg
    drv->draw3DLine(jointMapProjected.find(EL_JOINT_LEFT_HIP)->second.pos, jointMapProjected.find(EL_JOINT_LEFT_KNEE)->second.pos, video::SColor(255, 255, 0, 0));
    drv->draw3DLine(jointMapProjected.find(EL_JOINT_LEFT_KNEE)->second.pos, jointMapProjected.find(EL_JOINT_LEFT_FOOT)->second.pos, video::SColor(255, 255, 0, 0));
    //draw right leg
    drv->draw3DLine(jointMapProjected.find(EL_JOINT_RIGHT_HIP)->second.pos, jointMapProjected.find(EL_JOINT_RIGHT_KNEE)->second.pos, video::SColor(255, 255, 0, 0));
    drv->draw3DLine(jointMapProjected.find(EL_JOINT_RIGHT_KNEE)->second.pos, jointMapProjected.find(EL_JOINT_RIGHT_FOOT)->second.pos, video::SColor(255, 255, 0, 0));
}

f32 elSkeleton::getSkeletonHeight() const
{
    return skeletonHeight;
}

const elSkeleton::JointMap& elSkeleton::getJointMap() const
{
    return jointMap;
}

const elSkeletonMap& elSkeleton::getTransmissionSkeletonMap() const
{
    return skeletonTMap;
}

const elSkeletonMap& elSkeleton::getTransmissionProjectedSkeletonMap() const
{
    return skeletonTPMap;
}

void elSkeleton::setSkeletonHeightOffset(f32 offsetMeters)
{
    heightOffset = offsetMeters;
    printf("HMSKELETON: New height offset: %2.3f\n", heightOffset);
}
