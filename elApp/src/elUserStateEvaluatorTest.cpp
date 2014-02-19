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

#include "elUserStateEvaluatorTest.h"
#include "elGlobals.h"

#define CLICK_INTERVAL 1500
#define INTERACT_THRESH 0.5f

using namespace irr;

elUserStateEvaluatorTest::elUserStateEvaluatorTest() : userState(US_NONE), interactionState(IS_NONE), leftHandInteracting(false), rightHandInteracting(false)
{
    timer = elGlobals::getTimer();
}

elUserStateEvaluatorTest::~elUserStateEvaluatorTest()
{

}

elUserState elUserStateEvaluatorTest::getUserState() const
{
    return userState;
}

elInteractionState elUserStateEvaluatorTest::getInteractionState() const
{
    return interactionState;
}

const core::vector3df& elUserStateEvaluatorTest::getHandPosition() const
{
    return handPos;
}

const core::vector3df& elUserStateEvaluatorTest::getLeftHandPosition() const
{
    return leftHandPos;
}

const core::vector3df& elUserStateEvaluatorTest::getRightHandPosition() const
{
    return rightHandPos;
}

const core::vector3df& elUserStateEvaluatorTest::getHeadPosition() const
{
    return headPos;
}

bool elUserStateEvaluatorTest::isLeftHandInteracting() const
{
    return leftHandInteracting;
}

bool elUserStateEvaluatorTest::isRightHandInteracting() const
{
    return rightHandInteracting;
}

void elUserStateEvaluatorTest::evaluateSkeleton(const elSkeleton& _skeleton, bool isTracked)
{
    if (!isTracked)
    {
        userState = US_NONE;
        interactionState = IS_NONE;
        return;
    }

    //simple check: see if left hand is sufficiently pointed away from the torso:
    const elSkeleton::JointMap& jointMap = _skeleton.getJointMap();
    leftHandPos = jointMap.find(EL_JOINT_LEFT_HAND)->second.pos;
    const core::vector3df& leftTargetPos = jointMap.find(EL_JOINT_LEFT_HIP)->second.pos;
    rightHandPos = jointMap.find(EL_JOINT_RIGHT_HAND)->second.pos;
    const core::vector3df& rightTargetPos = jointMap.find(EL_JOINT_RIGHT_HIP)->second.pos;
    headPos = jointMap.find(EL_JOINT_HEAD)->second.pos;

    //f32 leftHandOut = leftTargetPos.Z - leftHandPos.Z;
    //f32 rightHandOut = rightTargetPos.Z - rightHandPos.Z;

    f32 leftHandOut = leftTargetPos.getDistanceFrom(leftHandPos);
    f32 rightHandOut = rightTargetPos.getDistanceFrom(rightHandPos);

    leftHandInteracting = leftHandOut > INTERACT_THRESH;
    rightHandInteracting = rightHandOut > INTERACT_THRESH;

    // check 1: do both hands exceed interaction threshold? if so, choose the one further out
    if (leftHandInteracting && rightHandInteracting)
    {
        if (leftHandOut > rightHandOut)
        {
            handPosLast = handPos;
            handPos = leftHandPos;
        }
        else
        {
            handPosLast = handPos;
            handPos = rightHandPos;
        }
    }
    // only left hand interacts
    else if (leftHandInteracting)
    {
        handPosLast = handPos;
        handPos = leftHandPos;
    }
    // only right hand interacts
    else if (rightHandInteracting)
    {
        handPosLast = handPos;
        handPos = rightHandPos;
    }
    // no interaction, leave
    else
    {
        userState = US_IDLE;
        interactionState = IS_NONE;
        return;
    }


    userState = US_INTERACTING;
    if (interactionState == IS_NONE)
        interactionState = IS_SELECTING;

    //evaluate gesture. simple first approach
    if (handPos.getDistanceFrom(handPosLast) < 0.01f)
    {
        const u32 curTime = timer->getTime();
        switch (interactionState)
        {
        case IS_SELECTING:
            interactionState = IS_CLICK_START;
            stateTransitionTime = curTime;
            break;

        case IS_CLICK_START:
            if ((curTime - stateTransitionTime) > CLICK_INTERVAL)
            {
                interactionState = IS_CLICK_END;
            }
            break;

        case IS_CLICK_END:
        case IS_NONE:
            break;
        }

    }
    else
    {
        interactionState = IS_SELECTING;
    }
}
