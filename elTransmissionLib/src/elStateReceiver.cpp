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

#include "UDPManager.h"
#include "elStateReceiver.h"

#include <cassert>

#ifdef _WIN32
# define WORKAROUND_EXIT_LOCKUP
#endif

elStateReceiver::elStateReceiver() : threadRunning(false)
{
    init();
}
elStateReceiver::elStateReceiver(unsigned short port) : threadRunning(false)
{
	init();
	setupConnection(port);
}

elStateReceiver::~elStateReceiver()
{
	if (threadRunning)
	{
		terminateExec.store(true);

#ifdef WORKAROUND_EXIT_LOCKUP
		thread.detach();
		Sleep(1100);
#else
		thread.join();
#endif

		threadRunning = false;
	}
	udpMan->Close();
	delete udpMan;
}

void elStateReceiver::setupConnection(unsigned short port)
{
	assert(! elStateReceiver::threadRunning);

    udpMan->Bind(port);
    udpMan->SetTimeoutReceive(1);
	threadRunning= true;
    thread = std::thread(&elStateReceiver::run, this);
}

void elStateReceiver::run()
{
    char recvBuf[1024];
    float relX, relY;
    int newState = -1;

    while (! terminateExec.load())
    {
        int retVal = udpMan->Receive(recvBuf, 1024);
        if (retVal != SOCKET_TIMEOUT && retVal != SOCKET_ERROR && (! terminateExec.load()))
        {
            switch(recvBuf[0])
            {
            case 'U':   //new user state
                memcpy(&newState, recvBuf + 1, 4);
                updateUserState((elUserState)newState);
                break;

            case 'I':   //new interaction state
                memcpy(&newState, recvBuf + 1, 4);
                updateInteractionState((elInteractionState)newState);
                break;

            case 'A':    //new interaction position
                memcpy(&relX, recvBuf + 1, 4);
                memcpy(&relY, recvBuf + 5, 4);
                updateInteractionPosition(relX, relY);
                break;

            case 'H':    //new head position
                memcpy(&relX, recvBuf + 1, 4);
                memcpy(&relY, recvBuf + 5, 4);
                updateHeadPosition(relX, relY);
                break;

            case 'S':   //new skeleton map
                parseSkeleton(recvBuf, retVal);
                updateSkeleton(sMap);
                break;

            case 'P':   //new projected skeleton map
                parseProjectedSkeleton(recvBuf, retVal);
                updateProjectedSkeleton(sPMap);
                break;

            default:
                break;

            }
        }
    }
}

void elStateReceiver::parseSkeleton(const char* recvBuf, int recvLen)
{
    int curPos = 1;
    bool goAhead = true;
    int intHelper = 0;
    elSkeletonMap::iterator iter = sMap.end();

    while (goAhead)
    {
        memcpy(&intHelper, recvBuf + curPos, 4); curPos += 4;

        if ((iter = sMap.find((elJointType)intHelper)) != sMap.end())
        {
            memcpy(&(iter->second.x), recvBuf + curPos, 4); curPos += 4;
            memcpy(&(iter->second.y), recvBuf + curPos, 4); curPos += 4;
            memcpy(&(iter->second.z), recvBuf + curPos, 4); curPos += 4;
        }
        else    //joint not found, skip. Should not happen as all joints are inited as default
        {
            printf("JOINT NOT FOUND IN MAP: %i\n", intHelper);
            curPos += 12;
        }

        //have we reached the end?
        if ((recvBuf[curPos] == 's') || ((curPos + 16) > recvLen))
        {
            goAhead = false;
        }
    }
}

void elStateReceiver::parseProjectedSkeleton(const char* recvBuf, int recvLen)
{
    int curPos = 1;
    bool goAhead = true;
    int intHelper = 0;
    elSkeletonMap::iterator iter = sPMap.end();

    while (goAhead)
    {
        memcpy(&intHelper, recvBuf + curPos, 4); curPos += 4;

        if ((iter = sPMap.find((elJointType)intHelper)) != sPMap.end())
        {
            memcpy(&(iter->second.x), recvBuf + curPos, 4); curPos += 4;
            memcpy(&(iter->second.y), recvBuf + curPos, 4); curPos += 4;
            memcpy(&(iter->second.z), recvBuf + curPos, 4); curPos += 4;
        }
        else    //joint not found, skip. Should not happen as all joints are inited as default
        {
            printf("JOINT NOT FOUND IN MAP: %i\n", intHelper);
            curPos += 12;
        }

        //have we reached the end?
        if ((recvBuf[curPos] == 'p') || ((curPos + 16) > recvLen))
        {
            goAhead = false;
        }
    }
}
void elStateReceiver::init()
{
	terminateExec= false;
    udpMan = new UDPManager();
	udpMan->Create();

    //init skeleton map
    for (int i = 0; i < EL_NUM_JOINTS; i++)
    {
        sMap.insert(std::pair<elJointType, elVec3f>((elJointType)i, elVec3f()));
        sPMap.insert(std::pair<elJointType, elVec3f>((elJointType)i, elVec3f()));
    }
}
