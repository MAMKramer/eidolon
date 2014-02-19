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
#include "elGlobals.h"

using namespace irr;

elInteractionPlane::elInteractionPlane(const core::vector3df& _ulCorner, const core::vector3df& _lrCorner) : interactionBox((_ulCorner + _lrCorner) * 0.5f)
{
    //NOTE: Inverting x axis here as the screen faces us
    core::vector3df ulCorner(_ulCorner);
    core::vector3df lrCorner(_lrCorner);
    ulCorner.X = -ulCorner.X;
    lrCorner.X = -lrCorner.X;

    interactionPlane.setPlane((ulCorner + lrCorner) * 0.5f, core::vector3df(0.f, 0.f, 1.f));
    interactionSpace.UpperLeftCorner = core::position2df(ulCorner.X, ulCorner.Y);
    interactionSpace.LowerRightCorner = core::position2df(lrCorner.X, lrCorner.Y);
    interactionSpace.repair();

    interactionBox.addInternalPoint(ulCorner);
    interactionBox.addInternalPoint(lrCorner);
}

elInteractionPlane::~elInteractionPlane()
{
}

bool elInteractionPlane::getCollisionPoint(const core::vector3df& headPos, const core::vector3df& handPos, core::vector3df& outPos, core::position2df& outPosScreen)
{
    //construct a line that's of length 10m
    core::vector3df dirVec = (handPos - headPos).normalize();
    dirVec *= 10.f;
    ray.start = headPos;
    ray.end = headPos + dirVec;

    if (interactionPlane.getIntersectionWithLimitedLine(ray.start, ray.end, outPos))
    {
        core::position2df rectPos(outPos.X, outPos.Y);
        if (interactionSpace.isPointInside(rectPos))    //ray collided with plane, collision point within interaction space
        {
            outPosScreen.X =  1.f - ((rectPos.X - interactionSpace.UpperLeftCorner.X) / interactionSpace.getWidth());
            outPosScreen.Y = -(rectPos.Y - interactionSpace.LowerRightCorner.Y) / interactionSpace.getHeight();
            return true;
        }
    }
    return false;
}

bool elInteractionPlane::getCollisionPointHead(const core::vector3df& headPos, core::vector3df& outPos, core::position2df& outPosScreen)
{
    //construct a line that's of length 10m
    core::vector3df dirVec = core::vector3df(0.f, 0.f, -1.f);
    dirVec *= 10.f;
    rayHead.start = headPos;
    rayHead.end = headPos + dirVec;

    if (interactionPlane.getIntersectionWithLimitedLine(rayHead.start, rayHead.end, outPos))
    {
        core::position2df rectPos(outPos.X, outPos.Y);
        if (interactionSpace.isPointInside(rectPos))    //rayHead collided with plane, collision point within interaction space
        {
            outPosScreen.X =  1.f - ((rectPos.X - interactionSpace.UpperLeftCorner.X) / interactionSpace.getWidth());
            outPosScreen.Y = -(rectPos.Y - interactionSpace.LowerRightCorner.Y) / interactionSpace.getHeight();
            return true;
        }
    }
    return false;
}

bool elInteractionPlane::getCollisionPointWithLimitedLine(const core::line3df& collLine, core::vector3df& outPos, core::position2df& outPosScreen)
{
    if (interactionPlane.getIntersectionWithLimitedLine(collLine.start, collLine.end, outPos))
    {
        core::position2df rectPos(outPos.X, outPos.Y);
        outPosScreen.X =  1.f - ((rectPos.X - interactionSpace.UpperLeftCorner.X) / interactionSpace.getWidth());
        outPosScreen.Y = -(rectPos.Y - interactionSpace.LowerRightCorner.Y) / interactionSpace.getHeight();

        return (interactionSpace.isPointInside(rectPos));
    }
    return false;
}

const core::line3df& elInteractionPlane::getDebugAimLine() const
{
    return ray;
}

const core::line3df& elInteractionPlane::getDebugHeadLine() const
{
    return rayHead;
}

const core::aabbox3df& elInteractionPlane::getDebugSpace() const
{
    return interactionBox;
}


