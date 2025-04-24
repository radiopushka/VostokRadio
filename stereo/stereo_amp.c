
void amplify_stereo_plex(short* buffer, short* buffer_end,float gain){
int pcount=0;
short* pptr;
float cgain=gain*2;



  for(short* ittr=buffer;ittr<buffer_end;ittr++){
    if(pcount!=0){
      int cur=*ittr;
      int prev=*pptr;
    

      int sum=cur+prev;
      float diff=prev-cur;//L-R
      diff=diff*gain;

      int L= diff + sum;
      int R= diff - sum;

      *pptr=L/cgain;
      *ittr=R/cgain;

    }
    pptr=ittr;
    pcount=~pcount;
  }

}
