#include <math.h>

//sinusoidal clipper
//when the value exceeds the set it will dent the wave form down
//it will generate harmonics (most likely high frequency harmonics)

//an interesting idea:
//if we bechave like two sinusoidal waveforms then we get a gradual descend to zero when our audio signal is above half
//we can also emulate the sigmoidal function
//this is the clip limit, emulating the sigmoidal function
#define PIHALF 1.570796327

float sin_clip(float input,float coeff,float max){
  return sin(input/coeff)*max;
}
float get_sin_clip_coeff(float max_expected_audio_level){
  return max_expected_audio_level/PIHALF;
}

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

