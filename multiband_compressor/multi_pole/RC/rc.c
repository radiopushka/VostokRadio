#include "rc.h"
#include <stdlib.h>
#include <math.h>

#define PI 3.141592653589793

struct rc_filter_info* create_rc_filter(double frequency,int rate, int direction){

  struct rc_filter_info* rcfilter=malloc(sizeof(struct rc_filter_info));
  double RC=1.0/(frequency*2*PI);
  double dt=1.0/rate;

  if(direction==0){
    rcfilter->alpha=dt/(RC+dt);
  }else{

    rcfilter->alpha=RC/(RC+dt);
  }
  //rcfilter->alpha=rcfilter->alpha*0.1;
  rcfilter->prev=0;
  rcfilter->prev_raw=0;
  rcfilter->direction=direction;
  return rcfilter;
}

double do_rc_filter(struct rc_filter_info* rcf,double in){

  double val;
  /*if(rcf->direction==0){
    val=rcf->prev * (1 - rcf->alpha) + rcf->alpha * (in - rcf->prev);
  }else{
    val=rcf->prev * (1 - rcf-> alpha) + (rcf->alpha * in);

  }*/
  if(rcf->direction==0){
    val=rcf->prev + rcf->alpha * (in - rcf->prev);
  }else{
    val=rcf->alpha * ( rcf->prev + in - rcf->prev_raw);

  }
    if(fabs(val) > 1e8){
        if(val > 0)
          val = val - 1e7;
        else 
          val = val + 1e7;
    }else if(fabs(val) < 1e-8){
        if(val > 0)
         val= val + 0.1;
        else
          val = val - 0.1;
    }

  rcf->prev_raw=in;
  rcf->prev=val;
  return val;
}

void free_rc_filter(struct rc_filter_info* rcf){
  if(rcf==NULL)
    return;

  free(rcf);
}
