#ifndef DEFAULTS
#define DEFAULTS


int fdef[]={190,600,3000,7000,16000}; //multiband compression filters
int fdef_size=5;

float mix_stereo[]={0           ,0              ,0          ,0          ,0     };
float def_attack[]={ 0.000000002      ,0.0000001       ,0.0000001     ,0.000012   ,0.00002};//multiband compression attack
float def_release[]={0.00000000000001  ,0.000000001  ,0.00000001   ,0.000002   ,0.00001}; //multiband compression release
float def_target[]={5000,15500,15600,15700,9000}; //multiband compression target volume 
float def_m_gain[]={1,1,1.5,1.7,2}; //make up gain
float pre_amp[]={0.7,1,1,2,30}; //multiband compressor pre compression gain
float def_gate[]={0,0,0,0,0}; //multi band compressor gate
int bypass[]={0,0,0,0,0}; //band compression bypass
float post_amp[]={0.9,1,1,1,1}; // band compression post amplification
float effect[]={0.5,0.6,0.4,0.4,0.7};//ratio, 1 being strongest
int types[]={COMP_RMS,COMP_RMS,COMP_RMS,COMP_RMS,COMP_RMS};//band compression compressor types
                  
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
#define FINAL_CLIP_LOOKAHEAD 100 //samples
#define FINAL_CLIP_LOOKAHEAD_RELEASE 0.001 //release coeficient, proportional to # samples
//Vostok RF AM transmitters can handle low bass pretty well, you could set this to 20hz
//most other AM transmitters require bass cut, so set this to like 70hz
//some PLLVCO based FM transmitters might also require bass cut, our current model has trouble with bass.
//poor bass response in FM transmitters is caused by the milivolts of change even after filtering, from the power supply's rectifier circuit (PLL VCOs are more sensitive than you think)
//The PLL VCO removes this noise due to its' feedback loop mechanism but this then re generates that noise waveform at the varactor diode and your audio signal will be out of phase with it.

#define HIGH_PASS // for FM transmitters that have trouble with low frequency bass
#define HIGH_PASS_CUTOFF 15 //comment the line above to disable
#define DC_REMOVAL_COEFF 0.01


//alsa configuration
#define RECORDING_IFACE "hw:1,1,0"
#define PLAYBACK_IFACE "hw:0,0"
#define RATE 192000 //output rate, for MPX
//the program always records with a sample rate of 48khz

// 0 is false 1 is true
#define STEREO 1
#define STEREO_GAIN 1.5
//#define STEREO_GAIN 3 //the stereo amplification coefficient(good setting for streams) 
			 

#define AGC_TARG 8000 //input AGC baseline target

#define AGC_SPEED 0.000001 //response coefficient
//#define AGC_SPEED 0 //response coefficient
//#define AGC_RELEASE 0.0000000001 //response coefficient
#define AGC_RELEASE 0.000003 //response coefficient
#define AGC_GATE 0

// set to 1 to show the levels in real time, 0 to keep silent
#define GUI 0 // just have this as zero, the gui doesnt work at this moment

//FM radio setings, only apply if the output sampling rate is 96khz or higher
#define MPX_ENABLE //uncomment to enable
//#define COMPOSITE_CLIPPER // recommended but likely unnesecary if using the final lookahead clipper
#define COMPOSITE_CLIPPER_LOOKAHEAD 20
#define COMPOSITE_CLIPPER_LOOKAHEAD_RELEASE 0.00006
#define PERCENT_PILOT 0.12 //percent of the signal devoted to the 19khz pilot tone
#define PERCENT_MONO 0.88 // percent of the signal devoted to mono audio
#define PERCENT_STEREO 0.70// percent of the signal devoted to mono audio
			   // sometimes if there is distortion, decreasing the percent stereo could help


#define SYNTHESIZE_MPX_REALTIME
                           

//MPX output channels enabeled
//sometimes having multiple channels sending MPX can cause cross talk in the cable and increase distortion

#define RIGHT_MPX
//#define LEFT_MPX

#endif // !DEFAULTS
