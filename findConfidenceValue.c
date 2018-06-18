#include "findConfidenceValue.h"

float
findConfidenceValue (float distance, float battery_power, int member_count, int MEMBER_THRESHOLD)
{
	  if (member_count > 5*MEMBER_THRESHOLD)
		      {
				       return (0);
					      }
	    else
		        {
				         /* return ( (distance *  battery_power) / (
						* distance + 2 * battery_power)); */
				         return ((1 / (member_count * distance + battery_power)));
					        }
}

float
findConfidenceValueLeach (float distance)
{
	  return (1 / distance);
}


