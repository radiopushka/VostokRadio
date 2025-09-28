#include<math.h>
#include <string.h>


//Evan Nikitin 2025
struct sigmoidal_lookahead{
  double dynamic_ratio_m;
  double dynamic_ratio_s;
  double* ring;
  double* limit_b;
  double* limit_b2;
  double* helper;
  double limit;
  double ratio;
  double range;
  double attack;
  double limiter_half;
  double release;
  size_t bsize_pre;
  int size;


};
typedef struct sigmoidal_lookahead* SLim;

SLim create_sigmoidal_limiter(int buffersize, double ratio, double limit,double range,double attack, double release){

  SLim limiter = malloc(sizeof(struct sigmoidal_lookahead));
  limiter->ring = malloc(sizeof(double)*buffersize);
  limiter->helper = malloc(sizeof(double)*buffersize);
  limiter->limit_b = malloc(sizeof(double)*buffersize);
  limiter->limit_b2= malloc(sizeof(double)*buffersize);
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

  memset(limiter->ring,0,sizeof(double)*buffersize);
  memset(limiter->helper,0,sizeof(double)*buffersize);

  for(int i =0;i<buffersize;i++){
    limiter->limit_b[i] = limit;
    limiter->limit_b2[i] = limit;
  }
  
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
  double rstartm = mimic_tanh(ma1 , limiter->ratio + limiter->dynamic_ratio_m , limiter->limit,limiter->limiter_half);
  double rstarts = mimic_tanh(ma2 , limiter->ratio + limiter->dynamic_ratio_s , limiter->limit,limiter->limiter_half);

  if(rstartm > limiter->limit - limiter->range){
    double diff=(((limiter->limit - limiter->range)/rstartm)*50);
    limiter->dynamic_ratio_m=limiter->dynamic_ratio_m  + (limiter->attack / diff);
   if(limiter->dynamic_ratio_m>10){
      limiter->dynamic_ratio_m = 10;
    }

  }else{
    double diff=((rstartm/(limiter->limit - limiter->range))*50);
    limiter->dynamic_ratio_m=limiter->dynamic_ratio_m - (limiter->release / diff);
    if(limiter->dynamic_ratio_m<0){
      limiter->dynamic_ratio_m = 0;
    }
  }

  if(rstarts > limiter->limit - limiter->range){
    double diff=(((limiter->limit - limiter->range)/rstartm)*50);
    limiter->dynamic_ratio_s=limiter->dynamic_ratio_s  + (limiter->attack / diff);
   if(limiter->dynamic_ratio_s>10){
      limiter->dynamic_ratio_s = 10;
    }

  }else{
    double diff=((rstartm/(limiter->limit - limiter->range))*50);
    limiter->dynamic_ratio_s=limiter->dynamic_ratio_s - (limiter->release / diff);
    if(limiter->dynamic_ratio_s<0){
      limiter->dynamic_ratio_s = 0;
    }
  }
  double limit = limiter->limit;
  double ratiom = limiter->ratio + limiter->dynamic_ratio_m*0.4;
  double ratios = limiter->ratio + limiter->dynamic_ratio_s*0.4;
  double st_c = tanh_func(retst , ratios , limit);
  double mono_cap = limit + limit - fabs(st_c);
  double mono_c = tanh_func(retmono , ratiom , mono_cap);
  double stereo_cap = limit + limit - fabs(mono_c);
  st_c = tanh_func(retst , ratios , stereo_cap);


  *input1 = mono_c;
  *input2 = st_c;


}

void free_sigmoidal(SLim lim){

  free(lim->ring);
  free(lim->helper);
  free(lim->limit_b);
  free(lim->limit_b2);
  free(lim);
}
