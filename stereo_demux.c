#include "fast_half.c"



void demux_mono(short* buffer, short* buffer_end){
int pcount=0;
short* pptr;



  for(short* ittr=buffer;ittr<buffer_end;ittr++){
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
