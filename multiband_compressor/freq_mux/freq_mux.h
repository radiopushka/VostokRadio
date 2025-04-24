#ifndef FREQ_MUX
#define FREQ_MUX

#include "../multi_pole/multi_pole.h"

struct fmux{
  afilter* filters;
  afilter* end_ptr;
  int poles;
  int num_segs;
  float* freqs;
};

typedef struct fmux* fmux;

fmux create_fmux(int poles,int rate,int max_freq,int bins);
fmux create_fmux_from_pre(int poles,int rate,int* freq_table,int size);
float power_at(fmux mux,int location);
int set_power_at(fmux mux,int location,float value);
void mux(fmux mux,int input);
int demux(fmux mux);

void free_mux(fmux mux);

#endif // !FREQ_MUX


