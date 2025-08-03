
#include "compressor.h"
#include <math.h>
#include <stdlib.h>

Compressor create_compressor(int method){
  Compressor comp=malloc(sizeof(struct Compressor));
  comp->avg=16000;//large value to prevent startup distortion
  comp->prev_val=0;
  comp->prevprev_val=0;
  comp->gain=1;
  comp->method=method;
  return comp;
}

float run_comp(Compressor comp,float release, float attack, float target, float input,float gate,float max_gain){
  if(fabs(input)<gate){
    return (comp->gain);
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
        comp->gain=comp->gain*(1-attack);
        //comp->gain=comp->gain-attack;
      }
      if(target>center){

        comp->gain=comp->gain*(1+release);
        //comp->gain=comp->gain+release;
      }
    }
  }else{
    comp->avg=(comp->avg+fabs(input)*comp->gain)/2;
    if(comp->avg<target){
        comp->gain=comp->gain*(1+release);
        //comp->gain=comp->gain+release;
    }
   if(comp->avg>target){
        comp->gain=comp->gain*(1-attack);
        //comp->gain=comp->gain-attack;

    }

  }

  if(comp->gain>max_gain){
    comp->gain=max_gain;
  }

  comp->prevprev_val=comp->prev_val;
  comp->prev_val=slope2;
  return comp->gain;
}

