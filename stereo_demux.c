#include "fast_half.c"
//Evan Nikitin 2025


void demux_mono(double* buffer, double* buffer_end){
int pcount=0;
double* pptr;



  for(double* ittr=buffer;ittr<buffer_end;ittr++){
    if(pcount!=0){
      double cur=*ittr;
      double prev=*pptr;
    
    
      
      double sum=(cur+prev)/2;

      *pptr=sum;
      *ittr=sum;

    }
    pptr=ittr;
    pcount=~pcount;
  }

}
