
#include "compressor.h"
#include <math.h>
#include <stdlib.h>

//Evan Nikitin 2025
Compressor create_compressor(int method){
  Compressor comp=malloc(sizeof(struct Compressor));
  comp->avg=16000;//large value to prevent startup distortion
  comp->prev_val=0;
  comp->prevprev_val=0;
  comp->gain=1;
  comp->ratio=1;
  comp->knee=0.3;
  comp->method=method;
  return comp;
}

double run_comp(Compressor comp,double release, double attack, double target, double input,double gate,double max_gain){
  if(fabs(input)<gate){
    if(comp->gain < 1)
      comp->gain=comp->gain*(1+release);
    if(comp->gain > 1)
      comp->gain=comp->gain*(1-release);

    

    
    return ((comp->gain)*(comp->ratio))+(1-comp->ratio);
  }
  int method=comp->method;
    
  float slope2=fabs(input)*comp->gain;


  if(method==COMP_PEAK){
    
    //detect the peak
    float slope=comp->prevprev_val;
    float center=comp->prev_val;
    //if peak
    if(center>slope && center>slope2){
      if(center>target){
        double diff=1-((target/center)*comp->knee);
        comp->gain=comp->gain+((attack*diff));
        //comp->gain=comp->gain-attack;
      }
      if(target>center){

        double diff=1-((center/target)*comp->knee);
        comp->gain=comp->gain-((release*diff));
        //comp->gain=comp->gain+release;
      }
    }
  }else{
    comp->avg=(comp->avg+fabs(input)*comp->gain)/2;
    if(comp->avg<target){
        double diff=1-((comp->avg/target)*comp->knee);
        comp->gain=comp->gain+((release*diff));
        //comp->gain=comp->gain+release;
    }
   if(comp->avg>target){
        double diff=1-((target/comp->avg)*comp->knee);
        comp->gain=comp->gain-((attack*diff));
        //comp->gain=comp->gain-attack;

    }

  }

  if(comp->gain>max_gain){
    comp->gain=max_gain;
  }
if(comp->gain<release/2){
    comp->gain=release/2;
  }

  comp->prevprev_val=comp->prev_val;
  comp->prev_val=slope2;
  return ((comp->gain)*(comp->ratio))+(1-comp->ratio);
}

