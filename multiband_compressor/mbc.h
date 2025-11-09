
#ifndef MBC
#define MBC

#include "./compressor/compressor.h"
#include "./freq_mux/freq_mux.h"
struct Multiband{
  Compressor* compressors;
  Compressor* end_ptr;

  int size;

  double* attacks;
  double* release;
  double* targets;
  double* gate;
  double* max_gain;
  int* bypass;
  double* post_amp;


  fmux freq_mux;
};

typedef struct Multiband* Multiband;


//create the multiband compressor object
Multiband create_mbt(fmux freq_mux,int* lookaheads);
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
void set_ratio(Multiband mbt,int index,float ratio);
void set_knee(Multiband mbt,int index,float knee);
void set_dknee(Multiband mbt,int index,float knee);


//get the amplitude for visual rendering:
double get_amplitude_at(Multiband mbt,int index);

//run after the frequency muxer
void run_compressors(Multiband mbt);
void run_compressors_advanced(Multiband mbt,double (*on_gain_value)(double,double,int));

//free memory
void free_multiband(Multiband mbc);

#endif // !MBC
