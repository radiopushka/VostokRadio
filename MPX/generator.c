#include "waves.h"
#include "generator.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include<stdio.h>
#include "../multiband_compressor/mbc.h"
//Evan Nikitin 2025

int intialize_timings=0;
double clip_value=0;
double _pilot=0;

double TPI=M_PI*2;

const int over_sample_co=128;
const int buffer_size=192000;

//most sound cards require this to generate the MPX signal propperly
double ANALOG_BIAS=0;
double HF_BIAS=0;

//the measured amplitude of the 2nd harmonic of the 19khz signal
double P2nd_DAC_HARMONIC=0.00006;

double st_bias_offset=0;

//so that the stereo is not modulated too much
double st_lowpass=0;
double st_lowpass2=0;
double m_lowpass=0;
double m_lowpass2=0;
const double mult_new=1;
double mult_pr=1-mult_new;

//determined experimentally on an old ASUS laptop
//192khz sample rate card
double offset19=0;
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
double offset38=0.00793875;
//float offseth=0;
//7516;

int itterator=0;

double* synth_19=NULL;
double* synth_38=NULL;


void free_mpx_cache(){

  free(synth_19);
  free(synth_38);

}

void init_mpx_cache(long double ratekhz,long double over_sampling){
      free_mpx_cache();
      long double shifter_19 = ((19000.0) / (ratekhz*over_sampling))*(2*M_PI);
      long double shifter_38 = shifter_19*2;



      intialize_timings=ratekhz;



      synth_19=malloc(sizeof(double)*buffer_size);
      synth_38=malloc(sizeof(double)*buffer_size);


      long double counter=0;
      long double counter_secondary=0;
      double* s38=synth_38;
      for(double* s19=synth_19;counter<buffer_size;s19++){
          long double v19=0;
          long double v38=0;
          for(int i=0;i<over_sampling;i++){
            v19=v19+sinl(shifter_19*(counter_secondary+offset19));
            v38=v38+sinl(shifter_38*(counter_secondary+offset38));
            counter_secondary=counter_secondary+1;
          }
          counter=counter+1;
          *s19=(v19/over_sampling)*_pilot;
          *s38=(v38/over_sampling);
          s38++;
      }



}


void init_mpx(int ratekhz,double percent_pilot,double max){
      clip_value=(1.0-percent_pilot)*max;
      _pilot=percent_pilot*max;
      HF_BIAS=_pilot*P2nd_DAC_HARMONIC;
      st_bias_offset=percent_pilot*P2nd_DAC_HARMONIC;
      init_mpx_cache(ratekhz,over_sample_co);

}

double mpx_peak_38khz_modulation(){
    //192 khz
    double max = 0;
    int mittr = itterator;
    for(int i=0;i<4;i++){
            double mval = fabs(synth_38[mittr]);
            if(mval>max){
                max = mval;
            }
            mittr++;
            if(mittr>=buffer_size){
                mittr = 0;
            }

        }

    return max;
}
double tlim = 2081818578;
double tanh_func_mpx(double input, double ratio,double limit){
  return tanh(input/(limit * ratio)) * limit;
  //return (atan(input/(limit * ratio))/(M_PI/2)) * limit;
}


//double get_mpx_next_value(double left,double right,double percent_stereo,double percent_mono){
double get_mpx_next_value(double mono,double stereo,double percent_mono,double percent_stereo){


 stereo=stereo*1.5;
 mono=mono*1.5;

 double o38=synth_38[itterator];
 /*double pre_c=stereo*o38;
 if(fabs(pre_c)<0.00001)
    pre_c=0.00001;
 if(fabs(mono)<0.00001)
    mono=0.00001;
 double ratios = fabs(pre_c/(pre_c+mono));
 //ratios = ratios*fabs(o38);
 double ratiom = fabs(mono/(pre_c+mono));

 //mono = mono*ratiom;
 //stereo = stereo*ratios;

   
 mono = tanh_func_mpx(mono,1,tlim*ratiom);
 stereo = tanh_func_mpx(stereo,1,tlim*ratios);*/

 mono = mono*(percent_mono);
 stereo = stereo*(percent_stereo);


   //100percent: 32760
  //double mono = ((left+right)/2.0)*percent_mono;
  //double stereo = (((left - right)/2.0)*(percent_stereo-st_bias_offset));


  //virtualy no need in adding this limiter
  /*
  float pre_mpx=mono+stereo;

  float limiter_out=pre_mpx;

  if(composite_clip!=NULL)
    limiter_out=run_limiter(composite_clip,pre_mpx,clip_value,release);

  */

  //perform oversampling to try and get rid of aliasing

  double k19=0;
  double k38=0;



  //sometimes it is better to just cut off this signal if there is not enough range to produce a more or less accurate waveform
  //having it at some baseline reduces distortion
  //apparently in order for stereo MPX signal to be generated propperly, sound cards need some kind of bias

  /*if(fabs(stereo)<ANALOG_BIAS){
    if(stereo<0)
      stereo=-ANALOG_BIAS;
    else
      stereo=ANALOG_BIAS;
  }*/
  stereo=stereo-HF_BIAS;



  double o19=synth_19[itterator];
  itterator++;
  if(itterator>=buffer_size){
          itterator=0;
  }

	double val_pilot = o19;
	double val_audio = (stereo)*o38;

		k19=val_pilot;
		k38=val_audio;

  //generate the sampled signal


  double composite=k38+mono;
  if(fabs(composite)<0.00001)
    composite=0.00001;

  composite=tanh_func_mpx(composite,1,tlim*fmax(percent_mono,percent_stereo));



  return k19+composite;



}

void resample_up_stereo_mpx(double* input,int* output,double* input_end,int ratio){

  for(double* loop=input;loop<input_end;loop=loop+2){

    double stereo = (*(loop + 1));
    double mono  = (*loop);


    for(int i=0;i<ratio;i++){
      //*output=*loop;

      *output=mono;
      output++;
      //*output=*right;
      *output=stereo;
      output++;
    }

  }
}
