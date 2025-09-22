
#ifndef MPX
#include "../lookahead_limiter/lookaheadlim.h"
//Evan Nikitin 2025
//get a value from the mpx encoder
double get_mpx_next_value(double left,double right, double percent_stereo,double percent_mono);
void free_mpx_cache();

void init_mpx(int rate,float percent_pilot,float max);

//sample rate resampling
void resample_up_stereo(int* input,int* output,int* input_end,int ratio);
 
#endif // !DEBUG 

