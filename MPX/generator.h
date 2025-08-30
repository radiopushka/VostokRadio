
#ifndef MPX
#include "../lookahead_limiter/lookaheadlim.h"

//get a value from the mpx encoder
float get_mpx_next_value(float left,float right,int ratekhz,float percent_pilot, float percent_stereo,float percent_mono,Limiter composite_clip,float release,float max,int synth);


//sample rate resampling
void resample_up_stereo(int* input,int* output,int* input_end,int ratio);
 
#endif // !DEBUG 

