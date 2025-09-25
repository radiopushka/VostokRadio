
#include "compressor.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

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
  memset(comp->ring,0,sizeof(double)*15);
  return comp;
}

double run_comp(Compressor comp,double release, double attack, double target, double input,double gate,double max_gain,int bypass){

  

  double helper[15];
  double excluded = comp->ring[14];

  memmove(helper + 1,comp->ring,sizeof(double)*14);
  helper[0]=input;
  memmove(comp->ring,helper,sizeof(double)*15);

  if(bypass == 1)
    return excluded;

  double max = fabs(excluded);
  for(int i=0;i<15;i++){
    double pull = comp->ring[i];
    if(comp->method != COMP_PEAK){
      max = (max + fabs(pull))/2;
    }else if(fabs(pull) > max){
      max = pull;
    }
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
        double diff=1-((target/input)*comp->knee);
        comp->gain=comp->gain*(1-(attack*diff));
        //comp->gain=comp->gain-attack;
      }
      if(target>input){

        double diff=1-((input/target)*comp->knee);
        comp->gain=comp->gain*(1+(release*diff));
        //comp->gain=comp->gain+release;
      }
    
  }else{
    input = input * comp->gain;
    comp->avg = (comp->avg + input)/2;
    if(comp->avg<target){
        double diff=1-((comp->avg/target)*comp->knee);
        comp->gain=comp->gain*(1+(release*diff));
        //comp->gain=comp->gain+release;
    }
   if(comp->avg>target){
        double diff=1-((target/comp->avg)*comp->knee);
        comp->gain=comp->gain*(1-(attack*diff));
        //comp->gain=comp->gain-attack;

    }

  }

  if(comp->gain>max_gain){
    comp->gain=max_gain;
  }

  comp->prevprev_val=comp->prev_val;
  comp->prev_val=slope2;
  return (((comp->gain)*(comp->ratio))+(1-comp->ratio)) * excluded;
}

