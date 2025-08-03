#ifndef LOOKAHEAD_LIMITER

#define LOOKAHEAD_LIMITER

struct limiter_info{

  float* buffer;
  int buffer_size;
  float porog;
};

typedef struct limiter_info* FLimiter;

FLimiter create_limiter(int look_back,float porog);
float perform_limiter(float input,FLimiter limiter);
void clear_limiter(FLimiter limiter);

#endif // !LOOKAHEAD_LIMITER
