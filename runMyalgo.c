#include "runMyalgo.h"
#include "gnuplot_i.h"
#include <math.h>

	
int
runMyAlgoSimulation (const struct sensor_node network[])
{


  gnuplot_ctrl *g;
  struct sensor_node *network_myalgo;
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
  float confidence_value[NETWORK_SIZE];
  float temp_confidence_value = 0.0;
  int temp_number_of_members;
  int temp_var = 0;
  float temp_total_network_battery = 0.0;
  float total_network_battery[TOTAL_ROUNDS];
  int total_trans_nodes_array[TOTAL_ROUNDS];
  int recent_round = 1 / CH_RATIO;
  int temp_ch_id = 0;

int MEMBER_THRESHOLD = 0 ;
  int ch_count = 0;
  FILE *totalnetpower2;

  network_myalgo =
    (struct sensor_node *) malloc (NETWORK_SIZE * 2 *
				   sizeof (struct sensor_node));

  //initialise the leach network variables

  for (i = 0; i < NETWORK_SIZE; i++)
    {
      network_myalgo[i].currBattery = 3.0;
      network_myalgo[i].xLoc_new = network[i].xLoc_new;
      network_myalgo[i].xLoc_old = network[i].xLoc_old;
      network_myalgo[i].yLoc_new = network[i].xLoc_new;
      network_myalgo[i].yLoc_old = network[i].yLoc_old;
      network_myalgo[i].member_count = 0;
      network_myalgo[i].belongsTo = -1;
      network_myalgo[i].isHead = 0;
    }

  network_myalgo[-1].xLoc_new = base_station.xLoc_new;
  network_myalgo[-1].yLoc_new = base_station.yLoc_new;

 //initialise the confidence matrix first to all = 0
 
  for (i=0; i< NETWORK_SIZE; i++)
  {
	  confidence_value[i] = 0.0;
  }

  for (j = 0; j < TOTAL_ROUNDS; j++)
    {

      Tn = CH_RATIO / 1 - CH_RATIO * (round_count % 1 / CH_RATIO);

      //find out how many nodes can transmit in this current round

      for (i = 0; i < NETWORK_SIZE; i++)
	{
	  if (network_myalgo[i].isHead != -1)
	    temp_total_trans_nodes++;
	}
      total_trans_nodes_array[j] = temp_total_trans_nodes;
      temp_total_trans_nodes = 0;


      for (i = 0; i < NETWORK_SIZE; i++)
	{
	  if (network_myalgo[i].isHead != -1)
	    {
	      temp_total_network_battery += network_myalgo[i].currBattery;
//		 printf("%f \n", network_myalgo[i].currBattery);
	    }
	}

      total_network_battery[j] = temp_total_network_battery;
	 //printf("temp_total_trans_nodes = %f\n", temp_total_network_battery);
      temp_total_network_battery = 0.0;

      for (i = 0; i < NETWORK_SIZE; i++)
	{
	  if (network_myalgo[i].isHead == -1)
	    temp_total_deadnodes_this_round++;
	}
      total_deadnodes_this_round[j] = temp_total_deadnodes_this_round;
  //     printf("Total dead nodes in round %d is=%d\n", j, total_deadnodes_this_round[j]); 
      temp_total_deadnodes_this_round = 0;

      for (i = 0; i < NETWORK_SIZE; i++)
	{

	  if ((network_myalgo[i].current_round < (j - recent_round)) ||
	      (j - recent_round <= 0))
	    {
	      if (network_myalgo[i].isHead != -1)
		{

		  random = .00001 * (rand () % 100000);
            //printf("network_myalgo[%d].currBattery = %f and DFB = %f and J = %d\n", i, network_myalgo[i].currBattery, DEFAULT_B_POWER, j);

		  if (random < Tn)
		    {
		      //this node is now designated as a CH.
		      //change the isHead variable to 1.

		  if ((network_myalgo[i].currBattery >= 0.6*DEFAULT_B_POWER * (1 - j / TOTAL_ROUNDS))  )
			{
			  //printf("chosen head %d \n ", i);
			  network_myalgo[i].isHead = 1;

			  //We need to know the last round for which
			  //this node was a CH. Update the round
			  //variable.

			  network_myalgo[i].current_round = j;

			  //We need to find out how many CHs are
			  //there in a network. This has to be
			  //limited to CH_RATIO.
			  cluster_head_count++;
			}
		    }
		}
	    }
	}

      // Now we find the optimal MEMBER_THRESHOLD; For this we need to
      // find the total number of CHs for this round:


      // After deciding upon the clusters, each CH will broadcast this
      // info to other nodes. All other nodes keep their receivers ON
      // during this phase.


      for (i = 0; i < NETWORK_SIZE; i++)
	{
	  if (network_myalgo[i].isHead == 1)
	    {
//		    printf("network_myalgo[%d] is a CH : node ID = %d \n", i, i);
	      ch_count++;
	    }
	}

	 if (ch_count != 0)
	 {
		 MEMBER_THRESHOLD = NETWORK_SIZE/ch_count;
	 }
	 

      // Now the NCH's will decide which CH to join. 
if (ch_count == 0)
{
	//all nodes are NCH's. all network_myalgo[i].belongsTo == -1
	
//printf ("all nodes are NCH's.  round NUmber %d\n", j);
}
else
{
      for (k = 0; k < NETWORK_SIZE; k++)
	{
	  // check first if the node is a NCH or not.
	  temp_confidence_value = 0;
	  if (network_myalgo[k].isHead == 0)	//if NCH == TRUE
	    {
	      for (i = 0; i < NETWORK_SIZE; i++)
		{
		  /* if ((network_myalgo[i].isHead == 1 && network_myalgo[i].member_count < MEMBER_THRESHOLD))  */
		  // if it's CH
		  if ((network_myalgo[i].isHead == 1) && i != k )	// if it's CH
		    {
		      nodal_distance =
			sqrt (pow
			      ((network_myalgo
				[i].xLoc_new - network_myalgo[k].xLoc_new),
			       2) + pow ((network_myalgo[i].yLoc_new -
					  network_myalgo[k].yLoc_new), 2));	//find distance from CH to node.

		      // these two lines are NOT a part of LEACH. Use it in YOUR ALGO.

		      current_CH_battery = network_myalgo[i].currBattery;
			 if((network_myalgo[i].currBattery > 0.0) ) 
			 {
		      confidence_value[i] =
			findConfidenceValue (nodal_distance,
					     current_CH_battery,
					     network_myalgo[i].member_count, MEMBER_THRESHOLD);
		      /* confidence_value =   findConfidenceValueLeach (nodal_distance); */
			 }
			 else {
				 confidence_value[i] = 0.0;
			 }
		    }
		}

		 for (i = 0; i < NETWORK_SIZE; i++)
		 {
			 if(confidence_value[i] > temp_confidence_value)
			 {
				 temp_confidence_value = confidence_value[i];
				 temp_ch_id = i;
			 } 
		 }

		 for(i = 0; i < NETWORK_SIZE; i++)
		 {
			 confidence_value[i] = 0.0;
		 }

		 if(network_myalgo[temp_ch_id].isHead == 1)
		 {
			  network_myalgo[k].belongsTo = temp_ch_id;
			  network_myalgo[temp_ch_id].member_count++;
		 } 
	    }
		//	 printf("network_myalgo[%d].belongsTo = %d and CH_COUNT = %d and network_myalgo.currBattery = %f, round NUMBER %d\n", k, network_myalgo[k].belongsTo, ch_count, network_myalgo[k].currBattery, j );
	}	
}

if (ch_count == 0)
	{
//printf ("there are NO ch's selected for this round number : %d\n", j);
	  // do nothing
	}
      else
{
		 if(ch_count > 0)
			 
	{

	  for (i = 0; i < NETWORK_SIZE; i++)
	    {

	      if (network_myalgo[i].isHead == 1)	//this is a CH. You have your transmitters ON
		{
		  network_myalgo[i].currBattery =
		    network_myalgo[i].currBattery -
		    findTransmissionEnergy (CH_ADVERTISEMENT_RANGE,
					    CH_ADVERTISEMENT_BIT_LENGTH);
		}
	      else		//else you have your receivers ON.

		{

		  network_myalgo[i].currBattery =
		    network_myalgo[i].currBattery -
		    findReceivingEnergy (CH_ADVERTISEMENT_BIT_LENGTH);
		}
	    }
	}
}

if(ch_count == 1)
{ 
      for (i = 0; i < NETWORK_SIZE; i++)
	{
		
	  if (network_myalgo[i].isHead == 1)
	    {
	   //          printf("This node is a CH: node[%d] \n", i);
	      //       printf(" child node count = %d \n", network_myalgo[i].member_count);

	    }
	}
	
	 for (i = 0; i < NETWORK_SIZE; i++)
	 {
	 if (network_myalgo[i].isHead == 0)	
	    {
	     //              printf("This node [%d] belongs to CH ID %d\n", i,  network_myalgo[i].belongsTo);
	    }
	 }


}



// let us print out which sensor belongs to which CH

      // Schedule creation: We only need to simulate and plot the energy
      // dissipation during sending out/receiving the scheduling
      // information

if(ch_count == 0)
{
	//do nothing
	
}
else 
{
if (ch_count > 0)
{
      for (i = 0; i < NETWORK_SIZE; i++)
	{
	  if (network_myalgo[i].isHead == 1)	// if it IS a CH
	    {
	      network_myalgo[i].currBattery =
		network_myalgo[i].currBattery -
		findTransmissionEnergy (SCHEDULING_DISTANCE,
					SCHEDULE_MSG_BIT_LENGTH);
	    }
	  else if (network_myalgo[i].isHead == 0)
	    {
	      network_myalgo[i].currBattery =
		network_myalgo[i].currBattery -
		findReceivingEnergy (SCHEDULE_MSG_BIT_LENGTH);
	    }
	}
}
}
      // Data Transmission phase: from NCHs to their "respective" CHs.
if (ch_count == 0)
{
	// all transmission happens from NCHs to Basestation directly
for (i = 0; i < NETWORK_SIZE; i++)
{
if(network_myalgo[i].isHead != -1)	
{
	nodal_distance =
		sqrt (pow
				((network_myalgo
				  [i].xLoc_new - base_station.xLoc_new), 2) + pow ( (network_myalgo[i].yLoc_new - base_station.yLoc_new), 2));
	network_myalgo[i].currBattery =
		 network_myalgo[i].currBattery -
		 findTransmissionEnergy (nodal_distance, MESSAGE_LENGTH);
	if (network_myalgo[i].currBattery < 0.0)
	 network_myalgo[i].isHead = -1;
}
}
}

else
{
	if (ch_count > 0)
{
	for (i = 0; i < NETWORK_SIZE; i++)
	{
	
	  if (network_myalgo[i].isHead == 0)	// it is NOT a CH and is NOT a DEAD_NODE (for which isHead == -1)
	    {
	      nodal_distance =
		sqrt (pow
		      ((network_myalgo
			[i].xLoc_new -
			network_myalgo[network_myalgo[i].belongsTo].xLoc_new),
		       2) +
		      pow (
			   (network_myalgo[i].yLoc_new -
			    network_myalgo[network_myalgo[i].
					   belongsTo].yLoc_new), 2));
	      //this NCH looses battery which is == (given below)

	      network_myalgo[i].currBattery =
		network_myalgo[i].currBattery -
		findTransmissionEnergy (nodal_distance, MESSAGE_LENGTH);

	      //The corresponding CH looses battery == (given below)

	      network_myalgo[network_myalgo[i].belongsTo].currBattery =
		network_myalgo[network_myalgo[i].belongsTo].currBattery -
		findReceivingEnergy (MESSAGE_LENGTH);

	      // Now check if this node's energy has gone under the threshold
	      // limit

	      if (network_myalgo[i].currBattery < 0.0)
		//battery drained. isHead == -1.

		network_myalgo[i].isHead = -1;
	    }
	}

      // Data Transmission Phase II: from CH's to BASE_STATION

      for (i = 0; i < NETWORK_SIZE; i++)
	{
	  if (network_myalgo[i].isHead == 1)	//If this IS a CH
	    {
	      nodal_distance =
		sqrt (
		      (pow
		       ((network_myalgo[i].xLoc_new - base_station.xLoc_new),
			2)) + (pow ((network_myalgo[i].yLoc_new -
				     base_station.yLoc_new), 2)));

	      //(1)
	      network_myalgo[i].currBattery =
		network_myalgo[i].currBattery -
		findTransmissionEnergy (nodal_distance,
					MESSAGE_LENGTH *
					network_myalgo[i].member_count);

	      if (network_myalgo[i].currBattery > 0.0)
		{
		  bits_transmitted =
		    bits_transmitted +
		    (MESSAGE_LENGTH * network_myalgo[i].member_count);
		}
	      else
		{
		  network_myalgo[i].isHead = -1;

		  // if the currBattery is < 0.0 after sending
		  // MESSAGE_LENGTH*network_myalgo[i].member_count bits,
		  // that means that currBattery became = 0 at some
		  // point when .member_count was < the actual count. We
		  // need to find out HOW MANY bits were transferred
		  // successfully before the currBattery became < 0.0

		  // First allocate the original currBattery 

		  network_myalgo[i].currBattery =
		    network_myalgo[i].currBattery +
		    findTransmissionEnergy (nodal_distance,
					    MESSAGE_LENGTH *
					    network_myalgo[i].member_count);

		  temp_number_of_members = network_myalgo[i].member_count;

		  while (temp_var == 0)
		    {

		      if (
			  (network_myalgo[i].currBattery =
			   network_myalgo[i].currBattery -
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
}
}
// this round is done. free up variables:
for(i = 0; i < NETWORK_SIZE; i++)
{
		if (network_myalgo[i].currBattery < 0)
		{
			network_myalgo[i].isHead = -1;
		}
}

for (i = 0; i < NETWORK_SIZE; i++)
	{
	
		if (network_myalgo[i].isHead == 1)	// if it is a CH
	    {
	      network_myalgo[i].member_count = 0;
	      network_myalgo[i].isHead = 0;
	    }
	  else
	    {
	      if (network_myalgo[i].isHead == 0 || (network_myalgo[i].isHead == -1))	//if it is a NCH
		{
		  network_myalgo[i].belongsTo = -1;
		}
	    }
	}

      cluster_head_count1 = 0;
      cluster_head_count = 0;
      ch_count = 0;
	 temp_var = 0;
      round_count++;



      /* printf("total currBattery: %lf", total_network_battery[j]); */

      // Find out how many nodes are DEAD after each round.


    }



  // Open a file and input the values of the variables in there for
  //total_deadnodes_this_round and total_network_battery 

  remove ("Results/totalnetpower2.txt");

  if (!(totalnetpower2 = fopen ("Results/totalnetpower2.txt", "a")))
    {
      printf ("cannot open Results/totalnetpower2.txt");
      exit (0);
    }
  for (i = 0; i < TOTAL_ROUNDS; i++)
    {
      /* printf ("runMYALGOSimulation final  number: %d \t %d\n", i,TOTAL_ROUNDS); */
      fprintf (totalnetpower2, "%d \t %f \t %d \t %d \n",
	       total_deadnodes_this_round[i], total_network_battery[i],
	       total_trans_nodes_array[i], i);
    }

  fclose (totalnetpower2);

  free (network_myalgo);
// Let us plot both the functions together: the 

  g = gnuplot_init ();
  gnuplot_set_xlabel (g, "Number of rounds");

  gnuplot_cmd (g, "set ylabel \"Total Network Energy \" ");


  /*gnuplot_cmd (g,
	       "plot \"Results/totalnetpower.txt\" using 4:1 title \'Total Dead Nodes/round\' smooth csplines with lines , \"Results/totalnetpower.txt\" using 4:2 title  \'Total Network Battery\'  smooth csplines with lines , \"Results/totalnetpower.txt\" using 4:3 ti \'Total Nodes transmitting\'  smooth csplines with lines, \"Results/totalnetpower2.txt\" using 4:1 title \'TDNodes - Myalgo\' smooth csplines with lines , \"Results/totalnetpower2.txt\" using 4:2 ti \'Tot_Nte_bat-myalgo\' smooth csplines with lines , \"Results/totalnetpower2.txt\" using 4:3 ti \'Tot_Nodes_trans-myalgo\' smooth csplines with lines ");

*/

  gnuplot_cmd (g,
	       "plot \"Results/totalnetpower.txt\" using 4:2 title  \'Total Network Battery - LEACH\'  smooth csplines with lines , \"Results/totalnetpower2.txt\" using 4:2 ti \'Total Network Battery - My Algo\' smooth csplines with lines ");
  sleep (15);

  gnuplot_cmd (g, "set ylabel \"Total Dead Nodes/Round\" ");
  gnuplot_cmd (g, 
		  "plot \"Results/totalnetpower.txt\" using 4:1 title \'Total Dead Nodes/round - LEACH\' smooth csplines with lines , \"Results/totalnetpower2.txt\" using 4:1 title \'Total Dead Nodes/round - My Algo\' smooth csplines with lines ");

sleep (10);

  gnuplot_cmd (g, "set ylabel \"Total Transmitting Nodes/Round\" ");
  gnuplot_cmd (g, 
		  "plot  \"Results/totalnetpower.txt\" using 4:3 ti \'Total Nodes transmitting/Round - LEACH\' smooth csplines with lines, \"Results/totalnetpower2.txt\" using 4:3 ti \'Total Nodes Transmitting/Round - My Algo\' smooth csplines with lines ");

sleep (10);

gnuplot_cmd (g, "set terminal postscript color");

gnuplot_cmd (g, "set ylabel \"Total Network Energy \" ");
gnuplot_cmd (g, "set output \"Results/TotalNetBattery_ComparativeGraph.ps\"");
gnuplot_cmd (g, "plot \"Results/totalnetpower.txt\" using 4:2 title  \'Total Network Battery\'  smooth csplines with lines , \"Results/totalnetpower2.txt\" using 4:2 ti \'Tot_Nte_bat-myalgo\' smooth csplines with lines ");

 gnuplot_cmd (g, "set ylabel \"Total Dead Nodes/Round\" ");
gnuplot_cmd (g, "set output \"Results/TotalDeadNodes_ComparativeGraph.ps\"");
gnuplot_cmd (g,"plot \"Results/totalnetpower.txt\" using 4:1 title \'Total Dead Nodes/round - LEACH\' smooth csplines with lines , \"Results/totalnetpower2.txt\" using 4:1 title \'Total Dead Nodes/round - My Algo\' smooth csplines with lines ");

gnuplot_cmd (g, "set ylabel \"Total Transmitting Nodes/Round\" ");
gnuplot_cmd (g, "set output \"Results/TotalTransmittingNodes_ComparativeGraph.ps\"");
gnuplot_cmd (g,"plot  \"Results/totalnetpower.txt\" using 4:3 ti \'Total Nodes transmitting/Round - LEACH\' smooth csplines with lines, \"Results/totalnetpower2.txt\" using 4:3 ti \'Total Nodes Transmitting/Round - My Algo\' smooth csplines with lines ");

gnuplot_close (g);

printf("You can also view the Resulting Graphs by typing : \n\n");
printf("\t\t\n");
printf("\t\t (a): gv Results/TotalNetBattery_ComparativeGraph.ps for a comparative study of the Total Network battery remaining after each successive rounds \n");
printf("\t\t (b): gv Results/TotalDeadNodes_ComparativeGraph.ps for a comparative study of the Total Dead Nodes remaining after each successive rounds \n");
printf("\t\t (c): gv Results/TotalTransmittingNodes_ComparativeGraph.ps for a comparative study of the Total Transmitting Nodes remaining after each successive rounds \n\n\n");

  return(0);
}

