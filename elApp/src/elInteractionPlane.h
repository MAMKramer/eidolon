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

class elInteractionPlane
{
public:
    elInteractionPlane(const irr::core::vector3df& _ulCorner, const irr::core::vector3df& _lrCorner);
    ~elInteractionPlane();
    //! get the collision point with the interaction plane
    /** Provided the current aim line constructed from the head
      * over the hand hits the interaction plane, this function
      * yields the hit point.
      * \param headPos The user's head position in real world coordinates
      * \param handPos The user's hand position in real world coordinates
      * \param outPos Caller-provided parameter that stores the real-world
      * hit point
      * \param outPosScreen Caller-provided parameter that stores the interaction
      * plane relative "screen coordinates" ranging from 0.f to 1.f (ref point: upper-left corner)
      * \return True if a collision has happened. "Out params" won't store anything otherwise. */
    bool getCollisionPoint(const irr::core::vector3df& headPos, const irr::core::vector3df& handPos, irr::core::vector3df& outPos, irr::core::position2df& outPosScreen);
    //! get the collision point of the head with the interaction plane
    /** This method casts a perpendicular line from the head on to the
      * interaction plane to estimate the person's position on the screen
      * (in screen coords).
      * \param headPos The user's head position in real world coordinates
      * \param outPos Caller-provided parameter that stores the real-world
      * hit point
      * \param outPosScreen Caller-provided parameter that stores the interaction
      * plane relative "screen coordinates" ranging from 0.f to 1.f (ref point: upper-left corner)
      * \return True if a collision has happened. "Out params" won't store anything otherwise. */
    bool getCollisionPointHead(const irr::core::vector3df& headPos, irr::core::vector3df& outPos, irr::core::position2df& outPosScreen);
    bool getCollisionPointWithLimitedLine(const irr::core::line3df& collLine, irr::core::vector3df& outPos, irr::core::position2df& outPosScreen);
    //! get the head-hand line for viz purposes
    const irr::core::line3df& getDebugAimLine() const;
    //! get the head-plane line for viz purposes
    const irr::core::line3df& getDebugHeadLine() const;
    //! get the interaction space for viz purposes
    const irr::core::aabbox3df& getDebugSpace() const;

private:
    elInteractionPlane();
    irr::core::plane3df interactionPlane;
    irr::core::rectf interactionSpace;


    //DEBUG / DRAWING
    irr::core::line3df ray;
    irr::core::line3df rayHead;
    irr::core::aabbox3df interactionBox;
};
