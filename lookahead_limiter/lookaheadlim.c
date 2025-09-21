#include "lookaheadlim.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>



Limiter create_limiter(int size){


  Limiter lim = malloc(sizeof(struct limiter));
  lim->ring_buffer = malloc(sizeof(double)*size);
  lim->ring_buffer_helper = malloc(sizeof(double)*size);
  lim->size = size;
  lim->max_cache=FLT_MIN;
  lim->local_gain=1;

  memset(lim->ring_buffer,0,sizeof(double)*size);
  return lim;
}

void increment_gain(Limiter lim,double release){
  double g_gain=lim->local_gain;
  g_gain=g_gain+release;
  if(g_gain>1){
    g_gain=1;
  }
  lim->local_gain=g_gain;
}

double run_limiter(Limiter limiter, double input,double limit,double release){

  //return the last index of the buffer and shift down
  double output = *(limiter->ring_buffer);
  //max calculation
  /*if(fabs(output)==limiter->max_cache){
    double max_val=0;
    for(float* ittr=outpu+1t;ittr<limiter->size;ittr++){
      if(fabs(ittr)>max_val){
        m
      }
    }
  }*/

  double gain;
  int size = limiter->size;
  double* endptr = limiter->ring_buffer + (size-1);
  double max = fabs(output);
  double* strptr;
  int max_index=0;
  for(strptr = limiter->ring_buffer; strptr<endptr ;strptr++){
    float next = *(strptr+1);
    *strptr = next;
    if(fabs(next)>max){
      max = fabs(next);
      max_index=strptr-limiter->ring_buffer;
    }

  }
  *strptr = input;
  if(fabs(input) > max){
    max=fabs(input);
    max_index=size-1;
  }

  if(max > limit){
    double real_gain = (limit)/(max);
    double tmp_gain= real_gain+((1.0-real_gain)*(((double)max_index)/((double)size)));
    if(tmp_gain>1){
      tmp_gain=1;
    }
    if(fabs(tmp_gain*output)>limit){
      gain=real_gain;
    }else{
      gain = tmp_gain;
    }
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
  free(limiter -> ring_buffer_helper);
  free(limiter);
}
