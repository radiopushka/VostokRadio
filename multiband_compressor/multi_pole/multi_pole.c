#include "multi_pole.h"
#include<stdlib.h>
#include <stdio.h>

struct a_f* poled_f(int rate, int freq, int poles,int direction){
  struct a_f* mpoled=malloc(sizeof(struct a_f));
  rc_filter_info** lstr=malloc(sizeof(rc_filter_info*)*(poles));
  rc_filter_info** endptr=lstr+poles;

  for(rc_filter_info** lptr=lstr;lptr<endptr;lptr++){
    *lptr=create_rc_filter(freq,rate,direction);
  }
  mpoled->filters=lstr;
  mpoled->endptr=endptr;
  return mpoled;
}

float run_f(struct a_f* lpf,float input){

  rc_filter_info** startptr=lpf->filters;
  rc_filter_info** endptr=lpf->endptr;

  for(rc_filter_info** cptr=startptr;cptr<endptr;cptr++){
    input=do_rc_filter(*cptr,input);
  }
  return input;
}
void free_f(struct a_f* lpf){
  if(lpf==NULL)
    return;

  rc_filter_info** startptr=lpf->filters;
  rc_filter_info** endptr=lpf->endptr;

  for(rc_filter_info** cptr=startptr;cptr<endptr;cptr++){
    free_rc_filter(*cptr);
  }
  free(startptr);
  free(lpf);
}


