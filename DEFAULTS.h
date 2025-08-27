#ifndef DEFAULTS
#define DEFAULTS

//default frequency muxer:
int fdef[]={190,500,3000,7000,15000}; //multiband compression filters
int fdef_size=5;

float def_attack[]={0.0000000001      ,0.000001     ,0.0001     ,0.0002     ,0.001};//multiband compression attack
float def_release[]={0.00000000000001  ,0.000000001  ,0.000003   ,0.000003   ,0.0005}; //multiband compression release
float def_target[]={15000,17000,17000,17000,24000}; //multiband compression target volume 
float def_m_gain[]={400,400,400,400,400}; //multiband compressor max gain
float pre_amp[]={1,2,1,4,40}; //multiband compressor pre compression gain
float def_gate[]={6000,3000,4000,4000,4000}; //multi band compressor gate
int bypass[]={0,0,0,0,0}; //band compression bypass
float post_amp[]={1,1,1,1,1}; // band compression post amplification
int types[]={COMP_RMS,COMP_RMS,COMP_PEAK,COMP_PEAK,COMP_PEAK};//band compression compressor types

#define FINAL_AMP 1 // can change the global gain after the multiband compressor
#define FINAL_CLIP//comment to disable and use a gain leveler instead(not recommended)
#define FINAL_CLIP_LOOKAHEAD 1000 //samples
#define FINAL_CLIP_LOOKAHEAD_RELEASE 0.0001 //release coeficient, proportional to # samples

//alsa configuration
#define RECORDING_IFACE "default"
#define PLAYBACK_IFACE "default"
#define RATE 192000 //output rate, for MPX
//the program always records with a sample rate of 48khz

// 0 is false 1 is true
#define STEREO 1
#define STEREO_GAIN 1.6 //the stereo amplification coefficient

#define AGC_TARG 12000 //input AGC baseline target

#define AGC_SPEED 0.0001 //response coefficient
#define AGC_GATE 3000

// set to 1 to show the levels in real time, 0 to keep silent
#define GUI 0 // just have this as zero, the gui doesnt work at this moment

//FM radio setings, only apply if the output sampling rate is 96khz or higher
//#define MPX_ENABLE //uncomment to enable
#define COMPOSITE_CLIPPER // recommended but likely unnesecary if using the final lookahead clipper
#define COMPOSITE_CLIPPER_LOOKAHEAD 100
#define COMPOSITE_CLIPPER_LOOKAHEAD_RELEASE 0.001
#define PERCENT_PILOT 0.09 //percent of the signal devoted to the 19khz pilot tone
#define PERCENT_MONO 0.45 // percent of the signal devoted to mono audio



#endif // !DEFAULTS
