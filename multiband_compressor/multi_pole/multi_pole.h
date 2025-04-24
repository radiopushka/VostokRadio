#ifndef MULTI_LPF
#define MULTI_LPF

#include "RC/rc.h"
struct a_f{
  rc_filter_info** filters;
  rc_filter_info** endptr;

};

typedef struct a_f* afilter;


struct a_f* poled_f(int rate, int freq, int poles,int direction);

float run_f(struct a_f* f,float input);

void free_f(struct a_f* lpf);

#endif // !MULTI_LPF
