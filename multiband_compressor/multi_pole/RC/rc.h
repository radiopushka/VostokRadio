#ifndef RC_FILTERS
#define RC_FILTERS

struct rc_filter_info{
  float alpha;
  float prev;
  float prev_raw;
  int direction;
};


typedef struct rc_filter_info rc_filter_info;

//RC low pass and high pass filters
rc_filter_info* create_rc_filter(float frequency,int rate, int direction);
float do_rc_filter(rc_filter_info* rcf,float in);
void free_rc_filter(rc_filter_info* rcf);

#endif // !RC_FILTERS
