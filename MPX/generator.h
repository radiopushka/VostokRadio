
#ifndef MPX
#include "../lookahead_limiter/lookaheadlim.h"
//Evan Nikitin 2025
//get a value from the mpx encoder
//double get_mpx_next_value(double left,double right, double percent_stereo,double percent_mono);
double get_mpx_next_value(double left,double right,double percent_stereo,double percent_mono);
void free_mpx_cache();

void init_mpx(int rate,double percent_pilot,double max);

double mpx_peak_38khz_modulation();//get mpx modulation value for time sliced clipping
//sample rate resampling
//void resample_up_stereo(int* input,int* output,int* input_end,int ratio);
void resample_up_stereo_mpx(double* input,int* output,double* input_end,int ratio);

#endif // !DEBUG

