#include "runLeach.h"
#include "gnuplot_i.h"
#include <math.h>

int runLeachSimulation (const struct sensor_node network[])
{


  gnuplot_ctrl *h;
  struct sensor_node *network_leach;
  int bits_transmitted;
  int i, j, k;
  double Tn;
  float random;
  int round_count = 0;
  int cluster_head_count = 0;
  int cluster_head_count1 = 0;
  int temp_total_trans_nodes = 0;
  int total_deadnodes_this_round[TOTAL_ROUNDS];
  int temp_total_deadnodes_this_round = 0;
  float nodal_distance;
  float current_CH_battery;
  float confidence_value;
  float temp_confidence_value = 0.0;
  int temp_number_of_members;
  int temp_var = 0;
  float temp_total_network_battery = 0.0;
  float total_network_battery[TOTAL_ROUNDS];
  int total_trans_nodes_array[TOTAL_ROUNDS];
  int recent_round = 1 / CH_RATIO;
  FILE *totalnetpower;
  int ch_count = 0;
  network_leach =
    (struct sensor_node *) malloc (NETWORK_SIZE * 2 *
				   sizeof (struct sensor_node));

  //initialise the leach network variables

  for (i = 0; i < NETWORK_SIZE; i++)
    {
      network_leach[i].currBattery = network[i].currBattery;
      network_leach[i].xLoc_new = network[i].xLoc_new;
      network_leach[i].xLoc_old = network[i].xLoc_old;
      network_leach[i].yLoc_new = network[i].xLoc_new;
      network_leach[i].yLoc_old = network[i].yLoc_old;
      network_leach[i].member_count = 0;
      network_leach[i].isHead = 0;
      network_leach[i].belongsTo = -1;

    }

  network_leach[-1].xLoc_new = base_station.xLoc_new;
  network_leach[-1].yLoc_new = base_station.yLoc_new;

  for (j = 0; j < TOTAL_ROUNDS; j++)
    {
      Tn = CH_RATIO / 1 - CH_RATIO * (round_count % 1 / CH_RATIO);

      //find out how many nodes can transmit in this current round

      for (i = 0; i < NETWORK_SIZE; i++)
	{
		
	  if (network_leach[i].isHead != -1)
	    temp_total_trans_nodes++;
	}
      total_trans_nodes_array[j] = temp_total_trans_nodes;
//	 printf("temp_total_trans_nodes = %d \n", temp_total_trans_nodes);

      temp_total_trans_nodes = 0;	//re-initialize this variable for next round.

      for (i = 0; i < NETWORK_SIZE; i++)
	{
	  if (network_leach[i].isHead == -1)
	    temp_total_deadnodes_this_round++;
	}
      total_deadnodes_this_round[j] = temp_total_deadnodes_this_round;
      /* printf("Total dead nodes in round %d is=%d\n", j, total_deadnodes_this_round[j]); */
      temp_total_deadnodes_this_round = 0;


      for (i = 0; i < NETWORK_SIZE; i++)
	{
	  if (network_leach[i].isHead != -1)
	    {
	      temp_total_network_battery += network_leach[i].currBattery;
//		 printf("%f , %i \n", temp_total_network_battery, NETWORK_SIZE);
	    }
	}
      total_network_battery[j] = temp_total_network_battery;
      temp_total_network_battery = 0.0;


      for (i = 0; i < NETWORK_SIZE; i++)
	{

	  if ((network_leach[i].current_round < (j - recent_round)) ||
	      (j - recent_round <= 0))
	    {
	      if (network_leach[i].isHead != -1)
		{

		  random = .00001 * (rand () % 100000);

		  if (random < Tn)
		    {
		      //this node is now designated as a CH.
		      //change the isHead variable to 1.
		      network_leach[i].isHead = 1;

		      //We need to know the last round for which
		      //this node was a CH. Update the round
		      //variable.

		      network_leach[i].current_round = j;

		      //We need to find out how many CHs are
		      //there in a network. This has to be
		      //limited to CH_RATIO.

		      cluster_head_count++;
		    }
		}
	    }
	}


      // After deciding upon the clusters, each CH will broadcast this
      // info to other nodes. All other nodes keep their receivers ON
      // during this phase.


      for (i = 0; i < NETWORK_SIZE; i++)
	{
	  if (network_leach[i].isHead == 1)	//this is a CH. You have your transmitters ON
	    {
	      network_leach[i].currBattery =
		network_leach[i].currBattery -
		findTransmissionEnergy (CH_ADVERTISEMENT_RANGE,
					CH_ADVERTISEMENT_BIT_LENGTH);
	    }
	  else			//else you have your receivers ON.

	    {
		    if(network_leach[i].isHead != -1)
		    {
	    network_leach[i].currBattery =
		network_leach[i].currBattery -
		findReceivingEnergy (CH_ADVERTISEMENT_BIT_LENGTH);
		    } 
	    }	
	}
      // Now the NCH's will decide which CH to join. 

      for (k = 0; k < NETWORK_SIZE; k++)
	{

	  // check first if the node is a NCH or not.

	  temp_confidence_value = 0;

	  if (network_leach[k].isHead == 0)
	    {
	      for (i = 0; i < NETWORK_SIZE; i++)
		{
		  if (network_leach[i].isHead == 1)
		    {
		      nodal_distance =
			sqrt (pow
			      ((network_leach
				[i].xLoc_new - network_leach[k].xLoc_new),
			       2) + pow ((network_leach[i].yLoc_new -
					  network_leach[k].yLoc_new), 2));	//find distance from CH to node.

		      // these two lines are NOT a part of LEACH. Use it in YOUR ALGO.

		      current_CH_battery = network_leach[i].currBattery;
		      confidence_value =
			findConfidenceValueLeach (nodal_distance);
		      if (confidence_value > temp_confidence_value)
			{

			  // This is the CH ID this NCH belongs to.

			  network_leach[k].belongsTo = i;
			  network_leach[i].member_count++;
			  temp_confidence_value = confidence_value;
			}
		    }
		}

	      //Increment the Member count of the CH to which this NCH
	      //was allocated. 
	      //
	      //For MY ALGO, we need to check if this value is ever
	      //crossing the threshold.
	    }

	}

      // Schedule creation: We only need to simulate and plot the energy
      // dissipation during sending out/receiving the scheduling
      // information

      for (i = 0; i < NETWORK_SIZE; i++)
	{
	  if (network_leach[i].isHead == 1)	// if it IS a CH
	    {
	      network_leach[i].currBattery =
		network_leach[i].currBattery -
		findTransmissionEnergy (SCHEDULING_DISTANCE,
					SCHEDULE_MSG_BIT_LENGTH);
	    }
	  else
	    {
	      network_leach[i].currBattery =
		network_leach[i].currBattery -
		findReceivingEnergy (SCHEDULE_MSG_BIT_LENGTH);
	    }
	}


      // Data Transmission phase: from NCHs to their "respective" CHs.

      for (i = 0; i < NETWORK_SIZE; i++)
	{
	  if (network_leach[i].isHead == 0)	// it is NOT a CH and is NOT a DEAD_NODE (for which isHead == -1)
	    {
	      nodal_distance =
		sqrt (pow
		      ((network_leach
			[i].xLoc_new -
			network_leach[network_leach[i].belongsTo].xLoc_new),
		       2) +
		      pow (
			   (network_leach[i].yLoc_new -
			    network_leach[network_leach[i].
					  belongsTo].yLoc_new), 2));

	      //this NCH looses battery which is == (given below)

	      network_leach[i].currBattery =
		network_leach[i].currBattery -
		findTransmissionEnergy (nodal_distance, MESSAGE_LENGTH);

	      //The corresponding CH looses battery == (given below)

	      network_leach[network_leach[i].belongsTo].currBattery =
		network_leach[network_leach[i].belongsTo].currBattery -
		findReceivingEnergy (MESSAGE_LENGTH);

	      // Now check if this node's energy has gone under the threshold
	      // limit

	      if (network_leach[i].currBattery < 0.000)
		//battery drained. isHead == -1.

		network_leach[i].isHead = -1;
	    }
	}

      // Data Transmission Phase II: from CH's to BASE_STATION

      for (i = 0; i < NETWORK_SIZE; i++)
	{
	  if (network_leach[i].isHead == 1)	//If this IS a CH
	    {
	      nodal_distance =
		sqrt (
		      (pow
		       ((network_leach[i].xLoc_new - base_station.xLoc_new),
			2)) + (pow ((network_leach[i].yLoc_new -
				     base_station.yLoc_new), 2)));

	      //(1)
	      network_leach[i].currBattery =
		network_leach[i].currBattery -
		findTransmissionEnergy (nodal_distance,
					MESSAGE_LENGTH *
					network_leach[i].member_count);

	      if (network_leach[i].currBattery > 0.0)
		{
		  bits_transmitted =
		    bits_transmitted +
		    (MESSAGE_LENGTH * network_leach[i].member_count);
		}
	      else
		{
		  network_leach[i].isHead = -1;

		  // if the currBattery is < 0.0 after sending
		  // MESSAGE_LENGTH*network_leach[i].member_count bits,
		  // that means that currBattery became = 0 at some
		  // point when .member_count was < the actual count. We
		  // need to find out HOW MANY bits were transferred
		  // successfully before the currBattery became < 0.0

		  // First allocate the original currBattery 

		  network_leach[i].currBattery =
		    network_leach[i].currBattery +
		    findTransmissionEnergy (nodal_distance,
					    MESSAGE_LENGTH *
					    network_leach[i].member_count);

		  temp_number_of_members = network_leach[i].member_count;

		  while (temp_var == 0)
		    {

		      if (
			  (network_leach[i].currBattery =
			   network_leach[i].currBattery -
			   findTransmissionEnergy (nodal_distance,
						   MESSAGE_LENGTH *
						   temp_number_of_members)) >
			  0)
			{
			  bits_transmitted +=
			    MESSAGE_LENGTH * temp_number_of_members;
			  temp_var = 1;
			}
		      else
			{
			  temp_number_of_members--;
			}
		      if (temp_number_of_members == 0)
			temp_var = 1;
		    }
		}
	    }
	}
// this round is done. free up variables:

      for (i = 0; i < NETWORK_SIZE; i++)
	{
	  if (network_leach[i].isHead == 1)	// if it is a CH
	    {
	      network_leach[i].member_count = 0;
	      network_leach[i].isHead = 0;
	    }
	  else
	    {
	      if (network_leach[i].isHead == 0)	//if it is a NCH
		{
		  network_leach[i].belongsTo = -1;
		}
	    }
	}
      cluster_head_count = 0;
      cluster_head_count1 = 0;
	 temp_var = 0;
	 ch_count = 0;
      round_count++;


      /* printf("total currBattery: %lf", total_network_battery[j]); */

      // Find out how many nodes are DEAD after each round.


// Find out the totalnetwork battery after each round is finished.
//
    }


  // Open a file and input the values of the variables in there for
  //total_deadnodes_this_round and total_network_battery 

//printf("runLEACHSimulation final  number: %d", round_count);


  remove ("Results/totalnetpower.txt");

  if (!(totalnetpower = fopen ("Results/totalnetpower.txt", "a")))
    {
      printf ("cannot open Results/totalnetpower.txt");
      exit (0);
    }
  for (i = 0; i < TOTAL_ROUNDS; i++)
    {
      /* printf ("runLEACHSimulation final  number: %d \t %d\n", i,     TOTAL_ROUNDS); */
      fprintf (totalnetpower, "%d \t %f \t %d \t %d \n",
	       total_deadnodes_this_round[i], total_network_battery[i],
	       total_trans_nodes_array[i], i);
    }
  fclose (totalnetpower);
  free (network_leach);
  // Let us plot both the functions together: the 

  h = gnuplot_init ();
  gnuplot_set_xlabel (h, "Number of rounds");
  gnuplot_cmd (h, "set ylabel \"Total Network Energy - OR - Total nodes transmitting\"");
 // gnuplot_cmd (h,
//	       "plot \"Results/totalnetpower.txt\" using 4:1 title \"Total Dead Nodes/round - Using LEACH\" smooth csplines with lines , \"Results/totalnetpower.txt\" using 4:2 smooth csplines ti \"Total Network Battery - Using LEACH\" with lines , \"Results/totalnetpower.txt\" using 4:3 smooth csplines ti \"Total Nodes transmitting - Using LEACH\"  with lines");


//  sleep (5);
  gnuplot_close (h);

  return(0);
}



