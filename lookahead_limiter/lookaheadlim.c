#include "lookaheadlim.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>



Limiter create_limiter(int size){


  Limiter lim = malloc(sizeof(struct limiter));
  lim->ring_buffer = malloc(sizeof(float)*size);
  lim->size = size;
  lim->local_gain=1;

  memset(lim->ring_buffer,0,sizeof(float)*size);
  return lim;
}

void increment_gain(Limiter lim,float release){
  float g_gain=lim->local_gain;
  g_gain=g_gain+release;
  if(g_gain>1){
    g_gain=1;
  }
  lim->local_gain=g_gain;
}

float run_limiter(Limiter limiter, float input,float limit,float release){

  //return the last index of the buffer and shift down
  float output = *(limiter->ring_buffer);

  float gain;
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
    gain=(limit/max);
    increment_gain(limiter,release);
    if(gain<limiter->local_gain){
      limiter->local_gain=gain;
    }
    
  }else{
    increment_gain(limiter,release);
  }

  return output*(limiter->local_gain);

  
}

void free_limiter(Limiter limiter){
  free(limiter -> ring_buffer);
  free(limiter);
}
