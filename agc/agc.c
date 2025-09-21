#include <math.h>
#include<stdlib.h>

float gain=1;

float gain_max=20;
float gain_max_half=20;
int dtime=0;
float avg_error=0;
float avg_audio=0;

#define PIHALF 1.570796327

float apply_agc(float input,float target,float sens,int thresh,float trace_val,float release){
 /* if(sens<0){
    return input;
  }*/
  if(sens == 0){

    return sin(input/20860)*32760;
  }
  float absv=fabs(trace_val)*gain;

  avg_audio=avg_audio/2+absv/2;
  if(absv<thresh){

      target = avg_audio;      

  }
  if(gain>gain_max_half){
    release=release/(gain_max-(gain_max-gain));
  }

    float cur_val=avg_audio;
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
  
  //roll down slow roll up quickyl
  if(avg_audio<thresh){
    float vcalc=sin(avg_error/20860);
    if(vcalc<0){
      vcalc=-(1+vcalc);
    }else{
      vcalc=1-vcalc;
    }
    gain=gain+vcalc*sens;
  }else{
    if(release!=0 && avg_error > 0){

      gain=gain+sin(avg_error/20860)*release;
      //gain=gain*(1+release);
    }else if (avg_error<0){
      gain=gain+sin(avg_error/20860)*sens;
      //gain=gain*(1-sens);
    }
    
  }
  //
  /*if(release != 0){
  if(cur_val>target){
    gain = gain*(1-sens);
  }else if(cur_val<target){
    gain = gain*(1+release);
      
  }
  }*/
  if(gain>gain_max){
    gain=gain_max;
  }
  if(gain<1){
    gain=1;
  }

  //return sin((input*gain)/20860)*target;
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
