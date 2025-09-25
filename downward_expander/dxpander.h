#ifndef DXPANDER
  #define DXPANDER


  struct dxpander{
    double gain; 
    double attack;
    double release;
    double ratio;
    double threshold;
  };
typedef struct dxpander* Dexpander;
//negative range compression
Dexpander create_downward_expander(double attack,double release, double ratio,double threshold);

//apply negative range dynamic compression
double apply_expander(Dexpander expander,double input,double min_att);
#endif // !DXPANDER
