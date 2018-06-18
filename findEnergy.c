#include "findEnergy.h"

/* For the next two function definitions, Ref: Paper on LEACH.  */

float
findTransmissionEnergy (double distance_between_nodes, int messageBitLength)
{

  float Eelec = 50 * pow (10, -9);

  // radio dissipates 50nJ/bit to run a transmitter
  // or a receiver circuitary. 

  float Epsilonamp = 100 * pow (10, -12);

  // radio needed Epsilonamp J/bit/m^2 for the transmit amplifier to achieve an
  // acceptable E_b/N_o. 


  //These values are taken directly from the LEACH
  //paper.

  float TransmitEnergyUsed = 0.0;

  return (TransmitEnergyUsed =
	  messageBitLength * Eelec +
	  Epsilonamp * messageBitLength * pow (distance_between_nodes, 2));
}

float
findReceivingEnergy (int messageBitLength)
{
  float Eelec = 50 * pow (10, -9);	// radio dissipates 50nJ/bit to run a transmitter
  // or a receiver circuitary.

  float ReceivingEnergyUsed = 0.0;
  return (ReceivingEnergyUsed = Eelec * messageBitLength);
}
