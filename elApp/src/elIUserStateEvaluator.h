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

#include "elSkeleton.h"
#include "elUserStates.h"

class elIUserStateEvaluator
{
public:
    virtual ~elIUserStateEvaluator() {}
    //! Should yield the current state the user is in
    virtual elUserState getUserState() const = 0;
    //! Obtains - once the user is in US_INTERACTING - the state of interaction
    virtual elInteractionState getInteractionState() const = 0;
    //! Once the user is US_INTERACTING this yields the active hand's position in real world coords from the camera (metric floats)
    virtual const irr::core::vector3df& getHandPosition() const = 0;
    virtual const irr::core::vector3df& getLeftHandPosition() const = 0;
    virtual const irr::core::vector3df& getRightHandPosition() const = 0;
    virtual const irr::core::vector3df& getHeadPosition() const = 0;
    virtual bool isLeftHandInteracting() const = 0;
    virtual bool isRightHandInteracting() const = 0;
    //! Main computation method the derivate should evalute the state based on the skeleton
    /** \param isTracked State whether the skeleton has updated in the last frame (-> if a user is there atm) */
    virtual void evaluateSkeleton(const elSkeleton& _skeleton, bool isTracked = true) = 0;
};
