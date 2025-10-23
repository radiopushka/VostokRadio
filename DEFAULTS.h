#ifndef DEFAULTS
#define DEFAULTS

//Evan Nikitin 2025

int fdef[]={60,250,350,1000,5000,17000}; //multiband compression filters
int fdef_size=6;


float mix_stereo[]={0           ,0              ,0          ,0 ,0         ,0     };
int   lookaheads[]={5 ,  5 , 5, 5 ,5, 5}; // will affect phase
float def_attack[]={ 140   ,15  ,  5  , 3 , 1  ,0.5};//multiband compression attack
float def_release[]={ 600 , 170  ,160 ,150 ,150   ,30}; //multiband compression release
float def_target[]={20000,20000,20000,15000,15000,19000}; //multiband compression target volume
float def_m_gain[]={1.05,1.01,1.01,1.05,1.05,1.05}; //make up gain
float pre_amp[]={2,1,1,1,1,60}; //multiband compressor pre compression gain
float def_gate[]={0,0,0,0,0,0}; //multi band compressor gate
int bypass[]={0,0,0,0,0,0}; //band compression bypass
float post_amp[]={1,1,1,1,1,1.2}; // band compression post amplification
float effect[]={8,6,4.0,8.0,9.0,8.0};//ratio
float knee[]={200,70,50,2,2,2};//knee
int types[]={COMP_PEAK,COMP_RMS,COMP_PEAK,COMP_PEAK,COMP_PEAK,COMP_PEAK};//band compression compressor types

//this maximizes loudness, you can comment this out if you are using mono
//#define MONO_COMPRESSION //turns the compressor from stereo to mono
//
//#define DYNAMIC_COMPRESSOR //makes the sound louder but ruins quality

#define MULTIBAND_COMPRESSION // comment this to disable the multiband compressor

//#define BYPASS //uncomment this to bypass compressor chain


#define EXPANDER_RATIO 0.7
#define EXPANDER_ATTACK 0.0002
#define EXPANDER_RELEASE 0.005
#define EXPANDER_GAIN 0.000001
#define EXPANDER_THRESHOLD 6000

/* Not yet implemented
#define GATE
#define GATE_RELEASE 0.00001
#define GATE_ATTACK 0.001
#define GATE_THRESHOLD 2000
*/
#define PRE_CLIP_SATURATION 0.1
#define PRE_CLIP_SATURATION_LIMIT 1.3
#define POST_SAT_GAIN 1

#define TAPE_SAT_THRESH 33767
#define TAPE_SAT_WETNESS 0.2
#define TAPE_SAT_OFFSET 1.1
#define TAPE_SAT_DRIVE 1.00
//#define TAPE_SAT_BYPASS


#define FINAL_AMP 1 // can change the global gain after the multiband compressor
#define FINAL_CLIP//comment to disable and use a gain leveler instead(not recommended)
//#define FINAL_CLIP_LOOKAHEAD 100 //samples
//#define FINAL_CLIP_LOOKAHEAD_RELEASE 0.004 //release coeficient, proportional to # samples
#define SIGMOIDAL_CO 1.6
#define SIGMOIDAL_ATTACK  32
#define SIGMOIDAL_RELEASE 32
#define SIGMOIDAL_BUFFER 20
#define SIGMOIDAL_KNEE 8000
#define SIGMOIDAL_DRANGE -52767 //this should be near the start of the convergance to 1 or -1 of the tanh function relative to the limit
//Vostok RF AM transmitters can handle low bass pretty well, you could set this to 20hz
//most other AM transmitters require bass cut, so set this to like 70hz
//some PLLVCO based FM transmitters might also require bass cut, our current model has trouble with bass.
//poor bass response in FM transmitters is caused by the milivolts of change even after filtering, from the power supply's rectifier circuit (PLL VCOs are more sensitive than you think)
//The PLL VCO removes this noise due to its' feedback loop mechanism but this then re generates that noise waveform at the varactor diode and your audio signal will be out of phase with it.

#define HIGH_PASS // for FM transmitters that have trouble with low frequency bass
#define HIGH_PASS_CUTOFF 20 //comment the line above to disable
#define DC_REMOVAL_COEFF 0.002


//alsa configuration
#define RECORDING_IFACE "hw:1,1,0"
#define PLAYBACK_IFACE "hw:0,0"
#define RATE 192000 //output rate, for MPX
//the program always records with a sample rate of 48khz

// 0 is false 1 is true
#define STEREO 1
#define STEREO_GAIN 1.5
//#define STEREO_GAIN 3 //the stereo amplification coefficient(good setting for streams)

#define POST_AGC_GAIN 1.5

#define AGC_TARG 2000 //input AGC baseline target
#define AGC_LOOKAHEAD 1000
#define AGC_SPEED 0.01 //response coefficient
//#define AGC_SPEED 0 //response coefficient
//#define AGC_RELEASE 0.0000000001 //response coefficient
#define AGC_RELEASE 0.01 //response coefficient
#define AGC_GATE 1

// set to 1 to show the levels in real time, 0 to keep silent
#define GUI 0 // just have this as zero, the gui doesnt work at this moment

//FM radio setings, only apply if the output sampling rate is 96khz or higher
#define MPX_ENABLE //uncomment to enable
//#define COMPOSITE_CLIPPER // recommended but likely unnesecary if using the final lookahead clipper
#define COMPOSITE_CLIPPER_LOOKAHEAD 20
#define COMPOSITE_CLIPPER_LOOKAHEAD_RELEASE 0.00006
#define PERCENT_PILOT 0.15 //percent of the signal devoted to the 19khz pilot tone
#define PERCENT_MONO 0.85 // percent of the signal devoted to mono audio
#define PERCENT_STEREO 0.70// percent of the signal devoted to mono audio
			   // sometimes if there is distortion, decreasing the percent stereo could help


#define SYNTHESIZE_MPX_REALTIME


//MPX output channels enabeled
//sometimes having multiple channels sending MPX can cause cross talk in the cable and increase distortion

#define RIGHT_MPX
//#define LEFT_MPX

#endif // !DEFAULTS
