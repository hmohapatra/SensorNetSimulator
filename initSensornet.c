#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>		//especially needed for srand() and rand()
#include <time.h>
#include <ctype.h>
#include <math.h>
#include "gnuplot_i.h"
#include "sensorHeaders.h"



void plotInitialNetwork (int choice);
int numberOfNodes (FILE * node);
void spread_sensor (void);
struct sensor_node *sensorConfig (char *config_file);
float findTransmissionEnergy (double distance_between_nodes,
			      int messageBitLength);
float findReceivingEnergy (int messageBitLength);
float findConfidenceValue (float distance, float battery_power,
			   int member_count, int member_threshold);
float findConfidenceValueLeach (float distance);
void plotMyAlgoNetwork ();
void plotLeachNetwork ();
int runLeachSimulation (const struct sensor_node network[]);
int runMyAlgoSimulation (const struct sensor_node network_m[]);


int NETWORK_SIZE = DEFAULT_SIZE;
int START_X_COORD = DEFAULT_X_COORD;
int START_Y_COORD = DEFAULT_Y_COORD;
int STARTING_BATTERY_POWER = DEFAULT_B_POWER;
double CH_RATIO = DEFAULT_CH_RATIO;
int TOTAL_ROUNDS = DEFAULT_ROUNDS;

int 
main (int argc, char *argv[])
{
  FILE *nodeinfo;
  // this is the file from which ALL nodes
  // will gather their information - this file is opened
  // first.
  
  struct sensor_node *network;
  //covers the CH's and NCH's.

  extern struct sensor_node base_station;

  //define BASE STATION in the beginning. Make it global.
  //contains details of the Base-station. All the CHs collect data and send it to the BS.

  int init_completed = 0;
  int node_count = 0;
  int i, config_needed = 0;
  char curr_line[100];
  char *temp;
  //char answer;

//variables that tell us about how many rounds of data transmissions
//occured in LEACH and MYalgo simulations before the network energy became
//less than the threshold energy.



  base_station.xLoc_new = 0.0;
  base_station.yLoc_new = 0.0;


  for (i = 0; i < argc; i++)
    {
      if ((argv[i][0] == '-') && (argv[i][1] == 'f'))
	{
	  config_needed = i;
	}
    }

  for (i=0; i< argc; i++)
  {
	  if ((argv[i][0] == '-') && ((argv[i][1] == 'h') || argv[i][1] != 'f') )
	  {
		  printf("\n\n./simulator \n \t\t[to run with default configs (taken from .config)] \n \n\n");
		  printf("./simulator -f <alt-configfile> \n \t\t [to run with alternate config file]\n\n \n");
		  exit(0);
	  }

  }
  
 
			  
  if (config_needed == 0)
    {
      // no command line config file was found. Use .config

      network = sensorConfig (".config");
      //printf ("using .config\n");
    }
  else
    {
      network = sensorConfig (argv[config_needed + 1]);
 //     printf ("\nUsing the config file: %s\n", argv[config_needed + 1]);
      //printf ("NETWORKsize %d\n", NETWORK_SIZE);
    }

/* First we call the spread_sensor () function to populate the
 * nodeinfo.txt file which contains all the node locations and information
 * about which nodes are acting as cluster Heads. This function is called
 * JUST ONCE in the whole program. Users are free to open the nodeinfo.txt
 * file and change some node locations to simulate mobility of nodes. */

  
  spread_sensor ();

  if (!(nodeinfo = fopen ("Initialize/nodeinfo.txt", "r")))
    {
      printf ("Could not open file Initialize/nodeinfo.txt:");
      exit (1);
    }

  while (!init_completed)
    {

      temp = fgets (curr_line, 100, nodeinfo);
      if (temp == NULL)
	{
	  init_completed = 1;
	}
      else
	{

	  sscanf (curr_line, "%ld\t%ld", &network[node_count].xLoc_new, &network[node_count].yLoc_new);

	  // We are not using static CH's now. Use a randomised CH algo as in
	  // LEACH and for every ROUND, choose a different CH. Both the CH and
	  // the NCH have the SAME BATTERY POWER.

	  network[node_count].isHead = 0;	//initialize all nodes to 0 first.
	  network[node_count].xLoc_old = network[node_count].xLoc_new;
	  network[node_count].yLoc_old = network[node_count].yLoc_new;
	  network[node_count].current_round = 0;
	  network[node_count].rPeriod = 0.0;
	  network[node_count].tPeriod = 0.0;
	  network[node_count].belongsTo = -1;
	  network[node_count].currBattery = 3.0;
	  node_count++;
	}
    }

  fclose (nodeinfo);
  //printf
  //  ("Would you like to see a graph of your sensor node distribution? (y/n)");
  //answer = getchar ();
  /*putchar ('\n');
  if (answer == 'y')
    {
      //plotInitialNetwork (0);
    }
  else
    {
      //plotInitialNetwork (1);
    }
*/
  // Now we first run a LEACH simulation and find out how many rounds the
  // network lives for: also, graphically generate the final power of the
  // network after each round. We intend to compare this with the final
  // graph resulted from my algorithm. 


  runLeachSimulation (network);
  runMyAlgoSimulation (network);
  //runDirectSimulation (network);

  return(0);
}

