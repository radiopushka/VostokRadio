#include<math.h>


//Evan Nikitin 2025

double sigmoidal_clipper(double input,double limit,double ratio){

  
  return (1.0 / (1.0 + pow(1 + 1 / (( limit ) / ratio),-input))) * (limit * 2) - limit;
}
double sigmoidal_clipper_tanh(double input,double limit,double ratio,double* autoratio){

  
  double result= tanh(input/(limit * (ratio + *autoratio))) * limit;
  //distortion threshold:
  double dist_thresh=limit-4000;
  if(fabs(result) > dist_thresh){
    *autoratio=*autoratio+0.1;
    result= tanh(input/(limit * (ratio + *autoratio))) * limit;

  }else{
    *autoratio=*autoratio-0.00005;
    if(*autoratio<0){
      *autoratio=0;
    }
  }
  return result;
}
