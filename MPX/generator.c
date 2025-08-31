#include "waves.h"
#include "generator.h"
#include <math.h>
#include <stddef.h>
#include "../multiband_compressor/mbc.h"


int intialize_timings=0;
float shifter_19=0;
float current_19 = 0;
float shifter_38=0;
float current_38 = 0;

float TPI=M_PI*2;

const float correct_38=507.5;
float correct_19=correct_38/2;

int itterator=0;
float get_mpx_next_value(float left,float right,int ratekhz,float percent_pilot,float percent_stereo,float percent_mono,Limiter composite_clip,float release,float max,int synth){

  float mult19=0;
  float mult38=0;

  
  if(intialize_timings!=ratekhz){

    shifter_19 = ((19000.0 + correct_19) / ratekhz)*(2*M_PI);
    shifter_38 = ((38000.0 + correct_38) / ratekhz)*(2*M_PI);

    current_19=0;
    current_38=0;

    intialize_timings=ratekhz;
  }
  

  if(synth<0){
  //cache synthesis
  if(ratekhz==192000){
        mult19=_19KHZ_192[itterator];
        mult38=_38KHZ_192[itterator];
        itterator++;
        if(itterator>S_192){
          itterator=0;
        }
  }else{
        mult19=_19KHZ_96[itterator];
        mult38=_38KHZ_96[itterator];

        itterator++;
        if(itterator>S_96){
          itterator=0;
        }

  }
  }else{
    //realtime synthersis

    mult19=sin(current_19);    
    mult38=sin(current_38);    

    current_19 = current_19 + shifter_19;
    if(current_19 >= TPI)
      current_19 = current_19 - (TPI);

   current_38 = current_38 + shifter_38;
    if(current_38 >= TPI)
      current_38 = current_38 - (TPI);

  }
  //100percent: 32760
  float percent_38=percent_stereo;
  float mono = ((left+right)/2.0)*percent_mono;
  float stereo = (((left - right)/2.0)*percent_38)*mult38;
  float percentnmpx = 1.0-percent_pilot;
  float clip_value = percentnmpx*max;

  float pre_mpx=mono+stereo;

  float limiter_out=pre_mpx;

  if(composite_clip!=NULL)
    limiter_out=run_limiter(composite_clip,pre_mpx,clip_value,release);

  float pilot=(percent_pilot*max)*mult19;

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
