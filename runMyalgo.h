#include "sensorHeaders.h"
#include "globalVars.h"

float findTransmissionEnergy (double distance_between_nodes,
		                     int messageBitLength);
float findReceivingEnergy (int messageBitLength);

float findConfidenceValue (float distance, float battery_power,
			   int member_count, int member_threshold);
