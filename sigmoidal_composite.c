#include "./clippers/sigmoidal.c"


void to_mpx(double* array, double* endptr){
  for(;array<endptr;array++){
    double* left = array;
    array++;
    double* right = array;

    double mono = (*left+*right);
    double stereo = (*left - *right);
    
    *left=mono;
    *right=stereo;
  }
}

void mpx_clip(SLim clipper, double* array,double* endptr,double limit){

  for(;array<endptr;array++){
    double* mono = array;
    array++;
    double* stereo = array;


    
    
    apply_sigmoidal(clipper,mono,stereo);

  }
  
}


void gain_array(double* array, double* endptr,double gain){
  for(;array<endptr;array++){
    *array = (*array) * gain;
  }

}
