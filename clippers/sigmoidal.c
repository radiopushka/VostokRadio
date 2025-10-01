#include<math.h>
#include <string.h>
#include<stdlib.h>

//double LIM_knee=3000;

//Evan Nikitin 2025
struct sigmoidal_lookahead{
  double dynamic_ratio_m;
  double dynamic_ratio_s;
  double* ring;
  double  limit_b;
  double  limit_b2;
  double* helper;
  double limit;
  double ratio;
  double range;
  double attack;
  double limiter_half;
  double release;
  int clip_count;
  int clip_count_internal;
  size_t bsize_pre;
  double knee;

  double pre_saturation_ratio;
  double lim_saturate;
  double post_sat_gain;

  //anti-aliasing
  double intrp_mono[3];
  double intrp_st[3];
  size_t intrp_size;
  size_t intrp_cp_size;

  int size;


};
typedef struct sigmoidal_lookahead* SLim;

SLim create_sigmoidal_limiter(int buffersize, double ratio, double limit,double range,double attack, double release,double knee,double sat_ratio,double lim_prc_sat,double post_gain){

  SLim limiter = malloc(sizeof(struct sigmoidal_lookahead));
  limiter->ring = malloc(sizeof(double)*buffersize);
  limiter->helper = malloc(sizeof(double)*buffersize);
  limiter->limit_b = limit * 1.5;
  limiter->limit_b2= limit * 1.5;
  limiter->ratio=ratio;
  limiter->limit=limit;
  limiter->dynamic_ratio_s=0;
  limiter->dynamic_ratio_m=0;
  limiter->range=range;
  limiter->size=buffersize;
  limiter->limiter_half = limit * 1.5;
  limiter->attack=attack * ratio;
  limiter->release=release * ratio;
  limiter->bsize_pre=sizeof(double)*buffersize;

  limiter->pre_saturation_ratio=sat_ratio;
  limiter->lim_saturate=lim_prc_sat;
  limiter->post_sat_gain=post_gain;

  limiter->knee = knee;

  limiter->clip_count=0;
  limiter->clip_count_internal=0;

  memset(limiter->intrp_mono,0,sizeof(double)*3);
  memset(limiter->intrp_st,0,sizeof(double)*3);

  limiter->intrp_size = sizeof(double)*3;
  limiter->intrp_cp_size = sizeof(double)*2;

  memset(limiter->ring,0,sizeof(double)*buffersize);
  memset(limiter->helper,0,sizeof(double)*buffersize);


  return limiter;
}

int get_clip_count(SLim limiter){
  int clip_count = limiter->clip_count;
  limiter->clip_count=0;
  return clip_count;
}



double sigmoidal_clipper(double input,double limit,double ratio){


  return (1.0 / (1.0 + pow(1 + 1 / (( limit ) / ratio),-input))) * (limit * 2) - limit;
}
double sigmoidal_clipper_tanh(double input,double limit,double ratio,double* autoratio){




  double result= tanh(input/(limit * (ratio + *autoratio))) * limit;
  //distortion threshold:

  double dist_thresh=limit - 7000;
  if(fabs(result) > dist_thresh){
    *autoratio=*autoratio+0.01;

  }else{
    *autoratio=*autoratio-0.001;
    if(*autoratio<0){
      *autoratio=0;
    }
  }
  return result;
}


void sigmoidal_limit(SLim limiter,double limit){
  limiter->limit = limit;
}
double tanh_func(double input, double ratio,double limit){
  return tanh(input/(limit * ratio)) * limit;
  //return (atan(input/(limit * ratio))/(M_PI/2)) * limit;
}
double atan_func(double input, double ratio,double limit){
  return (atan(input/(limit * ratio))/(M_PI/2)) * limit;
  //return (atan(input/(limit * ratio))/(M_PI/2)) * limit;
}
double mimic_tanh(double input,double ratio, double limit,double limit_scale){

  return (input/(limit_scale * ratio)) * limit;
}
double saturator(double input, double limit,double ratio,double wetness,double* true_value){
    double tanhv =  tanh_func(input,ratio,limit);
    *true_value=tanhv;
    return tanhv * wetness + input*(1-wetness);
}


