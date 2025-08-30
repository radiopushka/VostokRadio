
void amplify_stereo_plex(short* buffer, short* buffer_end,float gain){
int pcount=0;
float cgain=gain*2;
if(gain<1){
    cgain=gain/2;
  }


  for(short* ittr=buffer;ittr<buffer_end;ittr=ittr+2){
    if(pcount!=0){
      int L=*ittr;
      int R=*ittr+1;
    

      float sum=L+R;
      float diff=L-R;//L-R
      diff=diff*gain;

      float L_t= diff + sum;
      float R_t= diff + sum;

      if(gain<1){
      *ittr=L_t*cgain;
      *(ittr+1)=R_t*cgain;

      }else{
      *ittr=L_t/cgain;
      *(ittr+1)=R_t/cgain;
      }

    }
    pcount=~pcount;
  }

}
//two functions for less abstraction and less computation time
void amplify_stereo_plex_int(int* buffer, int* buffer_end,float gain){
int pcount=0;
float cgain=gain*2;
if(gain<1){
    cgain=gain/2;
  }


  for(int* ittr=buffer;ittr<buffer_end;ittr=ittr+2){
    if(pcount!=0){
      int L=*ittr;
      int R=*ittr+1;
    

      float sum=L+R;
      float diff=L-R;//L-R
      diff=diff*gain;

      float L_t= diff + sum;
      float R_t= diff + sum;

      if(gain<1){
      *ittr=L_t*cgain;
      *(ittr+1)=R_t*cgain;

      }else{
      *ittr=L_t/cgain;
      *(ittr+1)=R_t/cgain;
      }

    }
    pcount=~pcount;
  }

}
