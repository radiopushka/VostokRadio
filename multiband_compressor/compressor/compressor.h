
#ifndef COMPRESSOR
#define COMPRESSOR
//Evan Nikitin 2025
struct Compressor{
  double avg;
  double prev_val;
  double prevprev_val;
  double gain;
  double knee;
  double ratio;
  double ring[15];
  int method;
};

#define COMP_RMS 1
#define COMP_PEAK 2

typedef struct Compressor* Compressor;

//method: 1=peak detector 0=RMS
Compressor create_compressor(int method);

double run_comp(Compressor comp,double release, double attack, double target,double input,double gate,double max_gain,int bypass);

#endif // !COMPRESSOR
