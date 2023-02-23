
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

#define ioColour 0 
#define compColour 1 
#define HIGH_LOW 1 

/*
 * Assigns io colour based on high low ordering 
 * Comp server ranks are lower ranked, IO server ranks are higher ranked 
 * in 1 node when using Hyperthreads 
 * After 1 node the logic operates in a step function way 
 * and divides ranks into nodes given by NODESIZE
 */

void highLowOrdering(struct iocomp_params *iocompParams) 
{

  /*
   * assume high low ordering of hyperthreads 
   */ 
  if(globalSize <= NODESIZE*2)
  {
    if (globalRank < globalSize/2) // comp server 
    {
      iocompParams->colour					= compColour;
    } 

    else if(globalRank >= globalSize/2) // io server
    {
      iocompParams->colour					= ioColour; 
    }
  }

  /*
   * If size is greater than a full node and its Hyperthreads
   * eg. ARCHER2 nodesize is 128. If size > 128*2 
   */ 
  else if(globalSize > NODESIZE*2)
  {
    if (globalRank%(NODESIZE*2) < globalSize%(NODESIZE*2)/2) // comp server 
    {
      iocompParams->colour					= compColour;
    } 

    else if(globalRank >= globalSize%(NODESIZE*2)/2) // io server
    {
      iocompParams->colour					= ioColour; 
    }
  }

}
}

