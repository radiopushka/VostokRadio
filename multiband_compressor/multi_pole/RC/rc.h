#ifndef RC_FILTERS
#define RC_FILTERS

struct rc_filter_info{
  double alpha;
  double prev;
  double prev_raw;
  int direction;
};


typedef struct rc_filter_info rc_filter_info;

//RC low pass and high pass filters
rc_filter_info* create_rc_filter(double frequency,int rate, int direction);
double do_rc_filter(rc_filter_info* rcf,double in);
void free_rc_filter(rc_filter_info* rcf);

#endif // !RC_FILTERS
