
#include "compressor.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>



//Evan Nikitin 2025
Compressor create_compressor(int method,int lookahead){
  Compressor comp=malloc(sizeof(struct Compressor));
  comp->avg=16000;//large value to prevent startup distortion
  comp->prev_val=0;
  comp->prevprev_val=0;
  comp->gain=1;
  comp->ratio=1;
  comp->knee=0.3;
  comp->ring_size=lookahead;
  comp->ring=malloc(sizeof(double)*lookahead);
  comp->method=method;
  memset(comp->ring,0,sizeof(double)*lookahead);
  return comp;
}

void free_compressor(Compressor comp){
  free(comp->ring);
  free(comp);
}
double run_comp(Compressor comp,double release, double attack, double target, double input,double gate,double max_gain,int bypass){

  
  int size = comp->ring_size;
  double excluded = comp->ring[size-1];

  memmove(comp->ring + 1,comp->ring,sizeof(double)*(size-1));
  comp->ring[0]=input;

  if(bypass == 1)
    return excluded;

  double max = fabs(excluded);
  if(comp->method == COMP_PEAK){

    for(int i=0;i<size;i++)
      max = fmax(comp->ring[i],max);
    
  }else{
      max = fabs(input); 
  }

  input = max;

  if(input<gate){
    if(comp->gain < 1)
      comp->gain=comp->gain*(1+release);
    if(comp->gain > 1)
      comp->gain=comp->gain*(1-release);

    

    
    return (((comp->gain)*(comp->ratio))+(1-comp->ratio)) * excluded;
  }
  int method=comp->method;
    
  float slope2=input*comp->gain;


  if(method==COMP_PEAK){
    
      input = input * comp->gain;
      if(input>target){
        double diff=((target/input)*comp->knee);
        double nattack=(attack/diff);
        if(nattack>attack)
          nattack=attack;
        comp->gain=comp->gain*(1-(nattack));
        //comp->gain=comp->gain-attack;
      }else{

        double diff=((input/target)*comp->knee);
        double nrelease=(release/diff);
        if(nrelease>release)
          nrelease=release;
        comp->gain=comp->gain*(1+(nrelease));
        //comp->gain=comp->gain+release;
      }
    
  }else{
    input = input * comp->gain;
    comp->avg = (comp->avg + input)/2;
    if(comp->avg<target){
    

        double diff=((comp->avg/target)*comp->knee);
        double nrelease=(release/diff);
        if(nrelease>release)
          nrelease=release;

        comp->gain=comp->gain*(1+(nrelease));
        //comp->gain=comp->gain+release;
    }else{
        double diff=((target/comp->avg)*comp->knee);
        double nattack=(attack/diff);
        if(nattack>attack)
          nattack=attack;

        comp->gain=comp->gain*(1-(nattack));
        //comp->gain=comp->gain-attack;

    }

  }

  if(comp->gain>max_gain){
    comp->gain=max_gain;
  }

  if(comp->gain < 0.0000001){
    comp->gain = 0.0000001;
  }

  comp->prevprev_val=comp->prev_val;
  comp->prev_val=slope2;
  return (((comp->gain)*(comp->ratio))+(1-comp->ratio)) * excluded;
}

