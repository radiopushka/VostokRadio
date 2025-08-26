
#ifndef MPX
#include "../lookahead_limiter/lookaheadlim.h"

//get a value from the mpx encoder
float get_mpx_next_value(float left,float right,int ratekhz,float percent_pilot,float percent_mono,Limiter composite_clip);


//sample rate resampling
void resample_up_stereo(short* input,short* output,short* input_end,int ratio);
 
#endif // !DEBUG 

