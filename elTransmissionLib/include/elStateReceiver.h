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

#include <thread>
#include <atomic>
#include "elUserStates.h"
#include "elDataTypes.h"

//! Base class for receiving updates from the tracker
/** elStateReceiver implements the UDP-based network protocol and makes
  * events available via callbacks.
  * \note The deriving class might need to take care of thread safety. */
class elStateReceiver
{
public:
    //! The default constructor, use in combination with setupConnection
    elStateReceiver();
    //! The constructor
    /** \param port The UDP port to listen on */
    elStateReceiver(unsigned short port);
    //! The destructor
    virtual ~elStateReceiver();
	//! Called for late initialization, only valid with default constructor
    /** \param port The UDP port to listen on */
	virtual void setupConnection(unsigned short port);
    //! Called when the user state changes
    /** \param newState The currently active user state
      * \see elUserState for further information */
    virtual void updateUserState(elUserState newState) = 0;
    //! Called when the interaction state changes
    /** \param newState The currently active interaction state
      * \see elInteraction for further information */
    virtual void updateInteractionState(elInteractionState newState) = 0;
    //! Called when the interaction position changes
    /** Once the user 'aims' towards the interaction plane and a line
      * can be successfully cast onto it, the collision position is
      * returned in coordinates relative to the interaction plane's
      * dimensions, ranging from 0.f to 1.f
      * \param relX the horizontal coord relative to the interaction plane's width
      * \param relY the vertical coord relative to the interaction plane's height*/
    virtual void updateInteractionPosition(float relX, float relY) = 0;
    //! Called when the user's head position changes
    /** This is a handy callback to retrieve the head's position mapped
      * already onto the interaction plane in relative coordinates ranging from 0.f to 1.f
      * \param relX the horizontal coord relative to the interaction plane's width
      * \param relY the vertical coord relative to the interaction plane's height*/
    virtual void updateHeadPosition(float relX, float relY) = 0;
    //! Called when a 'new skeleton' is available
    /** Whenever the user's skeleton has updated, the skeleton map
      * can be retrieved over this callback.
      * This is represented as a map with the joint name as key and
      * the joint position (real world, left handed, meters) with the
      * camera as origin.
      * \param skeleton The current joint positions
      * \see elJointType */
    virtual void updateSkeleton(const elSkeletonMap& skeleton) = 0;
    //! Called when a 'new projected skeleton' is available
    /** Similar to updateSkeleton, this method is called when a new
      * projected skeleton is available. This elSkeletonMap stores joint
      * information already in screen coordinates as projected onto the
      * mirror surface with respect to the user's head (eye) position.
      * Representing 2D information, only the X and Y members of the elVec3f
      * store the (relative) screen coords.
      * \note Coords can be >1.f and <0.f to correctly draw the skeleton even
      * if joints are outside the screen space
      * \param skeleton The current projected joint positions
      * \see elJointType */
    virtual void updateProjectedSkeleton(const elSkeletonMap& skeleton) = 0;

private:
    elStateReceiver(const elStateReceiver& other);

    void init();
    void parseSkeleton(const char* recvBuf, int recvLen);
    void parseProjectedSkeleton(const char* recvBuf, int recvLen);

    void run();
    std::atomic_bool terminateExec;
    bool threadRunning;
    std::thread thread;

    class UDPManager* udpMan;
    elSkeletonMap sMap, sPMap;
};
