#ifndef LOOKAHEAD_LIMITER

#define LOOKAHEAD_LIMITER

struct limiter{
  float* ring_buffer;
  int size;

};

typedef struct limiter* Limiter;

Limiter create_limiter(int size);
float run_limiter(Limiter limiter,float input,float limit);
void free_limiter(Limiter limiter);


#endif // !LOOKAHEAD_LIMITER

