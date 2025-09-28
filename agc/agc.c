#include <math.h>
#include<stdlib.h>
#include<string.h>

/* 
double gain=10;

double gain_max=40;
double gain_max_half=15;
int dtime=0;
double avg_error=0;
double avg_audio=0;
*/

struct agc_info{
  double gain;
  double gain_max;
  double gain_max_half;
  int dtime;
  double avg_error;
  double avg_audio;
  double gain_avg;
  double* ring_buffer;
  int buffer_size;
  size_t copy_size;
};


#define PIHALF 1.570796327

typedef struct agc_info* AGC;
AGC create_agc(double gain_max,double gain_start,double gain_cut, int lookahead){
 
  AGC agc = malloc(sizeof(struct agc_info));
  agc->gain = gain_start;
  agc->gain_max = gain_max;
  agc->gain_max_half = gain_cut;
  agc->dtime=0;
  agc->avg_audio = 0;
  agc->avg_error = 0;
  agc->gain_avg = gain_start;

  agc->ring_buffer = malloc(sizeof(double)*lookahead);
  memset(agc->ring_buffer,0,sizeof(double)*lookahead);
  agc->buffer_size = lookahead;
  agc->copy_size = sizeof(double)*(lookahead - 1);
  return agc;
}
void free_agc(AGC agc){
  free(agc->ring_buffer);
  free(agc);
}

double apply_agc(AGC agc,double input,float target,float sens,int thresh,float trace_val,float release){
 /* if(sens<0){
    return input;
  }*/
  double* ring_buffer = agc->ring_buffer;
  double output = *(ring_buffer + agc->buffer_size - 1);
  memmove(ring_buffer + 1,ring_buffer,agc->copy_size);
  *ring_buffer = input;
  if(sens == 0){

    return sin(input/20860)*32760;
  }
  float absv=fabs(trace_val)*agc->gain;

  agc->avg_audio=agc->avg_audio/2+absv/2;
  if(absv<thresh){

      target = agc->avg_audio;      

  }
  if(agc->gain>agc->gain_avg){
    release=release/(agc->gain - agc->gain_avg + 1);
  }

    double cur_val=agc->avg_audio;
    double error2=target-cur_val;
    if(agc->dtime==0){
      agc->avg_error=error2;
      agc->dtime=1;
    }else{
      agc->avg_error=(agc->avg_error+error2)/2;
    }
  

  double mult=1;
  double error=agc->avg_error;
  if(error > 32767){
    for(;error>32767;error=error - 32767)
      mult=mult+1;
   }else if(error < -32767){
    for(;error<-32767;error=error + 32767)
      mult=mult+1;

   
  }
  
  //roll down slow roll up quickyl
  if(agc->avg_audio<thresh){
    float vcalc=sin(error/20860)*mult;
    if(vcalc<0){
      vcalc=-(1+vcalc);
    }else{
      vcalc=1-vcalc;
    }
    agc->gain=agc->gain*(1+vcalc*sens);
  }else{
    if(release!=0 && error > 0){

      agc->gain=agc->gain*(1+sin(error/20860)*release*mult);
      //gain=gain*(1+release);
    }else if (error<0){
      agc->gain=agc->gain*(1-sin(error/20860)*sens*mult);
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
  if(agc->gain>agc->gain_max){
    agc->gain=agc->gain_max;
  }
  if(agc->gain<0.000001){
    agc->gain=0.000001;
  }

  agc->gain_avg = (agc->gain_avg + agc->gain)/2;
  //return sin((input*gain)/20860)*target;
  return output * agc->gain;
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