double calculate_interpolation(double* l3list){//basically a low pass filter at 16khz
  double side1 = l3list[0];
  double center = l3list[1];
  double side2 = l3list[2];

  double weight_side=1;
  double weight_center=3;

  double average = side1*weight_side + center*weight_center + side2*weight_side;

  return average/(weight_side+weight_side+weight_center);

}

int is_within(double d1,double d2,double pogreshnost){
    if(d1 < 0 && d2 > 0)
      return -1;

    if(d2 < 0 && d1 > 0)
      return -1;

    d1 = fabs(d1);
    d2 = fabs(d2);

    /*if(d1 < pogreshnost || d2 < pogreshnost)
      return -1;*/

    if(d1 == d2)
      return 1;

    if( fabs(d1 - d2) < pogreshnost)
      return 1;


    return -1;
}

double minimal_difference(double i1, double i2){
  i1 = fabs(i1);
  i2 = fabs(i2);
  if(i2 > i1)
    return i2 - i1;
  return i1 - i2;
}
//attempts to remove square waves
void harmonic_reduction(SLim limiter,double* l3list, double limit){
  double side1 = l3list[0];
  double center = l3list[1];
  double side2 = l3list[2];

  double max = fmax(fabs(side1),fabs(side2));
  max = fmax(fabs(center),fabs(max));

  double level = 100;
  if(max<level){
    level = max;
  }


  if(is_within(side1,center,level) == 1 && is_within(center,side2,level) == 1){// && is_within(center,limit,level) == 1){
      double difference1 = minimal_difference(center,side1);
      if(side1<0)
        l3list[0] = side1 + (level - difference1);
      else
        l3list[0] = side1 - (level - difference1);

      double difference2 = minimal_difference(center,side2);;
      if(side2<0)
        l3list[2] = side2 + (level - difference2);
      else
        l3list[2] = side2 - (level - difference2);
  }else if (is_within(side1,center,level) == 1){// && is_within(center,limit,level) == 1){
      double difference1 = minimal_difference(center,side1);
      if(side1<0)
        l3list[0] = side1 + (level - difference1);
      else
        l3list[0] = side1 - (level - difference1);

  }else if (is_within(side2,center,level) == 1){// && is_within(center,limit,level) == 1){
      double difference2 = minimal_difference(center,side2);;
      if(side2<0)
        l3list[2] = side2 + (level - difference2);
      else
        l3list[2] = side2 - (level - difference2);

  }

}

