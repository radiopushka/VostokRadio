#include <math.h>
#include<stdlib.h>

float gain=1;

float gain_max=20;
int dtime=0;
float avg_error=0;

#define PIHALF 1.570796327

float apply_agc(float input,float target,float sens,int thresh,int trace_val){
 /* if(sens<0){
    return input;
  }*/
  short absv=abs(trace_val);
  
  if(absv>thresh){
    float cur_val=absv*gain;
    float error=target-cur_val;
    if(dtime==0){
      avg_error=error;
      dtime=1;
    }else{
      avg_error=(avg_error+error)/2;
    }
  
  if(cur_val>32767){//handle clipping
    avg_error=avg_error/2+(target-cur_val);
  }
  
  gain=gain+sin(avg_error/20860)*sens;
  if(gain>gain_max){
    gain=gain_max;
  }
  if(gain<0.1){
    gain=0.1;
  }
  }
  return input*gain;
  /*
  //printf("%g\n",gain);
  if(abs(input) < thresh){
    return input;
  }

  if(input<0)
    thresh = -thresh;

  float target_mult=(target)*2;
  float range = (32767/PIHALF);
  if(fabs(input/range) > PIHALF){
    return input;
  }
   
  float level = thresh + sin(input/range)*(target_mult-abs(thresh));


  return apply_agc(level,target,sens - 1, thresh, trace_val);*/
}
