#include "sensorConfig.h"
	
struct sensor_node *
sensorConfig (char *config_file)
{


  char buffer[100], cut1[9], cut2[9], copy[100];
  int i = 0;
  FILE *fd;
  if ((fd = fopen (config_file, "r")) == NULL)
    {
      //printf ("No config file found: using default configs\n");
      return ((struct sensor_node *)
	      malloc (DEFAULT_SIZE * sizeof (struct sensor_node)));

    }
//  printf ("\n\nNo alternate config-file provided: Using %s file\n", config_file);

  while (fgets (buffer, 100, fd) != NULL)
    {
      strncpy (cut1, buffer, 8);
      strncpy (cut2, cut1, 8);
      if (cut1[0] == '#')
	{
	  i = 0;
	}
      else if (cut1[0] == 'N' && cut1[1] == 'E')	// this is the line NETWORK_SIZE
	{
	  i = 12;
	  while (buffer[i] != '=')
	    {
	      i++;
	    }
	  // advance the pointer one bit after '=' 
	  i++;

	  while (buffer[i] == ' ')
	    {
	      i++;
	    }
	  strcpy (copy, buffer + i);
	  NETWORK_SIZE = atoi (copy);
	}
      else if (cut1[0] == 'S' && cut1[6] == 'X')
	{
	  i = 13;
	  while (buffer[i] != '=')
	    {
	      i++;
	    }
	  i++;
	  while (buffer[i] == ' ')
	    {
	      i++;
	    }
	  strcpy (copy, buffer + i);
	  START_X_COORD = atoi (copy);
	}
      else if (cut1[0] == 'S' && cut1[6] == 'Y')
	{
	  i = 13;
	  while (buffer[i] != '=')
	    {
	      i++;
	    }
	  i++;
	  while (buffer[i] == ' ')
	    {
	      i++;
	    }
	  strcpy (copy, buffer + i);
	  START_Y_COORD = atoi (copy);
	}
      else if (cut1[0] == 'S' && cut1[6] == 'N')
	{
	  i = 22;
	  while (buffer[i] != '=')
	    {
	      i++;
	    }
	  i++;
	  while (buffer[i] == ' ')
	    {
	      i++;
	    }
	  strcpy (copy, buffer + i);
	  STARTING_BATTERY_POWER = atoi (copy);
	}
      else if (cut1[0] == 'C')
	{
	  i = 8;
	  while (buffer[i] != '=')
	    {
	      i++;
	    }
	  i++;
	  while (buffer[i] == ' ')
	    {
	      i++;
	    }
	  strcpy (copy, buffer + i);
	  CH_RATIO = atof (copy);
	}
      else if (cut1[0] == 'T')
	{
	  i = 12;
	  while (buffer[i] != '=')
	    {
	      i++;

	    }
	  i++;
	  while (buffer[i] == ' ')
	    {
	      i++;
	    }
	  strcpy (copy, buffer + i);
	  TOTAL_ROUNDS = atoi (copy);
	}
    }

  if (fclose (fd) != 0)
    {
      printf ("\nError in closing .config \n");
      exit (1);
    }
  return (struct sensor_node *) malloc (NETWORK_SIZE *
					sizeof (struct sensor_node));

}
