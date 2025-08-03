#include "lookaheadlim.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>


Limiter create_limiter(int size){


  Limiter lim = malloc(sizeof(struct limiter));
  lim->ring_buffer = malloc(sizeof(float)*size);
  lim->size = size;

  memset(lim->ring_buffer,0,sizeof(float)*size);
  return lim;
}

float run_limiter(Limiter limiter, float input,float limit){

  //return the last index of the buffer and shift down
  float output = *(limiter->ring_buffer);

  int size = limiter->size;
  float* endptr = limiter->ring_buffer + (size-1);
  float max = fabs(output);
  float* strptr;
  for(strptr = limiter->ring_buffer; strptr<endptr ;strptr++){
    float next = *(strptr+1);
    *strptr = next;
    if(fabs(next)>max){
      max = fabs(next);
    }

  }
  *strptr = input;
  if(fabs(input) > max){
    max=fabs(input);
  }

  if(max > limit){
    output = output*(limit/max);
  }

  return output;

  
}

void free_limiter(Limiter limiter){
  free(limiter -> ring_buffer);
  free(limiter);
}