// Function definitions start here:

void
plotInitialNetwork (int choice)
{
  if (choice == 0)		// user wants to see graphical data live.
    {
      gnuplot_ctrl *g;
      g = gnuplot_init ();
      gnuplot_set_xlabel (g, "Sensor X Coordinates");
	 gnuplot_cmd (g, "set ylabel \"Sensor Y Coordinates\"");
	 
      /* gnuplot_cmd (g, "set grid"); */
      gnuplot_cmd (g, "set pointsize 3");
      gnuplot_cmd (g,
		   "plot \"Initialize/nodeinfo.txt\" using 1:2 title \'.  - Sensor Nodes\' with dots, \"Initialize/nodeinfo.txt\" using ($1*$3):($2*$3) title \' X - Cluster Heads\' with points");
      gnuplot_cmd (g, "set pointsize 1");

      sleep (10);

      gnuplot_cmd (g, "set grid");
      gnuplot_cmd (g, "set pointsize 3");
      gnuplot_cmd (g, "set terminal postscript color");
      gnuplot_cmd (g, "set output \"Initialize/nodeinfo.ps\"");
      /* gnuplot_cmd (g, "set nokey"); */
      gnuplot_cmd (g,
		   "plot \"Initialize/nodeinfo.txt\" using 1:2 title \'.  - Sensor Nodes\' with dots, \"Initialize/nodeinfo.txt\" using ($1*$3):($2*$3) title \' X - Cluster Heads\' with points");


	 gnuplot_close(g);
      printf("\n You can also view the file by typing \'gv Initialize/nodeinfo.ps\' on the command prompt \n\n");
    }

  else if (choice == 1)
    {
      gnuplot_ctrl *g;
      g = gnuplot_init ();
      gnuplot_set_xlabel (g, "sensor x coord");
      gnuplot_set_ylabel (g, "sensor y coord");
      gnuplot_cmd (g, "set grid");
      gnuplot_cmd (g, "set terminal postscript color");
      gnuplot_cmd (g, "set output \"Initialize/nodeinfo.ps\"");
      gnuplot_cmd (g, "set nokey");
      gnuplot_cmd (g, "set pointsize 3");
      gnuplot_cmd (g,
		   "plot \"Initialize/nodeinfo.txt\" using 1:2 with dots, \"Initialize/nodeinfo.txt\" using ($1*$3):($2*$3) with points");

      printf ("Your graph has been saved to Initialize/nodeinfo.ps\n\n");
	 gnuplot_close(g);
    }
}

