#include "freq_mux.h"
#include <stdlib.h>
#include <string.h>

fmux create_fmux(int poles,int rate,int max_freq,int bins){
  fmux mux=malloc(sizeof(struct fmux));
  mux->poles=poles;

  int fstep=max_freq/bins;
  
  int num_filters=bins*2-1;//because the lowest one has no highpass
  mux->freqs=malloc(sizeof(int)*(bins));
  mux->num_segs=bins;

  float* freq_itr=mux->freqs;
  memset(freq_itr,0,sizeof(float)*bins);
   
  afilter* filters=malloc(sizeof(struct a_f*)*(num_filters));
  mux->filters=filters;
  afilter* endptr=filters+num_filters;
  mux->end_ptr=endptr;
  int fone=fstep;





  //lowest frequency set

  for(afilter* ittr=filters;ittr<endptr;ittr++){

    *ittr=poled_f(rate,fone,poles,0);
    ittr++;

    if(ittr>=endptr)
      break;

    *ittr=poled_f(rate,fone,poles,1);


    fone=fone+fstep;
  }
  
  return mux;

  
}
fmux create_fmux_from_pre(int poles,int rate,int* freq_table,int size){
  int bins=size;
  fmux mux=malloc(sizeof(struct fmux));
  mux->poles=poles;

  
  int num_filters=bins*2-1;//because the lowest one has no highpass
  mux->freqs=malloc(sizeof(int)*(bins));
  mux->num_segs=bins;

  float* freq_itr=mux->freqs;
  memset(freq_itr,0,sizeof(float)*bins);
   
  afilter* filters=malloc(sizeof(struct a_f*)*(num_filters));
  mux->filters=filters;
  afilter* endptr=filters+num_filters;
  mux->end_ptr=endptr;





  //lowest frequency set

  for(afilter* ittr=filters;ittr<endptr;ittr++){

    *ittr=poled_f(rate,*freq_table,poles,0);
    ittr++;

    if(ittr>=endptr)
      break;

    *ittr=poled_f(rate,*freq_table,poles,1);


    freq_table++;
  }
  
  return mux;

  
}

void mux(fmux mux,int in){


  afilter* filters=mux->filters;
  afilter* end=mux->end_ptr;
  float* dptr=mux->freqs;
  *dptr=run_f(*filters,in);
  dptr++;
  filters++;

  int audio;
  for(afilter* ft=filters;ft<end;ft++){
    audio=run_f(*ft,in);
    if(ft>=end){
      *dptr=audio;
      break;
    }
    ft++;
    *dptr=run_f(*ft,audio);
    dptr++;
  }

}

float power_at(fmux mux,int index){
  if(index>=mux->num_segs){
    return -1;
  }
  return mux->freqs[index];
}
int set_power_at(fmux mux,int index, float value){
  if(index>=mux->num_segs){
    return -1;
  }

    mux->freqs[index]=value;
  return 1;
}

int demux(fmux mux){
  float* segs=mux->freqs;
  int size=mux->num_segs;

  int ret=0;
  for(int i=0;i<size;i++){
    if(i==0){
      ret=*segs;
    }else{
      ret=ret+segs[i];
     }
  }
  return ret;
}

void free_mux(fmux mux){

  free(mux->freqs);
  afilter* fil=mux->filters;
  afilter* endptr=mux->end_ptr;

  for(afilter* ff=fil;ff<endptr;ff++){
    free_f(*ff);
  }
  free(fil);
  free(mux);

}
