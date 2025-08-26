#ifndef LOOKAHEAD_LIMITER

#define LOOKAHEAD_LIMITER

struct limiter{
  float* ring_buffer;
  float local_gain;
  int size;

};

typedef struct limiter* Limiter;

Limiter create_limiter(int size);
float run_limiter(Limiter limiter,float input,float limit,float release);
void free_limiter(Limiter limiter);


#endif // !LOOKAHEAD_LIMITER

