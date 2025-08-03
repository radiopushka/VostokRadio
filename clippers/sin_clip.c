#include <math.h>
#include<stdio.h>

//sinusoidal clipper
//when the value exceeds the set it will dent the wave form down
//it will generate harmonics (most likely high frequency harmonics)

//an interesting idea:
//if we bechave like two sinusoidal waveforms then we get a gradual descend to zero when our audio signal is above half
//we can also emulate the sigmoidal function
//this is the clip limit, emulating the sigmoidal function
#define PIHALF 1.570796327
#define SIN_THRESHOLD (PIHALF*0.5)
#define DEF_COEFF (32767/PIHALF)
#define DYN_RANGE 30000
#define COOL_DOWN 0.00005



/*float dynamic_compressor(float input, int strength){
  if(strength < 1)
    return input;

  float compressval = input/DEF_COEFF;
  float vv = sin(compressval)*32767;
  
  if(input > 32767){
    vv = 32767;
  }else if(input < -32767){
    vv = -32767;
  }
    

  return dynamic_compressor(vv,strength - 1);
}*/
float clpval = 1.4;
float sin_clip(float input,float coeff,float max, float* max_audio){
  if(fabs(input) > *max_audio){
    *max_audio = fabs(input);
  }else if(fabs(input) > DYN_RANGE){

    float attarashi = fabs(input) + (32767 - DYN_RANGE);
    if(attarashi > *max_audio)
      *max_audio=attarashi;
  }
  float val = input/(*max_audio/PIHALF);
  if(*max_audio > 32767  && fabs(input) < DYN_RANGE){
    *max_audio = *max_audio - (*max_audio - 32767)*COOL_DOWN;
  }
  float ret=sin(val);

  if(fabs(input) <= 16383){

    if(fabs(val)> 16056){
    //printf("%f\n",(ret*max/input));
      clpval = ((ret*max)/input);
    }
    
    return input*clpval;
  }
  return ret*max;
}
float get_sin_clip_coeff(float max_expected_audio_level){
  return max_expected_audio_level/PIHALF;
}
//auto AGC type clipper
float sin_clip_sigmoidal(float input,float coeff,float max){
  float ccdiv=input/coeff;

  if(ccdiv>PIHALF){
    ccdiv=PIHALF;
  }

  if(ccdiv<-PIHALF){
    ccdiv=-PIHALF;
  }


  return sin(ccdiv)*max;
  
}
float sin_clip_bouncy(float input,float coeff,float max, float* max_audio){
  if(fabs(input) > *max_audio){
    *max_audio = fabs(input);
  }else if(fabs(input) > DYN_RANGE){

    float attarashi = fabs(input) + (32767 - DYN_RANGE);
    if(attarashi > *max_audio)
      *max_audio=attarashi;
  }
  float val = input/(*max_audio/PIHALF);
  if(*max_audio > 32767 &&fabs(input) < DYN_RANGE ){
    *max_audio = *max_audio - (*max_audio - 32767)*COOL_DOWN;
  }
   float ret=sin(val);

   return ret*max;
}
float dynamic_limiter = 32767;
float dynamic_compressor(float input, int strength){
  if(strength < 1)
    return input;

  float vv = sin_clip_bouncy(input,0,32767,&dynamic_limiter);
  
    

  return dynamic_compressor(vv,strength - 1);
}

