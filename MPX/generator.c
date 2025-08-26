#include "waves.h"
#include "generator.h"
#include <stddef.h>


int itterator=0;
float get_mpx_next_value(float left,float right,int ratekhz,float percent_pilot,float percent_mono,Limiter composite_clip){
  float mult19=_19KHZ_96[itterator];
  float mult38=_38KHZ_96[itterator];
  if(ratekhz==192000){
        mult19=_19KHZ_192[itterator];
        mult38=_38KHZ_192[itterator];
        itterator++;
        if(itterator>S_192){
          itterator=0;
        }
  }else{
        itterator++;
        if(itterator>S_96){
          itterator=0;
        }

  }
  //100percent: 32760
  float percent_38=1.0-(percent_pilot+percent_mono);
  float mono = ((left+right)/2.0)*percent_mono;
  float stereo = (((left - right)/2.0)*percent_38)*mult38;
  float percentnmpx = 1.0-percent_pilot;
  float clip_value = percentnmpx*32760;

  float pre_mpx=mono+stereo;

  float limiter_out=pre_mpx;

  if(composite_clip!=NULL)
    limiter_out=run_limiter(composite_clip,pre_mpx,clip_value);

  float pilot=(percent_pilot*32760)*mult19;

  return limiter_out+pilot;



}

void resample_up_stereo(short* input,short* output,short* input_end,int ratio){
  
  for(short* loop=input;loop<input_end;loop=loop+2){
    short* right = loop + 1;
    for(int i=0;i<ratio;i++){
      *output=*loop;
      output++;
      *output=*right;
      output++;
    }

  }
}
