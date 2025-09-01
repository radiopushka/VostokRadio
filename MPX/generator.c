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


float offset=0;
float offseth=0.0716755875225;

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
      float shifter_38 = ((38000.0) / ratekhz)*(2*M_PI);



      intialize_timings=ratekhz;

     

      synth_19=malloc(sizeof(float)*48000);
      synth_38=malloc(sizeof(float)*48000);

      float counter=0;
      float* s38=synth_38;
      for(float* s19=synth_19;counter<48000;s19++){
          *s19=sin(shifter_19*(counter+offset));  
          *s38=sin(shifter_38*(counter+offseth));  
          s38++;
          counter=counter+1;
      }



}


float get_mpx_next_value(float left,float right,int ratekhz,float percent_pilot,float percent_stereo,float percent_mono,Limiter composite_clip,float release,float max,int synth){

  float mult19=0;
  float mult38=0;

   if(intialize_timings!=ratekhz){

      init_mpx_cache(ratekhz); 
      clip_value=(1.0-percent_pilot)*max;
      _pilot=percent_pilot*max;
    }

   

 
        mult38=synth_38[itterator];
        mult19=synth_19[itterator];
        itterator++;
        if(itterator>=48000){
          itterator=0;
        }

   //100percent: 32760
  float percent_38=percent_stereo;
  float mono = ((left+right)/2.0)*percent_mono;
  float stereo = (((left - right)/2.0)*percent_38);

	stereo=stereo*mult38;

  float pre_mpx=mono+stereo;

  float limiter_out=pre_mpx;

  if(composite_clip!=NULL)
    limiter_out=run_limiter(composite_clip,pre_mpx,clip_value,release);

  float pilot=(_pilot)*mult19;

  return limiter_out+pilot;



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
