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

//! Vector helper class to store three-dimensional positions and the like
class elVec3f
{
public:
    elVec3f() : x(0.f), y(0.f), z(0.f)
    {}
    elVec3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
    {}
    elVec3f(const elVec3f& other) : x(other.x), y(other.y), z(other.z)
    {}
    elVec3f operator= (const elVec3f& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }

    ~elVec3f()
    {}

    float x, y, z;
};

//! Vector helper class to store two-dimensional positions and the like
class elVec2f
{
public:
    elVec2f() : x(0.f), y(0.f)
    {}
    elVec2f(float _x, float _y) : x(_x), y(_y)
    {}
    elVec2f(const elVec2f& other) : x(other.x), y(other.y)
    {}
    elVec2f operator= (const elVec2f& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    ~elVec2f()
    {}

    float x, y;
};

//! enumerates the joints in the skeleton
/** This is actually a copy of NiTE's enum introduced
  * to drop NiTE as a dependency for the transmission
  * lib. */
enum elJointType
{
	EL_JOINT_HEAD,
	EL_JOINT_NECK,

	EL_JOINT_LEFT_SHOULDER,
	EL_JOINT_RIGHT_SHOULDER,
	EL_JOINT_LEFT_ELBOW,
	EL_JOINT_RIGHT_ELBOW,
	EL_JOINT_LEFT_HAND,
	EL_JOINT_RIGHT_HAND,

	EL_JOINT_TORSO,

	EL_JOINT_LEFT_HIP,
	EL_JOINT_RIGHT_HIP,
	EL_JOINT_LEFT_KNEE,
	EL_JOINT_RIGHT_KNEE,
	EL_JOINT_LEFT_FOOT,
	EL_JOINT_RIGHT_FOOT,
	EL_NUM_JOINTS
};

//! Map that holds the entire skeleton on the receiver side
typedef std::map<elJointType, elVec3f> elSkeletonMap;
