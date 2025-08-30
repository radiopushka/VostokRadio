
void amplify_stereo_plex(short* buffer, short* buffer_end,float gain){
int pcount=0;
short* pptr;
float cgain=gain*2;
if(gain<1){
    cgain=gain/2;
  }


  for(short* ittr=buffer;ittr<buffer_end;ittr++){
    if(pcount!=0){
      int cur=*ittr;
      int prev=*pptr;
    

      float sum=cur+prev;
      float diff=prev-cur;//L-R
      diff=diff*gain;

      float L= diff + sum;
      float R= diff - sum;

      if(gain<1){
      *pptr=L*cgain;
      *ittr=R*cgain;

      }else{
      *pptr=L/cgain;
      *ittr=R/cgain;
      }

    }
    pptr=ittr;
    pcount=~pcount;
  }

}
//two functions for less abstraction and less computation time
void amplify_stereo_plex_int(int* buffer, int* buffer_end,float gain){
int pcount=0;
int* pptr;
float cgain=gain*2;


  for(int* ittr=buffer;ittr<buffer_end;ittr++){
    if(pcount!=0){
      int cur=*ittr;
      int prev=*pptr;
    

      float sum=cur+prev;
      float diff=prev-cur;//L-R
      diff=diff*gain;

      float L= diff + sum;
      float R= diff - sum;

      if(gain<1){
      *pptr=L*cgain;
      *ittr=R*cgain;

      }else{
      *pptr=L/cgain;
      *ittr=R/cgain;
      }

    }
    pptr=ittr;
    pcount=~pcount;
  }

}
