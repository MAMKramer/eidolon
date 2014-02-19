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

#include "elStateSender.h"
#include "UDPManager.h"

elStateSender::elStateSender() : udpMan(0)
{

}

elStateSender::~elStateSender()
{
    closeConnection();
}

void elStateSender::setupConnection(const char* host, unsigned short port)
{
    closeConnection();
    udpMan = new UDPManager();
    udpMan->Create();
    udpMan->Connect(host, port);

    printf("HMSTATESENDER: Sending to %s on port %u\n", host, port);
}


void elStateSender::closeConnection()
{
    if (udpMan)
    {
        udpMan->Close();
        delete udpMan;
    }
}

bool elStateSender::newHeadPosition(float relX, float relY)
{
    if (!udpMan)
        return false;

    char sndBuf[10];
    sndBuf[0] = 'H';
    memcpy(sndBuf + 1, &relX, 4);
    memcpy(sndBuf + 5, &relY, 4);
    sndBuf[9] = 'h';

    udpMan->SendAll(sndBuf, 10);
    return true;
}

bool elStateSender::newInteractionPosition(float relX, float relY)
{
    if (!udpMan)
        return false;

    char sndBuf[10];
    sndBuf[0] = 'A';
    memcpy(sndBuf + 1, &relX, 4);
    memcpy(sndBuf + 5, &relY, 4);
    sndBuf[9] = 'a';

    udpMan->SendAll(sndBuf, 10);
    return true;
}

bool elStateSender::newUserState(elUserState state)
{
    if (!udpMan)
        return false;

    char sndBuf[6];
    sndBuf[0] = 'U';
    memcpy(sndBuf + 1, &state, 4);
    sndBuf[5] = 'u';

    udpMan->SendAll(sndBuf, 6);
    return true;
}

bool elStateSender::newInteractionState(elInteractionState state)
{
    if (!udpMan)
        return false;

    char sndBuf[6];
    sndBuf[0] = 'I';
    memcpy(sndBuf + 1, &state, 4);
    sndBuf[5] = 'i';

    udpMan->SendAll(sndBuf, 6);
    return true;
}

bool elStateSender::newSkeletonMap(const elSkeletonMap& map)
{
    if (!udpMan)
    {
        return false;
    }

    char sndBuf[512];
    int numBytes = 0;
    sndBuf[0] = 'S'; numBytes++;

    for (elSkeletonMap::const_iterator iter = map.begin(); iter != map.end(); ++iter)
    {
        //copy the enum
        memcpy(sndBuf + numBytes, &(iter->first), 4); numBytes += 4;

        //now copy the vector
        memcpy(sndBuf + numBytes, &(iter->second.x), 4); numBytes += 4;
        memcpy(sndBuf + numBytes, &(iter->second.y), 4); numBytes += 4;
        memcpy(sndBuf + numBytes, &(iter->second.z), 4); numBytes += 4;
    }
    sndBuf[numBytes] = 's'; numBytes++;

    udpMan->SendAll(sndBuf, numBytes);
    return true;
}

bool elStateSender::newProjectedSkeletonMap(const elSkeletonMap& map)
{
    if (!udpMan)
    {
        return false;
    }

    char sndBuf[512];
    int numBytes = 0;
    sndBuf[0] = 'P'; numBytes++;

    for (elSkeletonMap::const_iterator iter = map.begin(); iter != map.end(); ++iter)
    {
        //copy the enum
        memcpy(sndBuf + numBytes, &(iter->first), 4); numBytes += 4;

        //now copy the vector
        memcpy(sndBuf + numBytes, &(iter->second.x), 4); numBytes += 4;
        memcpy(sndBuf + numBytes, &(iter->second.y), 4); numBytes += 4;
        memcpy(sndBuf + numBytes, &(iter->second.z), 4); numBytes += 4;
    }
    sndBuf[numBytes] = 'p'; numBytes++;

    udpMan->SendAll(sndBuf, numBytes);
    return true;
}
