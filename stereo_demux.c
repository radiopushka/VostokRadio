#include "fast_half.c"



void demux_mono(float* buffer, float* buffer_end){
int pcount=0;
float* pptr;



  for(float* ittr=buffer;ittr<buffer_end;ittr++){
    if(pcount!=0){
      int cur=*ittr;
      int prev=*pptr;
    

      int sum=fast_half(cur+prev);

      *pptr=sum;
      *ittr=sum;

    }
    pptr=ittr;
    pcount=~pcount;
  }

}
