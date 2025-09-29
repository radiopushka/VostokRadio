#include<math.h>
#include <string.h>

double LIM_knee=3000;

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
  size_t bsize_pre;

  //anti-aliasing
  double intrp_mono[3];
  double intrp_st[3];
  size_t intrp_size;
  size_t intrp_cp_size;

  int size;


};
typedef struct sigmoidal_lookahead* SLim;

SLim create_sigmoidal_limiter(int buffersize, double ratio, double limit,double range,double attack, double release){

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
  limiter->attack=attack;
  limiter->release=release;
  limiter->bsize_pre=sizeof(double)*buffersize;

  memset(limiter->intrp_mono,0,sizeof(double)*3);
  memset(limiter->intrp_st,0,sizeof(double)*3);

  limiter->intrp_size = sizeof(double)*3;
  limiter->intrp_cp_size = sizeof(double)*2;

  memset(limiter->ring,0,sizeof(double)*buffersize);
  memset(limiter->helper,0,sizeof(double)*buffersize);

  
  return limiter;
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

double calculate_interpolation(double* l3list){
  double side1 = l3list[0]; 
  double center = l3list[1]; 
  double side2 = l3list[2]; 

  double weight_side=1;
  double weight_center=8;

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

    if(d1 == d2)
      return 1;

    if( fabs(d1 - d2) < pogreshnost)
      return 1;


    return -1;
}
//attempts to remove square waves
void harmonic_reduction(double* l3list, double limit){
  double side1 = l3list[0]; 
  double center = l3list[1]; 
  double side2 = l3list[2]; 

  double level = 100;


  if(is_within(side1,center,level) == 1 && is_within(center,side2,level) == 1){// && is_within(center,limit,level) == 1){
      if(side1<0)
        l3list[0] = side1 + level;
      else
        l3list[0] = side1 - level;

      if(side2<0)
        l3list[2] = side2 + level;
      else
        l3list[2] = side2 - level;
  }else if (is_within(side1,center,level) == 1){// && is_within(center,limit,level) == 1){
      if(side1<0)
        l3list[0] = side1 + level;
      else
        l3list[0] = side1 - level;

  }else if (is_within(side2,center,level) == 1){// && is_within(center,limit,level) == 1){
      if(side2<0)
        l3list[2] = side2 + level;
      else
        l3list[2] = side2 - level;

  }

}

void apply_sigmoidal(SLim limiter, double* input1, double* input2){

  double* ring_buffer=limiter->ring;
  double* ring_buffer2=limiter->helper;

  double retmono = *(ring_buffer + limiter->size - 1);
  double retst = *(ring_buffer2 + limiter->size - 1);


  memmove(ring_buffer + 1,ring_buffer,(limiter->bsize_pre - sizeof(double)));
  memmove(ring_buffer2 + 1,ring_buffer2,(limiter->bsize_pre - sizeof(double)));
  *ring_buffer = *input1;
  *ring_buffer2 = *input2;



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

  double limit = limiter->limit;
  double ratiom = limiter->ratio + limiter->dynamic_ratio_m*0.4;
  double ratios = limiter->ratio + limiter->dynamic_ratio_s*0.4;

  double st_c = tanh_func(ma2 , ratios , limit + limit);
  double mono_c = tanh_func(ma1 , ratiom , limit + limit);
  double stereo_cap = limit + limit - mono_c;

  double rstartm = mimic_tanh(ma1 , limiter->ratio + limiter->dynamic_ratio_m , limiter->limit,limit + limit - st_c );
  double rstarts = mimic_tanh(ma2 , limiter->ratio + limiter->dynamic_ratio_s , limiter->limit,stereo_cap);

  if(rstartm > limiter->limit - limiter->range){
    double diff=(((limiter->limit - limiter->range)/rstartm)*LIM_knee);
    limiter->dynamic_ratio_m=limiter->dynamic_ratio_m  + (limiter->attack / diff);
   if(limiter->dynamic_ratio_m>10){
      limiter->dynamic_ratio_m = 10;
    }

  }else if(rstartm < limiter->limit - limiter->range){
    double diff=((rstartm/(limiter->limit - limiter->range))*LIM_knee);
    limiter->dynamic_ratio_m=limiter->dynamic_ratio_m - (limiter->release / diff);
    if(limiter->dynamic_ratio_m<0){
      limiter->dynamic_ratio_m = 0;
    }
  }

  if(rstarts > limiter->limit - limiter->range){
    double diff=(((limiter->limit - limiter->range)/rstartm)*LIM_knee);
    limiter->dynamic_ratio_s=limiter->dynamic_ratio_s  + (limiter->attack / diff);
   if(limiter->dynamic_ratio_s>10){
      limiter->dynamic_ratio_s = 10;
    }

  }else if(rstarts < limiter->limit - limiter->range){
    double diff=((rstartm/(limiter->limit - limiter->range))*LIM_knee);
    limiter->dynamic_ratio_s=limiter->dynamic_ratio_s - (limiter->release / diff);
    if(limiter->dynamic_ratio_s<0){
      limiter->dynamic_ratio_s = 0;
    }
  }
  ratiom = limiter->ratio + limiter->dynamic_ratio_m*0.4;
  ratios = limiter->ratio + limiter->dynamic_ratio_s*0.4;

  mono_c = tanh_func(retmono , ratiom , limit + limit);
  stereo_cap = limit + limit - fabs(mono_c);
  double attenuation = (limit + limit)/stereo_cap - 1;// softer clipping
  st_c = tanh_func(retst , ratios + attenuation*limiter->ratio , stereo_cap);


  //before outputing the signal, it has to be passed through an anti-aliasing filter to prevent distortion

  double* interp_mono = limiter->intrp_mono;
  memmove(interp_mono+1,interp_mono,limiter->intrp_cp_size);
  *interp_mono=mono_c;

  harmonic_reduction(interp_mono , limit + limit);

  double* interp_stereo = limiter->intrp_st;
  memmove(interp_stereo+1,interp_stereo,limiter->intrp_cp_size);
  *interp_stereo = st_c;

  harmonic_reduction(interp_stereo, stereo_cap);
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
