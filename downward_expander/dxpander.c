#include "dxpander.h"

#include <math.h>
#include <stdlib.h>


Dexpander create_downward_expander(double attack,double release, double ratio,double threshold){
  Dexpander expnd = malloc(sizeof(struct dxpander));
  expnd->gain=1;
  expnd->attack=attack;
  expnd->release=release;
  expnd->ratio=ratio;
  expnd->threshold=threshold;

  return expnd;
}

double apply_expander(Dexpander expander,double input,double min_att){

   double gain = expander->gain;

   double precalc = fabs(input);

  if(precalc > 0){ // ignore off time
    if(precalc > expander->threshold){
      gain = gain * (1 + expander->release);
      if(gain>1){
        gain = 1;
      }
    }else if(precalc < expander->threshold){
      gain = gain * (1 - expander->attack);
    }

    if(gain<min_att){
      gain = min_att;
    }
    expander->gain = gain;
  }

  return (gain*expander->ratio + (1 - expander->ratio) ) * input;

}

