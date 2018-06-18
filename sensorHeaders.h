#ifndef __SENSORHEADERS_H
#define __SENSORHEADERS_H

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>      //especially needed for srand() and rand()
#include <time.h>
#include <ctype.h>
#include <math.h>


#define DEFAULT_SIZE  400
#define DEFAULT_X_COORD 300
#define DEFAULT_Y_COORD 300
#define DEFAULT_B_POWER 3.0
#define DEFAULT_CH_RATIO 0.05
#define DEFAULT_ROUNDS 500
#define SCHEDULING_DISTANCE 10
#define SCHEDULE_MSG_BIT_LENGTH 64
#define CH_ADVERTISEMENT_RANGE 10
#define CH_ADVERTISEMENT_BIT_LENGTH 64

//int DEFAULT_CH_POWER = STARTING_BATTERY_POWER*3;

#define POWER_THRESHOLD_CH 0.0

// This is the total power of the whole network
#define POWER_THRESHOLD_NETWORK 0.0

// default length of a broadcast message - to be changed/modified
#define MESSAGE_LENGTH 8192

struct sensor_node
{

    long xLoc_old;		// the x-location for previous trace file

    long yLoc_old;
    long xLoc_new;
    // if the new trace file has a new x or y
    // location (i.e. xLoc-old != xLoc-new)

    long yLoc_new;

// that means the sensor has moved. This should start a new routine for sending
// out new broadcasts to find new child nodes (if the
// moving node is a CH, or for finding a new CH (if the
// node is a NCH)


//We are now using a method where all battery power is equal initially.
//CH's do NOT have additional higher battery/communication powers.

    float initBattery;


    float currBattery;

// 1 if this node is a CH, otherwise 0. If it is = -1, it is a DEADNODE
    int isHead;

// is initially = -1. Once cluster setup phase is initiated, belongsTo ==
// ID of the CH it belongs to.

    int belongsTo;

    int member_count;

//How many maximum number of nodes can the CHs take. 
//  int member_threshold;

    int chID;

// ID of the CH for the node if it is not a CH.

    int current_round;
    float tPeriod;

//periods for which node has been transmitting

    float tPower;		// power used per transmission
    float rPeriod;		// periods for which node was receiving
    float rPower;		// power used per receiving session.
    char chPointer;		// used to point to the CH for a NCH

};

struct sensor_node base_station;

#endif /* __SENSORHEADERS_H */
