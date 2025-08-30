#ifndef DEFAULTS
#define DEFAULTS

//default frequency muxer:
int fdef[]={190,500,3000,7000,15000}; //multiband compression filters
int fdef_size=5;

float def_attack[]={0.000008      ,0.000001     ,0.0001     ,0.0002     ,0.001};//multiband compression attack
float def_release[]={0.0000001  ,0.000000001  ,0.000003   ,0.000003   ,0.0005}; //multiband compression release
float def_target[]={14000,17000,17000,17000,24000}; //multiband compression target volume 
float def_m_gain[]={400,400,400,400,400}; //multiband compressor max gain
float pre_amp[]={1,2,1,4,40}; //multiband compressor pre compression gain
float def_gate[]={6000,3000,4000,4000,4000}; //multi band compressor gate
int bypass[]={0,0,0,0,0}; //band compression bypass
float post_amp[]={0.5,1,1,1,1}; // band compression post amplification
int types[]={COMP_RMS,COMP_RMS,COMP_RMS,COMP_RMS,COMP_PEAK};//band compression compressor types
                
//this maximizes loudness, you can comment this out if you are using mono
//#define MONO_COMPRESSION //turns the compressor from stereo to mono
//
//#define DYNAMIC_COMPRESSOR //makes the sound louder but ruins quality

#define MULTIBAND_COMPRESSION // comment this to disable the multiband compressor

//#define BYPASS //uncomment this to bypass compressor chain



/* Not yet implemented
#define GATE
#define GATE_RELEASE 0.00001
#define GATE_ATTACK 0.001
#define GATE_THRESHOLD 2000
*/

#define FINAL_AMP 1 // can change the global gain after the multiband compressor
#define FINAL_CLIP//comment to disable and use a gain leveler instead(not recommended)
#define FINAL_CLIP_LOOKAHEAD 500 //samples
#define FINAL_CLIP_LOOKAHEAD_RELEASE 0.001 //release coeficient, proportional to # samples
//Vostok RF AM transmitters can handle low bass pretty well, you could set this to 20hz
//most other AM transmitters require bass cut, so set this to like 70hz
//some PLLVCO based FM transmitters might also require bass cut, our current model has trouble with bass.
//poor bass response in FM transmitters is caused by the milivolts of change even after filtering, from the power supply's rectifier circuit (PLL VCOs are more sensitive than you think)
//The PLL VCO removes this noise due to its' feedback loop mechanism but this then re generates that noise waveform at the varactor diode and your audio signal will be out of phase with it.

#define HIGH_PASS // for FM transmitters that have trouble with low frequency bass
#define HIGH_PASS_CUTOFF 10 //comment the line above to disable


//alsa configuration
#define RECORDING_IFACE "default"
#define PLAYBACK_IFACE "default"
#define RATE 192000 //output rate, for MPX
//the program always records with a sample rate of 48khz

// 0 is false 1 is true
#define STEREO 1
#define STEREO_GAIN 3 //the stereo amplification coefficient(good setting for streams) 

#define AGC_TARG 14000 //input AGC baseline target

#define AGC_SPEED 0.0004 //response coefficient
#define AGC_GATE 3000

// set to 1 to show the levels in real time, 0 to keep silent
#define GUI 0 // just have this as zero, the gui doesnt work at this moment

//FM radio setings, only apply if the output sampling rate is 96khz or higher
#define MPX_ENABLE //uncomment to enable
#define COMPOSITE_CLIPPER // recommended but likely unnesecary if using the final lookahead clipper
#define COMPOSITE_CLIPPER_LOOKAHEAD 4000
#define COMPOSITE_CLIPPER_LOOKAHEAD_RELEASE 0.00006
#define PERCENT_PILOT 0.09 //percent of the signal devoted to the 19khz pilot tone
#define PERCENT_MONO 0.85 // percent of the signal devoted to mono audio
#define PERCENT_STEREO 0.85// percent of the signal devoted to mono audio

//#define SYNTHESIZE_MPX_REALTIME //comment this if your CPU has a large Cache, degrades audio quality but reduces CPU load in some cases


                           



#endif // !DEFAULTS
