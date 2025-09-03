
#ifndef MPX
#include "../lookahead_limiter/lookaheadlim.h"

//get a value from the mpx encoder
float get_mpx_next_value(float left,float right, float percent_stereo,float percent_mono);
void free_mpx_cache();

void init_mpx(int rate,float percent_pilot,float max);

//sample rate resampling
void resample_up_stereo(int* input,int* output,int* input_end,int ratio);
 
#endif // !DEBUG 

