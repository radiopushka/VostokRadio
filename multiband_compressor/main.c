#include "mbc.h"
#include<stdlib.h>
#include<string.h>

Multiband create_mbt(fmux freq_mux){
  Multiband mbt=malloc(sizeof(struct Multiband));
  int num_comps=freq_mux->num_segs;

  Compressor* cmp=malloc(sizeof(Compressor)*num_comps);
  Compressor* cmpend=cmp+num_comps;

  float* attacks=malloc(sizeof(float)*num_comps);
  float* release=malloc(sizeof(float)*num_comps);
  float* targets=malloc(sizeof(float)*num_comps);
  float* gate=malloc(sizeof(float)*num_comps);
  float* max_gain=malloc(sizeof(float)*num_comps);
  float* post_gain=malloc(sizeof(float)*num_comps);
  int* bypass=malloc(sizeof(int)*num_comps);

  memset(attacks,0,sizeof(float)*num_comps);
  memset(release,0,sizeof(float)*num_comps);
  memset(targets,0,sizeof(float)*num_comps);
  memset(gate,0,sizeof(float)*num_comps);
  memset(max_gain,0,sizeof(float)*num_comps);
  memset(post_gain,0,sizeof(float)*num_comps);
  memset(bypass,0,sizeof(int)*num_comps);

  mbt->size=num_comps;
  mbt->compressors=cmp;
  mbt->end_ptr=cmpend;

  mbt->attacks=attacks;
  mbt->release=release;
  mbt->targets=targets;
  mbt->gate=gate;
  mbt->max_gain=max_gain;
  mbt->post_amp=post_gain;
  mbt->bypass=bypass;

  mbt->freq_mux=freq_mux;


  for(Compressor* cit=cmp;cit<cmpend;cit++){
    *cit=create_compressor(COMP_RMS);
  }

  return mbt;

}


int get_size(Multiband mbt){
  return mbt->size;
}

void set_attack(Multiband mbt,int index,float attack){
  mbt->attacks[index]=attack;
}
void set_release(Multiband mbt,int index,float release){
  mbt->release[index]=release;
}
void set_target(Multiband mbt,int index,float target){
  mbt->targets[index]=target;
}
void set_gate(Multiband mbt,int index,float gate){
  mbt->gate[index]=gate;
}
void set_max_gain(Multiband mbt,int index,float gain){
  mbt->max_gain[index]=gain;
}
void set_post_amp(Multiband mbt,int index,float gain){

  mbt->post_amp[index]=gain;
}
void set_bypass(Multiband mbt,int index,int bypass){

  mbt->bypass[index]=bypass;
}
void set_type(Multiband mbt,int index,int type){
  mbt->compressors[index]->method=type;
} 



float get_amplitude_at(Multiband mbt,int index){

  return power_at(mbt->freq_mux,index);
}

void run_compressors(Multiband mbt){

    Compressor* ptrstart=mbt->compressors;
    Compressor* ptrend=mbt->end_ptr;
    fmux mux=mbt->freq_mux;

    float* targs=mbt->targets;
    float* attacks=mbt->attacks;
    float* release=mbt->release;
    float* gate=mbt->gate;
    float* mgn=mbt->max_gain;
    int* bypass=mbt->bypass;
    float* post_gain=mbt->post_amp;

    int locs=0;
    for(Compressor* citr=ptrstart;citr<ptrend;citr++){
      
          float amplitude=power_at(mux,locs);
          float val=amplitude;
          float cmpd;

          if(*bypass!=1){

            cmpd=run_comp(*citr,*release,*attacks,*targs,amplitude,*gate,*mgn);
            val=amplitude*cmpd;
          }
          
          set_power_at(mux,locs, val*(*post_gain));

          locs++;
          targs++;
          attacks++;
          release++;
          gate++;
          mgn++;
          post_gain++;
          bypass++;
    }
}

void free_multiband(Multiband mbc){
  Compressor* ptrstart=mbc->compressors;
  Compressor* end=mbc->end_ptr;
  for(Compressor* cc=ptrstart;cc<end;cc++){
    free(*cc);
  }
  free(ptrstart);
  free(mbc->attacks);
  free(mbc->release);
  free(mbc->targets);
  free(mbc->gate);
  free(mbc->max_gain);
  free(mbc->bypass);
  free(mbc->post_amp);

  free_mux(mbc->freq_mux);
  free(mbc);
}
