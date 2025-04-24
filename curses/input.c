#include "curses.c"


int main(){

  init_inputs();
  int c;
  while((c=wgetch())!='q'){
  
    printf("%d\n",c);
  }
  return 0;
}
