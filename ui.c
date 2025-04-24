
#include "curses/curses.c"
#include "KEYS.h"

void draw_bar(int val,int x, int y,int shift,int max){
  int nv=val>>shift;
  for(int i=0;i<nv;i++){
    if(y-i>=0){
      mvmove(x,y-i);
      printf("*");
    }
  }
  int mv=max>>shift;
  if(y-mv>=0){
    mvmove(x,y-mv);
    printf("=");
  }
}

void clipper_agc_screen(int c,float* stereo_gain,int avg_post_agc,int avg_pre_agc,int avg_post_clip,int avg_pre_clip,int* stereo){
  int tw,th;
  term_size(&tw,&th);

  int hidari_ki=(tw>>2)-(tw>>4);

  int x=hidari_ki;
  int y=th-4;
  clear();
  fclear();

  mvmove(x-5,y);printf("after agc");
  draw_bar(avg_post_agc,x,y-2,10,32767);
  x=x+hidari_ki;
  mvmove(x-5,y);printf("before agc");
  draw_bar(avg_pre_agc,x,y-2,10,32767);
  x=x+hidari_ki;
  mvmove(x-5,y);printf("after clip");
  draw_bar(avg_post_clip,x,y-2,10,32767);
  x=x+hidari_ki;
  mvmove(x-5,y);printf("before clip");
  draw_bar(avg_pre_clip,x,y-2,10,32767);

  mvmove(0,th-2);
  if(*stereo==1){
    printf("stereo on (SPACE)");
  }else{
    printf("stereo off (SPACE)");
  }
  printf(" stereo gain: %f (arrow up down)",*stereo_gain);
  mvmove(0,th-1);
  printf("sinusoidal clipper: max expected input: 32760, max output: 32760");
  refresh();
  if(c==' '){
    if(*stereo==1){
      *stereo=0;
    }else{
      *stereo=1;
    }
  }
  if(c==ARROW_UP){
    *stereo_gain=*stereo_gain+0.01;
    if(*stereo_gain>10){
      *stereo_gain=10;
    }
  }
  if(c==ARROW_DOWN){
    *stereo_gain=*stereo_gain-0.01;
    if(*stereo_gain<1.1){
      *stereo_gain=1.1;
    }

  }



}

void draw_ui(int c,float* stereo_gain,int* stereo,int avg_post_agc, int avg_pre_agc,int avg_post_clip, int avg_pre_clip,Multiband mbt1, Multiband mbt2){
  clipper_agc_screen(c,stereo_gain,avg_post_agc,avg_pre_agc,avg_post_clip,avg_pre_clip,stereo);
  
}
