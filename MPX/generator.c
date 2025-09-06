#include "waves.h"
#include "generator.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include "../multiband_compressor/mbc.h"


int intialize_timings=0;
float clip_value=0;
float _pilot=0;

float TPI=M_PI*2;

const int over_sample_co=64;
const int buffer_size=192000*over_sample_co;

//most sound cards require this to generate the MPX signal propperly
float ANALOG_BIAS=200000;
float HF_BIAS=300;

//the measured amplitude of the 2nd harmonic of the 19khz signal
float P2nd_DAC_HARMONIC=0.00006;

float st_bias_offset=0;

//so that the stereo is not modulated too much
float st_lowpass=0;
float st_lowpass2=0;
float m_lowpass=0;
float m_lowpass2=0;
const float mult_new=0.53333;
float mult_pr=1-mult_new;

//determined experimentally on an old ASUS laptop
//192khz sample rate card
float offset=0;
//float offseth=0.0716755875220826;
//the sound card has a low pass filter which skews the phase
//you need to increase the phase here to compensate for it
/*
 *your sound card will not perfectly synthesize these signals,
 you will have to tune this offset value so that the distortion is not audible on receivers.
  You can try different sound cards, but design the transmitter input so that it is a high impedance load of a few kilo ohms, that way any filters on the sound card if they are present will have less of an effect. 
 sometimes you will have to disable one channel if you have a crappy thin cable
 * */
//float offseth=0.067633;
//it takes roughly 646 samples to sample 19 khz
//it takes 323 to sample 38
//for a 42.4 degree phase shift (42.4/0.5291)/2
//it is likely your sound just has distortion at 38 khz so you might have to find some value that can cancel it out
//this part of the tunning process is really hard, good luck
//float offseth=0.078375;//phase offset in samples, 33.86khz RC filter(most sound cards)
		     //you would need a phase meter or an oscilliscope 
		     //at the output of your sound card to accurately determine this value
		     //this is likely a positive value because most filters, parasetic or intentional. shift the phase in the negative direction
float offseth=0.00793875;
//float offseth=0;
//7516;

int itterator=0;

float* synth_19=NULL;
float* synth_38=NULL;


void free_mpx_cache(){

  free(synth_19);
  free(synth_38);

}

void init_mpx_cache(float ratekhz){
      free_mpx_cache();
      float shifter_19 = ((19000.0) / ratekhz)*(2*M_PI);
      float shifter_38 = shifter_19*2;



      intialize_timings=ratekhz;

     

      synth_19=malloc(sizeof(float)*buffer_size);
      synth_38=malloc(sizeof(float)*buffer_size);


      float counter=0;
      float* s38=synth_38;
      for(float* s19=synth_19;counter<buffer_size;s19++){
          counter=counter+1;
          *s19=sin(shifter_19*(counter+offset));  
          *s38=sin(shifter_38*(counter+offseth));  
          s38++;
      }



}


void init_mpx(int ratekhz,float percent_pilot,float max){
      init_mpx_cache(ratekhz*over_sample_co); 
      clip_value=(1.0-percent_pilot)*max;
      _pilot=percent_pilot*max;
      HF_BIAS=_pilot*P2nd_DAC_HARMONIC;
      st_bias_offset=percent_pilot*P2nd_DAC_HARMONIC;

}

float get_mpx_next_value(float left,float right,float percent_stereo,float percent_mono){


  
   

   
   //100percent: 32760
  float mono = ((left+right)/2.0)*percent_mono;
  float stereo = (((left - right)/2.0)*(percent_stereo-st_bias_offset));


  //virtualy no need in adding this limiter
  /*
  float pre_mpx=mono+stereo;

  float limiter_out=pre_mpx;

  if(composite_clip!=NULL)
    limiter_out=run_limiter(composite_clip,pre_mpx,clip_value,release);

  */

  //perform oversampling to try and get rid of aliasing 
  
  float k19=0;
  float k38=0;

  m_lowpass2=(m_lowpass2*mult_pr+mono*mult_new);
  m_lowpass=(m_lowpass*mult_pr+m_lowpass2*mult_new);
  mono=m_lowpass;


  //sometimes it is better to just cut off this signal if there is not enough range to produce a more or less accurate waveform
  //having it at some baseline reduces distortion
  //apparently in order for stereo MPX signal to be generated propperly, sound cards need some kind of bias
  
  if(fabs(stereo)<ANALOG_BIAS){
    if(stereo<0)
      stereo=-ANALOG_BIAS;
    else
      stereo=ANALOG_BIAS;
  }
  stereo=stereo-HF_BIAS;
  st_lowpass2=(st_lowpass2*mult_pr+stereo*mult_new);
  st_lowpass=(st_lowpass*mult_pr+st_lowpass2*mult_new);


  for(int i=0;i<over_sample_co;i++){

	      float o38=synth_38[itterator];
        float o19=synth_19[itterator];
        itterator++;
        if(itterator>=buffer_size){
          itterator=0;
        }

	float val_pilot = _pilot*o19;
	float val_audio = (st_lowpass)*o38;

	if(i==0){
		k19=val_pilot;
		k38=val_audio;
	}else{
		k19=k19/2+val_pilot/2;
		k38=k38/2+val_audio/2;
	}
  }

  //generate the sampled signal



  return k38+k19+mono;



}

void resample_up_stereo(int* input,int* output,int* input_end,int ratio){
  
  for(int* loop=input;loop<input_end;loop=loop+2){
    int* right = loop + 1;
    for(int i=0;i<ratio;i++){
      *output=*loop;
      output++;
      *output=*right;
      output++;
    }

  }
}
