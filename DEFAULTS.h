#ifndef DEFAULTS
#define DEFAULTS

//Evan Nikitin 2025

int fdef[]={60,250,350,1000,3000,11000,18800}; //multiband compression filters
int fdef_size=7;


float mix_stereo[]={0           ,0              ,0          ,0 ,0         ,0   ,0  };
int   lookaheads[]={4 ,  4 , 4, 4 ,4,4, 4}; // will affect phase
float def_attack[]={0.04667   ,0.25  ,  0.5  , 0.55 , 0.33 ,0.3 ,0.02};//multiband compression attack

float def_release[]={ 12 , 12  ,20 ,18.5 ,18.5 , 15  ,7}; //multiband compression release
float def_target[]={5000,4000,13500,14000,12000,12000,35000}; //multiband compression target volume
float def_m_gain[]={1.00,1.00,1.00,1.00,1.00,1.00,1.00}; //make up gain
float pre_amp[]={1.6,1,1,1,1,1,100}; //multiband compressor pre compression gain
float def_gate[]={0,0,0,0,0,0,0}; //multi band compressor gate
int bypass[]={0,0,0,0,0,0,0}; //band compression bypass
float post_amp[]={0.25,1,1,1,1,1,1}; // band compression post amplification
float effect[]={12,12,12,12,12,12,12};//ratio
float knee[]={1,1,1,1,1,1,1};//knee
float knee_release[]={0.1,0.1,0.3,0.3,0.1,0.1,0.1};//knee
//int types[]={COMP_PEAK,COMP_PEAK,COMP_PEAK,COMP_PEAK,COMP_PEAK,COMP_PEAK,COMP_PEAK};//band compression compressor types

//this maximizes loudness, you can comment this out if you are using mono
//#define MONO_COMPRESSION //turns the compressor from stereo to mono
//
//#define DYNAMIC_COMPRESSOR //makes the sound louder but ruins quality

#define MULTIBAND_COMPRESSION // comment this to disable the multiband compressor

//#define BYPASS //uncomment this to bypass compressor chain


#define EXPANDER_RATIO 0
#define EXPANDER_ATTACK 0.00048
#define EXPANDER_RELEASE 0.000048
#define EXPANDER_GAIN 0.0000001
#define EXPANDER_THRESHOLD 0.01

/* Not yet implemented
#define GATE
#define GATE_RELEASE 0.00001
#define GATE_ATTACK 0.001
#define GATE_THRESHOLD 2000
*/
#define PRE_CLIP_SATURATION 0.2
#define PRE_CLIP_SATURATION_LIMIT 2
#define POST_SAT_GAIN 1

#define TAPE_SAT_THRESH 82767
#define TAPE_SAT_WETNESS 0.1
#define TAPE_SAT_OFFSET 1.00
#define TAPE_SAT_DRIVE 1.00
//#define TAPE_SAT_BYPASS


#define FINAL_AMP 2.9 // can change the global gain after the multiband compressor
#define FINAL_CLIP//comment to disable and use a gain leveler instead(not recommended)
//#define FINAL_CLIP_LOOKAHEAD 100 //samples
//#define FINAL_CLIP_LOOKAHEAD_RELEASE 0.004 //release coeficient, proportional to # samples
#define SIGMOIDAL_CO 2
#define SIGMOIDAL_ATTACK  1.54335
#define SIGMOIDAL_RELEASE 2.5
#define SIGMOIDAL_BUFFER 8
#define SIGMOIDAL_KNEE 1
#define SIGMOIDAL_DRANGE -60000 //this should be near the start of the convergance to 1 or -1 of the tanh function relative to the limit
//Vostok RF AM transmitters can handle low bass pretty well, you could set this to 20hz
//most other AM transmitters require bass cut, so set this to like 70hz
//some PLLVCO based FM transmitters might also require bass cut, our current model has trouble with bass.
//poor bass response in FM transmitters is caused by the milivolts of change even after filtering, from the power supply's rectifier circuit (PLL VCOs are more sensitive than you think)
//The PLL VCO removes this noise due to its' feedback loop mechanism but this then re generates that noise waveform at the varactor diode and your audio signal will be out of phase with it.

#define HIGH_PASS // for FM transmitters that have trouble with low frequency bass
#define HIGH_PASS_CUTOFF 5 //comment the line above to disable
#define DC_REMOVAL_COEFF 0.0000001


//alsa configuration
#define RECORDING_IFACE "hw:1,1,0"
#define PLAYBACK_IFACE "hw:0,0"
#define RATE 192000 //output rate, for MPX
//the program always records with a sample rate of 48khz

// 0 is false 1 is true
#define STEREO 1
#define STEREO_GAIN 3
//#define STEREO_GAIN 3 //the stereo amplification coefficient(good setting for streams)

#define POST_AGC_GAIN 1

#define AGC_TARG 4000 //input AGC baseline target
#define AGC_LOOKAHEAD 100
#define AGC_SPEED 0.00058 //response coefficient
//#define AGC_SPEED 0 //response coefficient
//#define AGC_RELEASE 0.0000000001 //response coefficient
#define AGC_RELEASE 0.00048 //response coefficient
#define AGC_GATE 0.005

// set to 1 to show the levels in real time, 0 to keep silent
#define GUI 0 // just have this as zero, the gui doesnt work at this moment

//FM radio setings, only apply if the output sampling rate is 96khz or higher
#define MPX_ENABLE //uncomment to enable
//#define COMPOSITE_CLIPPER // recommended but likely unnesecary if using the final lookahead clipper
#define COMPOSITE_CLIPPER_LOOKAHEAD 20
#define COMPOSITE_CLIPPER_LOOKAHEAD_RELEASE 0.00006
#define PERCENT_PILOT 0.12 //percent of the signal devoted to the 19khz pilot tone
#define PERCENT_MONO 1.9 // percent of the signal devoted to mono audio
#define PERCENT_STEREO 1.9// percent of the signal devoted to mono audio
			   // sometimes if there is distortion, decreasing the percent stereo could help


#define SYNTHESIZE_MPX_REALTIME


//MPX output channels enabeled
//sometimes having multiple channels sending MPX can cause cross talk in the cable and increase distortion

#define RIGHT_MPX
//#define LEFT_MPX

#endif // !DEFAULTS
