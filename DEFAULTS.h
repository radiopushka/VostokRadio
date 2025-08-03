#ifndef DEFAULTS
#define DEFAULTS

//default frequency muxer:
int fdef[]={190,500,3000,6000,15000};
int fdef_size=5;

float def_attack[]={0.0000000001     ,0.000001    ,0.001    ,0.001    ,0.01};
float def_release[]={0.000000000001  ,0.00000001  ,0.0003   ,0.0003   ,0.001};
float def_target[]={15000,20000,20000,21000,22000};
float def_m_gain[]={400,400,400,400,400};
float pre_amp[]={0.5,2,1,4,18};
float def_gate[]={6000,3000,4000,4000,4000};
int bypass[]={0,0,0,0,0};
float post_amp[]={1,1,1,1,3};
int types[]={COMP_RMS,COMP_RMS,COMP_PEAK,COMP_PEAK,COMP_PEAK};

#define FINAL_AMP 1
#define FINAL_CLIP
#define FINAL_CLIP_LOOKAHEAD 500


#define RECORDING_IFACE "default"
#define PLAYBACK_IFACE "default"
#define RATE 48000

// 0 is false 1 is true
#define STEREO 1
#define STEREO_GAIN 1.6

#define AGC_TARG 15000

#define AGC_SPEED 0.00000001
#define AGC_GATE 3000

// set to 1 to show the levels in real time, 0 to keep silent
#define GUI 0


#endif // !DEFAULTS