void apply_sigmoidal(SLim limiter, double* input1, double* input2){

  double* ring_buffer=limiter->ring;
  double* ring_buffer2=limiter->helper;

  double retmono = *(ring_buffer + limiter->size - 1);
  double retst = *(ring_buffer2 + limiter->size - 1);

  double limit = limiter->limit;
  double limit2x = limit + limit;

  memmove(ring_buffer + 1,ring_buffer,(limiter->bsize_pre - sizeof(double)));
  memmove(ring_buffer2 + 1,ring_buffer2,(limiter->bsize_pre - sizeof(double)));
  double tval;
  *ring_buffer = saturator(*input1,limit2x*limiter->lim_saturate,limiter->ratio,limiter->pre_saturation_ratio,&tval)*limiter->post_sat_gain;
  double composite = limit2x*limiter->lim_saturate - fabs(tval);
  *ring_buffer2 = saturator(*input2,composite,limiter->ratio,limiter->pre_saturation_ratio,&tval)*limiter->post_sat_gain;



  //double mval = tanh_func(return_val , limiter->ratio + limiter->dynamic_ratio , limiter->limit);
  double ma1 = fabs(retmono);

  double ma2 = fabs(retst);


  ring_buffer=limiter->ring;
  for(double* bwalk=ring_buffer; bwalk < ring_buffer + limiter->size; bwalk++){
    double absval = fabs(*bwalk);//fabs(tanh_func(*bwalk , limiter->ratio + limiter->dynamic_ratio , limiter->limit));
    double absval2 = fabs(*ring_buffer2);//fabs(tanh_func(*bwalk , limiter->ratio + limiter->dynamic_ratio , limiter->limit));


    ring_buffer2++;
    if(absval > ma1){
      ma1 = absval;
    }
    if(absval2 > ma2){
      ma2 = absval2;
    }

  }

  double ratiom = limiter->ratio + limiter->dynamic_ratio_m;
  double ratios = limiter->ratio + limiter->dynamic_ratio_s;

  //double mono_c = tanh_func(ma1 , ratiom , limit2x);

  double rstartm = mimic_tanh(ma1 , limiter->ratio + limiter->dynamic_ratio_m , limiter->limit,limit2x);
  composite = limit2x - rstartm;
  if(composite<1)
      composite=1;
  double rstarts = mimic_tanh(ma2 , limiter->ratio + limiter->dynamic_ratio_s , limiter->limit,composite);

  if(rstartm > limiter->limit - limiter->range){
    double diff=(((limiter->limit - limiter->range)/rstartm)*limiter->knee);
    if(diff< 1)
      diff = 1;

    limiter->dynamic_ratio_m=limiter->dynamic_ratio_m*(1+(limiter->attack / diff));
   if(limiter->dynamic_ratio_m>10){
      limiter->dynamic_ratio_m = 10;
    }

  }else if(rstartm < limiter->limit - limiter->range){
    double diff=((rstartm/(limiter->limit - limiter->range))*limiter->knee);
    if(diff < 1)
      diff = 1;
    limiter->dynamic_ratio_m=limiter->dynamic_ratio_m*(1 - limiter->release / diff);
    if(limiter->dynamic_ratio_m<0.00001){
      limiter->dynamic_ratio_m = 0.00001;
    }
  }

  if(rstarts > limiter->limit - limiter->range){
    double diff=(((limiter->limit - limiter->range)/rstarts)*limiter->knee);
    if(diff < 1)
      diff = 1;
    limiter->dynamic_ratio_s=limiter->dynamic_ratio_s*(1 + limiter->attack / diff);
   if(limiter->dynamic_ratio_s>10){
      limiter->dynamic_ratio_s = 10;
    }

  }else if(rstarts < limiter->limit - limiter->range){
    double diff=((rstarts/(limiter->limit - limiter->range))*limiter->knee);
    if(diff < 1)
      diff = 1;
    limiter->dynamic_ratio_s=limiter->dynamic_ratio_s*(1 - limiter->release / diff);
    if(limiter->dynamic_ratio_s<0.00001){
      limiter->dynamic_ratio_s = 0.00001;
    }
  }

  /*if(limiter->clip_count_internal>0){
    limiter->dynamic_ratio_m = limiter->dynamic_ratio_m+limiter->attack;
    limiter->dynamic_ratio_s = limiter->dynamic_ratio_s+limiter->attack;
    limiter->clip_count_internal = 0;
  }else{
    limiter->dynamic_ratio_m = limiter->dynamic_ratio_m-limiter->release;
    limiter->dynamic_ratio_s = limiter->dynamic_ratio_s-limiter->release;
    if(limiter->dynamic_ratio_m<0)
      limiter->dynamic_ratio_m = 0;
    if(limiter->dynamic_ratio_s<0)
      limiter->dynamic_ratio_s = 0;


  }*/

  ratiom = limiter->ratio + limiter->dynamic_ratio_m;
  ratios = limiter->ratio + limiter->dynamic_ratio_s;

  double st_c = 0;
  double mono_c = tanh_func(retmono , ratiom , limit2x);
  double stereo_cap = limit2x - fabs(mono_c);
  if(stereo_cap>0)
    st_c = tanh_func(retst , ratios , stereo_cap);

  if(is_within(fabs(mono_c),limit2x,(limit2x * 0.25))==1){
    limiter->clip_count++;
    limiter->clip_count_internal++;
  }

  //before outputing the signal, it has to be passed through an anti-aliasing filter to prevent distortion

  double* interp_mono = limiter->intrp_mono;
  memmove(interp_mono+1,interp_mono,limiter->intrp_cp_size);
  *interp_mono=mono_c;

  harmonic_reduction(limiter,interp_mono , limit2x);

  double* interp_stereo = limiter->intrp_st;
  memmove(interp_stereo+1,interp_stereo,limiter->intrp_cp_size);
  *interp_stereo = st_c;



  harmonic_reduction(limiter,interp_stereo, stereo_cap);
  //*input1 = mono_c;
  *input1 = calculate_interpolation(interp_mono);
  //*input2 = st_c;
  *input2 = calculate_interpolation(interp_stereo);


}

void free_sigmoidal(SLim lim){

  free(lim->ring);
  free(lim->helper);
  free(lim);
}
