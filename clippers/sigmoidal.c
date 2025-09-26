#include<math.h>
#include <string.h>


//Evan Nikitin 2025
struct sigmoidal_lookahead{
  double dynamic_ratio;
  double* ring;
  double* helper;
  double limit;
  double ratio;
  double range;
  double attack;
  double release;
  size_t bsize_pre;
  int size;


};
typedef struct sigmoidal_lookahead* SLim;

SLim create_sigmoidal_limiter(int buffersize, double ratio, double limit,double range,double attack, double release){

  SLim limiter = malloc(sizeof(struct sigmoidal_lookahead));
  limiter->ring = malloc(sizeof(double)*buffersize);
  limiter->helper = malloc(sizeof(double)*buffersize);
  limiter->ratio=ratio;
  limiter->limit=limit;
  limiter->dynamic_ratio=0;
  limiter->range=range;
  limiter->size=buffersize;
  limiter->attack=attack;
  limiter->release=release;
  limiter->bsize_pre=sizeof(double)*buffersize;

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


double tanh_func(double input, double ratio,double limit){
  return tanh(input/(limit * ratio)) * limit;
  //return (atan(input/(limit * ratio))/(M_PI/2)) * limit;
}
double atan_func(double input, double ratio,double limit){
  return (atan(input/(limit * ratio))/(M_PI/2)) * limit;
  //return (atan(input/(limit * ratio))/(M_PI/2)) * limit;
}
double mimic_tanh(double input,double ratio, double limit){

  return (input/(limit * ratio)) * limit;
}

double apply_sigmoidal(SLim limiter, double input){

  double* ring_buffer=limiter->ring;
  double* aux_buffer=limiter->helper;

  double return_val = *(ring_buffer + limiter->size - 1);


  memmove(aux_buffer + 1,ring_buffer,(limiter->bsize_pre - sizeof(double)));
  *aux_buffer = input;

  memmove(ring_buffer,aux_buffer,limiter->bsize_pre);

  //double mval = tanh_func(return_val , limiter->ratio + limiter->dynamic_ratio , limiter->limit);
  double mstart = fabs(return_val);
  double attarashi_v = return_val;


  ring_buffer=limiter->ring;
  for(double* bwalk=ring_buffer; bwalk < ring_buffer + limiter->size; bwalk++){
    double absval = fabs(*bwalk);//fabs(tanh_func(*bwalk , limiter->ratio + limiter->dynamic_ratio , limiter->limit));


    if(absval > mstart){
      mstart = absval;
      attarashi_v = *bwalk;
    }
  }
  double rstart = fabs(mimic_tanh(attarashi_v , limiter->ratio + limiter->dynamic_ratio , limiter->limit));

  if(rstart > limiter->limit - limiter->range){
    double diff=(((limiter->limit - limiter->range)/rstart)*7000);
    limiter->dynamic_ratio=limiter->dynamic_ratio  + (limiter->attack / diff);
   if(limiter->dynamic_ratio>10){
      limiter->dynamic_ratio = 10;
    }

  }else{
    double diff=((rstart/(limiter->limit - limiter->range))*20000);
    limiter->dynamic_ratio=limiter->dynamic_ratio - (limiter->release / diff);
    if(limiter->dynamic_ratio<0){
      limiter->dynamic_ratio = 0;
    }
  }

  double mval = tanh_func(return_val , limiter->ratio + limiter->dynamic_ratio*0.4 , limiter->limit);


  return mval;

}

void free_sigmoidal(SLim lim){

  free(lim->ring);
  free(lim->helper);
  free(lim);
}
