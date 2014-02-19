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

#include "elIUserStateEvaluator.h"

class elUserStateEvaluatorTest : public elIUserStateEvaluator
{
public:
    elUserStateEvaluatorTest();
    ~elUserStateEvaluatorTest();

    void evaluateSkeleton(const elSkeleton& _skeleton, bool isTracked = true);
    elUserState getUserState() const;
    elInteractionState getInteractionState() const;
    const irr::core::vector3df& getHandPosition() const;
    const irr::core::vector3df& getLeftHandPosition() const;
    const irr::core::vector3df& getRightHandPosition() const;
    const irr::core::vector3df& getHeadPosition() const;
    bool isLeftHandInteracting() const;
    bool isRightHandInteracting() const;

private:
    elUserState userState;
    elInteractionState interactionState;
    irr::core::vector3df handPos, leftHandPos, rightHandPos, handPosLast, headPos;
    irr::ITimer* timer;
    irr::u32 stateTransitionTime;
    bool leftHandInteracting, rightHandInteracting;
};
