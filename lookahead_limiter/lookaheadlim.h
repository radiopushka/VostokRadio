#ifndef LOOKAHEAD_LIMITER
//Evan Niktin 2025
#define LOOKAHEAD_LIMITER

struct limiter{
  double* ring_buffer;
  double* ring_buffer_helper;
  double max_cache;
  double local_gain;
  int size;

};

typedef struct limiter* Limiter;

Limiter create_limiter(int size);
double run_limiter(Limiter limiter,double input,double limit,double release);
void free_limiter(Limiter limiter);


#endif // !LOOKAHEAD_LIMITER

