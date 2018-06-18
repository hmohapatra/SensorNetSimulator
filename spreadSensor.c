#include "spreadSensor.h"

void
spread_sensor (void)
{

  FILE *sensordist;
  int networksize = NETWORK_SIZE, temp = 1, j, CH_count = 0;
  int array[3 * networksize + 4];

// to populate/seed the rand() function call later in the program.

  srand (time (NULL));

  remove ("Initialize/nodeinfo.txt");
  if (!(sensordist = fopen ("Initialize/nodeinfo.txt", "a")))
    {
      printf ("cannot open file Initialize/nodeinfo.txt for writing/appending");
      exit (0);
    }
  while (temp < ((3 * networksize) + 1))
    {
      if (temp % 3 == 0)
	{
	  array[temp] = rand () % 2;
	  if (temp % 5 == 0)
	    {
	      if (array[temp] == 1)	// it IS a CH
		{
		  CH_count++;
		  if (((double) CH_count / (double) networksize) > DEFAULT_CH_RATIO)	// if it IS >, we can NOT have more CHs
		    {
		      array[temp] = 0;
		    }
		}
	    }
	  else
	    {
	      array[temp] = 0;
	    }

	}
      else
	{
	  array[temp] = rand () % networksize;
	}
      temp = temp + 1;
    }

//this is where we write this stuff into a file:
  for (j = 1; j < (3 * networksize) + 1; j++)
    {
      if (j % 3 == 0)
	{
	  fprintf (sensordist, "%d\n", array[j]);
	}
      else
	{
	  fprintf (sensordist, "%d\t", array[j]);
	}
    }
  fclose (sensordist);

}
