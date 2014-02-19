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

#include "elUserStates.h"
#include "elDataTypes.h"

class elStateSender
{
public:
    elStateSender();
    ~elStateSender();

    void setupConnection(const char* host, unsigned short port);
    bool newUserState(elUserState state);
    bool newInteractionState(elInteractionState state);
    bool newInteractionPosition(float relX, float relY);
    bool newHeadPosition(float relX, float relY);
    bool newSkeletonMap(const elSkeletonMap& map);
    bool newProjectedSkeletonMap(const elSkeletonMap& map);

private:
    class UDPManager* udpMan;
    void closeConnection();

};
