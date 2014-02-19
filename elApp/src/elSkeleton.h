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

#pragma once
#include <map>
#include <irrlicht.h>
#include <NiTE.h>
#include "elDataTypes.h"

class elSkeleton
{
public:
    struct JointInfo
    {
        elJointType jointType;
        irr::core::vector3df pos;
        irr::scene::ISceneNode* jointNode;
    };

    typedef std::map<elJointType, JointInfo> JointMap;

    elSkeleton(class elInteractionPlane* plane);
    ~elSkeleton();

    void setSkeletonHeightOffset(irr::f32 offsetMeters);
    void parseSkeleton(const nite::Skeleton& skeleton);
    void drawSkeleton() const;
    irr::f32 getSkeletonHeight() const;
    const JointMap& getJointMap() const;
    void setSkeletonVisible(bool visible = true);
    const elSkeletonMap& getTransmissionSkeletonMap() const;
    const elSkeletonMap& getTransmissionProjectedSkeletonMap() const;


private:
    JointMap jointMap;
    //Debug:
    JointMap jointMapProjected;
    irr::scene::ISceneManager* sman;
    irr::video::IVideoDriver* drv;
    irr::f32 skeletonHeight;
    bool showSkeleton;
    elSkeletonMap skeletonTMap, skeletonTPMap;
    class elInteractionPlane* interactionPlane;
    elSkeleton();

    void projectSkeleton();
    void drawSkeletonProjected() const;
    irr::f32 heightOffset;
};
