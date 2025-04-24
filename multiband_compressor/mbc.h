
#ifndef MBC
#define MBC

#include "./compressor/compressor.h"
#include "./freq_mux/freq_mux.h"
struct Multiband{
  Compressor* compressors;
  Compressor* end_ptr;

  int size;

  float* attacks;
  float* release;
  float* targets;
  float* gate;
  float* max_gain;
  int* bypass;
  float* post_amp;

  fmux freq_mux;
};

typedef struct Multiband* Multiband;


//create the multiband compressor object
Multiband create_mbt(fmux freq_mux);
//get the number of mutliband compressors present
int get_size(Multiband mbt);


//compressor settings
void set_attack(Multiband mbt,int index,float attack);
void set_release(Multiband mbt,int index,float release);
void set_target(Multiband mbt,int index,float target);
void set_gate(Multiband mbt,int index,float gate);
void set_max_gain(Multiband mbt,int index,float gain);
void set_post_amp(Multiband mbt,int index, float gain);
void set_bypass(Multiband mbt,int index,int bypass);
void set_type(Multiband mbt,int index,int type);


//get the amplitude for visual rendering:
float get_amplitude_at(Multiband mbt,int index);

//run after the frequency muxer
void run_compressors(Multiband mbt);

//free memory
void free_multiband(Multiband mbc);

#endif // !MBC
