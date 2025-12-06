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
double pr_pilot=0;

//so that the stereo is not modulated too much
double st_lowpass=0;
double st_lowpass2=0;
double m_lowpass=0;
double m_lowpass2=0;
const double mult_new=1;
double mult_pr=1-mult_new;

//how much ultrasonic distortion the clipper produces
#define PERCENT_COMP_DIST 0.02
double harmonic_red_val = 2081818578 * PERCENT_COMP_DIST;

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
double tlim = 2081818578;
double mpx_clip_t;
double* synth_19=NULL;
double* synth_19_fft=NULL;
double* synth_38=NULL;


void free_mpx_cache(){

  free(synth_19_fft);
  free(synth_19);
  free(synth_38);

}

void init_mpx_cache(long double ratekhz,long double over_sampling){
      free_mpx_cache();
      long double shifter_19 = ((19000.0) / (ratekhz*over_sampling))*(2*M_PI);
      long double shifter_38 = shifter_19*2;



      intialize_timings=ratekhz;



      synth_19=malloc(sizeof(double)*buffer_size);
      synth_19_fft=malloc(sizeof(double)*buffer_size);
      synth_38=malloc(sizeof(double)*buffer_size);


      long double counter=0;
      long double counter_secondary=0;
      double* s38=synth_38;
      double* s19_fft = synth_19_fft;
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
          *s19_fft=-(v19/over_sampling);
          *s38=(v38/over_sampling);
          s38++;
          s19_fft++;
      }



}


void init_mpx(int ratekhz,double percent_pilot,double max){
      clip_value=max;
      _pilot=percent_pilot*max;
      mpx_clip_t=max;
      tlim=max;
      pr_pilot = percent_pilot;
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
double tanh_func_mpx(double input, double ratio,double limit){
  return tanh(input/(limit * ratio)) * limit;
  //return (atan(input/(limit * ratio))/(M_PI/2)) * limit;
}
int is_within(double d1,double d2,double pogreshnost){
    if(d1 < 0 && d2 > 0)
      return -1;

    if(d2 < 0 && d1 > 0)
      return -1;

    d1 = fabs(d1);
    d2 = fabs(d2);

    /*if(d1 < pogreshnost || d2 < pogreshnost)
      return -1;*/

    if(d1 == d2)
      return 1;

    if( fabs(d1 - d2) < pogreshnost)
      return 1;


    return -1;
}
double minimal_difference(double i1, double i2){
  i1 = fabs(i1);
  i2 = fabs(i2);
  if(i2 > i1)
    return i2 - i1;
  return i1 - i2;
}
double mpx_list[3];
double rsamp_list1[3];
double rsamp_list2[3];
void put_list(double list[],double value){

  list[2]=list[1];
  list[1]=list[0];
  list[0]=value;
}
double calculate_interpolation_mpx(double list[],double cw){//basically a low pass filter at 16khz


  double side1 = list[0];
  double center = list[1];
  double side2 = list[2];

  double weight_side=1;
  double weight_center=cw;//3

  double average = side1*weight_side + center*weight_center + side2*weight_side;

  return average/(weight_side+weight_side+weight_center);

}
void harmonic_reduction(double* l3list, double limit){
  double side1 = l3list[0];
  double center = l3list[1];
  double side2 = l3list[2];

  double max = fmax(fabs(side1),fabs(side2));
  max = fmax(fabs(center),fabs(max));

  //this value controls how much ultrasonic harmonics the composite clipper will produce
  double level = harmonic_red_val;
  if(max<level){
    level = max;
  }


  if(is_within(side1,center,level) == 1 && is_within(center,side2,level) == 1){// && is_within(center,limit,level) == 1){
      double difference1 = minimal_difference(center,side1);
      if(side1<0)
        l3list[0] = side1 + (level - difference1);
      else
        l3list[0] = side1 - (level - difference1);

      double difference2 = minimal_difference(center,side2);;
      if(side2<0)
        l3list[2] = side2 + (level - difference2);
      else
        l3list[2] = side2 - (level - difference2);
  }else if (is_within(side1,center,level) == 1){// && is_within(center,limit,level) == 1){
      double difference1 = minimal_difference(center,side1);
      if(side1<0)
        l3list[0] = side1 + (level - difference1);
      else
        l3list[0] = side1 - (level - difference1);

  }else if (is_within(side2,center,level) == 1){// && is_within(center,limit,level) == 1){
      double difference2 = minimal_difference(center,side2);;
      if(side2<0)
        l3list[2] = side2 + (level - difference2);
      else
        l3list[2] = side2 - (level - difference2);

  }

}


//double get_mpx_next_value(double left,double right,double percent_stereo,double percent_mono){
double get_mpx_next_value(double mono,double stereo,double percent_mono,double percent_stereo){



 double o38=synth_38[itterator];
 double fft_19 = synth_19_fft[itterator];


 mono = mono*(percent_mono);
 stereo = stereo*(percent_stereo);



  stereo=stereo-HF_BIAS;



  double k19=synth_19[itterator];
  itterator++;
  if(itterator>=buffer_size){
          itterator=0;
  }

	double k38 = (stereo)*o38;


  //generate the sampled signal


  double composite=k38+mono;
  if(fabs(composite)<0.00001)
    composite=0.00001;

  composite=tanh_func_mpx(composite,1,tlim);


  put_list(mpx_list,composite);


  harmonic_reduction(mpx_list,tlim);
  double composite_out=calculate_interpolation_mpx(mpx_list,3);

  //remove any 19khz component that is 180 degrees out of phase with the pilot
  //do this while respecting limits
  double fft = composite_out*fft_19;
  double fftr = composite_out*(-fft_19);
  double prev = composite_out;
  composite_out = composite_out-(fft+fftr);
  if(fabs(composite_out) > tlim)
    composite_out = prev;


  //make room for the pilot on demand
  if(composite_out+k19 > tlim){
    composite_out = composite_out - ((composite_out+k19)-tlim);
  }

  if(composite_out+k19 < -tlim){

    composite_out = composite_out - ((composite_out+k19)+tlim);
  }

  return k19+composite_out;



}

void resample_up_stereo_mpx(double* input,int* output,double* input_end,int ratio){

  for(double* loop=input;loop<input_end;loop=loop+2){

    double stereo = (*(loop + 1));
    double mono  = (*loop);


    for(int i=0;i<ratio;i++){
      //*output=*loop;

      put_list(rsamp_list1,mono);
      *output=calculate_interpolation_mpx(rsamp_list1,2);
      output++;
      //*output=*right;
      put_list(rsamp_list2,stereo);
      *output=calculate_interpolation_mpx(rsamp_list2,2);
      output++;
    }

  }
}
