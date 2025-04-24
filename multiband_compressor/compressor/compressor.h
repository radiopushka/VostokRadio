
#ifndef COMPRESSOR
#define COMPRESSOR

struct Compressor{
  float avg;
  float prev_val;
  float prevprev_val;
  float gain;
  int method;
};

#define COMP_RMS 1
#define COMP_PEAK 2

typedef struct Compressor* Compressor;

//method: 1=peak detector 0=RMS
Compressor create_compressor(int method);

float run_comp(Compressor comp,float release, float attack, float target,float input,float gate,float max_gain);

#endif // !COMPRESSOR
