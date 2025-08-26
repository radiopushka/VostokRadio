#ifndef DEFAULTS
#define DEFAULTS

//default frequency muxer:
int fdef[]={190,500,3000,7000,15000};
int fdef_size=5;

float def_attack[]={0.0000000001      ,0.000001     ,0.0001     ,0.0002     ,0.01};
float def_release[]={0.00000000000001  ,0.000000001  ,0.000003   ,0.000003   ,0.005};
float def_target[]={15000,17000,17000,17000,24000};
float def_m_gain[]={400,400,400,400,400};
float pre_amp[]={1,2,1,4,40};
float def_gate[]={6000,3000,4000,4000,4000};
int bypass[]={0,0,0,0,0};
float post_amp[]={1,1,1,1,1};
int types[]={COMP_RMS,COMP_RMS,COMP_PEAK,COMP_PEAK,COMP_PEAK};

#define FINAL_AMP 1
#define FINAL_CLIP
#define FINAL_CLIP_LOOKAHEAD 1000
#define FINAL_CLIP_LOOKAHEAD_RELEASE 0.0001


#define RECORDING_IFACE "default"
#define PLAYBACK_IFACE "default"
#define RATE 192000

// 0 is false 1 is true
#define STEREO 1
#define STEREO_GAIN 1.6

#define AGC_TARG 12000

#define AGC_SPEED 0.0001
#define AGC_GATE 3000

// set to 1 to show the levels in real time, 0 to keep silent
#define GUI 0

//FM radio setings, only apply if the output sampling rate is 96khz or higher
//#define MPX_ENABLE
#define COMPOSITE_CLIPPER
#define COMPOSITE_CLIPPER_LOOKAHEAD 1000
#define COMPOSITE_CLIPPER_LOOKAHEAD_RELEASE 0.00001
#define PERCENT_PILOT 0.09
#define PERCENT_MONO 0.45



#endif // !DEFAULTS
