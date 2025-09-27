#include "alsa_pipe/alsa_pipe.h"
#include "agc/agc.c"
#include "stereo_demux.c"
#include "./clippers/sin_clip.c"
#include "./clippers/sigmoidal.c"
#include "./stereo/stereo_amp.c"
#include "./multiband_compressor/mbc.h"
#include "./lookahead_limiter/lookaheadlim.h"
#include "./MPX/generator.h"
#include "./downward_expander/dxpander.h"
#include "ui.c"
#include "DEFAULTS.h"

//Evan Nikitin 2025

void set_compressor_defaults(Multiband mbt){

    int size=fdef_size;
    for(int i=0;i<size;i++){
      //convert attack and release based on rate
      set_attack(mbt,i,48000.0/(def_attack[i]*48000000.0));
      set_release(mbt,i,48000.0/(def_release[i]*48000000.0));
      set_target(mbt,i,def_target[i]);
      set_gate(mbt,i,def_gate[i]);
      set_max_gain(mbt,i,def_m_gain[i]);
      set_post_amp(mbt,i,post_amp[i]);
      set_bypass(mbt,i,bypass[i]);
      set_type(mbt,i,types[i]);
      set_ratio(mbt,i,1.0-(1.0/effect[i]));
      set_knee(mbt,i,knee[i]);
      
    }
}


void gained(fmux muxf){
  for(int i=0; i<fdef_size;i++){
   double val = power_at(muxf,i);
   set_power_at(muxf,i,val*pre_amp[i]);
  }
}

fmux lmux;
fmux rmux;
//for mono compression in stereo audio
double* pvals;
double* gains;
double h_compressor_left(double signal,double gain,int location){

  #ifndef MONO_COMPRESSION
    return gain;
  #endif /* ifdef MONO_COMPRESSION */
  gains[location]=gain;
  double amplitude=power_at(lmux,location)/2;
  pvals[location]=signal*gain;
  if(mix_stereo[location]==1){

    set_power_at(lmux,location, pvals[location]);
  }else{
    set_power_at(lmux,location, amplitude*gain);
  }
  return amplitude;
}

int main(){
  float mt1,mt2;
  mt1 = 32767; mt2 = 32767; 
  int ch1=2;
  int ch2=2;
  int rate1=48000;
  int rate2=RATE;
  int buffer_size=50000;

  #ifdef MPX_ENABLE
    init_mpx(rate2,PERCENT_PILOT,2147483640);
  #endif 

  int gen_rate=rate2;
  if(setup_alsa_pipe(RECORDING_IFACE,PLAYBACK_IFACE,&ch1,&ch2,&rate1,&rate2,buffer_size)==-1){
    return -1;
  }
  
  if(gen_rate!=rate2){

    #ifdef MPX_ENABLE
    
        init_mpx(rate2,PERCENT_PILOT,2147483640);
    #endif 
  }
  int input_buffer_prop = rate2/rate1;
  int i_buffer_size = buffer_size/input_buffer_prop;
  int buffer_o[buffer_size];//output buffer

  printf("starting rates: input: %d, output: %d\n",rate1, rate2);
  
  int buffer_t[i_buffer_size];//input buffer
  double buffer_tf[i_buffer_size];//input buffer
  int helper_buffer[i_buffer_size];
  int* helper_buffer_end=helper_buffer+i_buffer_size;
  int* buffer_end=buffer_t+i_buffer_size;
  double* buffer_endf=buffer_tf+i_buffer_size;
  int* o_buffer_end=buffer_o+buffer_size;
  memset(buffer_t,0,i_buffer_size<<1);
  //frequency splitter and combiner
  lmux=create_fmux_from_pre(5,rate1,fdef,fdef_size);
  rmux=create_fmux_from_pre(5,rate1,fdef,fdef_size);
  fmux mmux=create_fmux_from_pre(5,rate1,fdef,fdef_size);//mono mux

  //final low pass (this is the setting for AM 15khz)
  //pretty soon FM MPX will be supported
  afilter lpassfinal=poled_f(rate1,17000,5,0);
  afilter rpassfinal=poled_f(rate1,17000,5,0);


  //pre agc bass filter
  afilter rbassc=poled_f(rate1,200,1,1);
  afilter lbassc=poled_f(rate1,200,1,1);


  afilter rbassc2=poled_f(rate1,HIGH_PASS_CUTOFF,4,1);
  afilter lbassc2=poled_f(rate1,HIGH_PASS_CUTOFF,4,1);

  afilter rbassc3=poled_f(rate1,HIGH_PASS_CUTOFF,4,1);
  afilter lbassc3=poled_f(rate1,HIGH_PASS_CUTOFF,4,1);
  

  //downward expander
  Dexpander dxr = create_downward_expander(EXPANDER_ATTACK,EXPANDER_RELEASE,EXPANDER_RATIO,EXPANDER_THRESHOLD);
  Dexpander dxl = create_downward_expander(EXPANDER_ATTACK,EXPANDER_RELEASE,EXPANDER_RATIO,EXPANDER_THRESHOLD);


  //multiband compression


  Multiband lmbt=create_mbt(lmux,lookaheads);
  Multiband rmbt=create_mbt(rmux,lookaheads);
  Multiband mmbt=create_mbt(mmux,lookaheads);//mono

  SLim sigmoidal1 = create_sigmoidal_limiter(SIGMOIDAL_BUFFER,SIGMOIDAL_CO,31767,SIGMOIDAL_DRANGE,SIGMOIDAL_ATTACK,SIGMOIDAL_RELEASE);
  SLim sigmoidal2 = create_sigmoidal_limiter(SIGMOIDAL_BUFFER,SIGMOIDAL_CO,31767,SIGMOIDAL_DRANGE,SIGMOIDAL_ATTACK,SIGMOIDAL_RELEASE);

  //final limiter
  /*Limiter migi = create_limiter(FINAL_CLIP_LOOKAHEAD);
  Limiter hidari = create_limiter(FINAL_CLIP_LOOKAHEAD);
  Limiter migi_h = create_limiter(FINAL_CLIP_LOOKAHEAD);
  Limiter hidari_h = create_limiter(FINAL_CLIP_LOOKAHEAD);
*/

  //composite_clipper
  Limiter Composite_clip = NULL;
  #ifdef COMPOSITE_CLIPPER
    Composite_clip = create_limiter(COMPOSITE_CLIPPER_LOOKAHEAD);
  #endif /* ifdef COMPOSITE_CLIP */


  set_compressor_defaults(lmbt);
  set_compressor_defaults(rmbt);
  set_compressor_defaults(mmbt);

  if(GUI==1)
    init_inputs();

  //clipper coefficient
  //this value is the maximum expected input value
  float sin_clip_c1=get_sin_clip_coeff(32760);

  int time_off=0;
  int is_silence=20000;

  int avg_post_agc=0;
  int avg_pre_agc=0;

  int avg_post_clip=0;
  int avg_pre_clip=0;
  int c=0;

  float stereo_gain=STEREO_GAIN;

  double agc_targ=AGC_TARG;
  double agc_speed=AGC_SPEED;
  double agc_thresh=AGC_GATE;

  int stereo_on=STEREO;

  double local_right=0;
  int taken_sample=0;



  gains=malloc(sizeof(double)*fdef_size);
  pvals=malloc(sizeof(double)*fdef_size);

  int process_zeros=1;

  while(c!='q' && c!=CTRLC){
    //printf("buffer1\n");
    if(get_audio(buffer_t,i_buffer_size)!=-1){
    //printf("buffer1_out\n");
       //convert to float
      double* ittr=buffer_tf;
      for(int* pl=buffer_t;pl<buffer_end;pl++){
        *ittr=(((double)*pl) / 65538.0 );
        ittr++;
      }

      #ifndef BYPASS
           
      if(stereo_on==1){
        if(stereo_gain!=1){
          amplify_stereo_plex(buffer_tf,buffer_endf,stereo_gain);
        }
      }else
        demux_mono(buffer_tf,buffer_endf);
      #endif /* ifndef BYPASS */

      double buffer;
      int count=0;
      float ch_nobass;
      avg_pre_agc=0;
      avg_post_agc=0;

      avg_post_clip=0;
      avg_pre_clip=0;
      

      int* helper_dr=helper_buffer;
      for(double* start=buffer_tf;start<buffer_endf;start++){
        #ifndef BYPASS
        
        if(*start==0){
          if(time_off<=is_silence){
            time_off++;
          }
        }else{
          time_off=0;
        }
        if(time_off<is_silence && !(process_zeros==-1 && *start ==0)){
          if(*start!=0){
            buffer = *start;
            
            if(count%2==0){
            

		    /*dc_removal_l2 = dc_removal_l2 + (buffer- dc_removal_l2)*DC_REMOVAL_COEFF;
              buffer=buffer - dc_removal_l2;*/
             #ifdef HIGH_PASS

              buffer=run_f(lbassc3,buffer);
              ch_nobass=run_f(lbassc,buffer);


              #else
             ch_nobass=run_f(lbassc,buffer);

             #endif /* ifdef MACRO */
            
              
            }else{

		    /*dc_removal_r2 = dc_removal_r2 + (buffer- dc_removal_r2)*DC_REMOVAL_COEFF;
              buffer=buffer - dc_removal_r2;*/

             #ifdef HIGH_PASS

              buffer=run_f(rbassc3,buffer);
              ch_nobass=run_f(rbassc,buffer);


             #else
              ch_nobass=run_f(rbassc,buffer);
             #endif /* ifdef MACRO */

            }


          
            if(avg_pre_agc<abs(*start)){
              avg_pre_agc=abs(*start);
            }
            buffer=apply_agc(buffer,agc_targ,agc_speed,agc_thresh,ch_nobass,AGC_RELEASE);
            
            if(avg_post_agc<abs(buffer)){
              avg_post_agc=abs(buffer);
            }

            /*if(count%2==0){
            //remove the AGC artifacts
            dc_removal_l = dc_removal_l + (buffer- dc_removal_l)*DC_REMOVAL_COEFF;
            dc_removal_l = dc_removal_l + (buffer- dc_removal_l)*DC_REMOVAL_COEFF;
              buffer=buffer - dc_removal_l;
            }else{
            dc_removal_r = dc_removal_r + (buffer- dc_removal_r)*DC_REMOVAL_COEFF;
              buffer=buffer - dc_removal_r;
            }*/

            //this value is the maximum value the clipper can reach
            #ifdef DYNAMIC_COMPRESSOR
            
            buffer=dynamic_compressor(buffer,1);
            #endif /* ifdef DYNAMIC_COMPRESSOR */
            buffer = buffer * POST_AGC_GAIN;
          }else{
            buffer=*start;
            buffer = buffer * POST_AGC_GAIN;
          }
          if(count==0){
            #ifdef MULTIBAND_COMPRESSION
             buffer = apply_expander(dxr,buffer,EXPANDER_GAIN);
             mux(lmux,buffer);
             gained(lmux);

            #ifdef MONO_COMPRESSION
              double value=(local_right+buffer)/2.0;
             
               mux(mmux,value);
               gained(mmux);
               run_compressors_advanced(mmbt,h_compressor_left);
              //printf("%f\n",get_amplitude_at(lmbt,0));

              taken_sample=1;

            #else
             
              run_compressors(lmbt);
              //printf("%f\n",get_amplitude_at(lmbt,0));


            #endif /* ifdef MACRO */
            buffer=demux(lmux) * FINAL_AMP;
                      

            #endif
            if(avg_pre_clip<abs(buffer)){
              avg_pre_clip=abs(buffer);
            }
                       
          
            #ifdef FINAL_CLIP 
              //buffer=run_limiter(hidari,buffer*FINAL_AMP,30000,FINAL_CLIP_LOOKAHEAD_RELEASE );
              //buffer=sigmoidal_clipper_tanh(buffer*FINAL_AMP,31000,SIGMOIDAL_CO);

            #else
              buffer=sin_clip_bouncy(buffer,sin_clip_c1,32767,&mt1);
            #endif 

            buffer=run_f(lpassfinal,buffer);
            #ifdef HIGH_PASS
              buffer=run_f(lbassc2,buffer);
            #endif /* ifdef MACRO */
           #ifdef FINAL_CLIP 
              //buffer=run_limiter(hidari_h,buffer*FINAL_AMP,31760,FINAL_CLIP_LOOKAHEAD_RELEASE );
              //buffer=sigmoidal_clipper_tanh(buffer*FINAL_AMP,31760,SIGMOIDAL_CO,&clip_tracker1);
              buffer=apply_sigmoidal(sigmoidal1,buffer);
            #endif

            if(avg_post_clip<abs(buffer)){
              avg_post_clip=abs(buffer);
            }


          }else{
            #ifdef MULTIBAND_COMPRESSION
            
            buffer = apply_expander(dxl,buffer,EXPANDER_GAIN);
            //migi
            mux(rmux,buffer);
            gained(rmux);

            #ifdef MONO_COMPRESSION
              float copy=buffer;
              if(taken_sample==1){
                  for(int i=0;i<fdef_size;i++){
                    
                    if(mix_stereo[i]==1){

                      set_power_at(rmux,i, pvals[i]);
                    }else{
                      float amplitude=power_at(rmux,i);
                      set_power_at(rmux,i, amplitude*gains[i]);
                    }
                  }
              }
              taken_sample=0;
              local_right=copy;
            #else
              run_compressors(rmbt);
            #endif /* ifdef MONO_COMPRESSION */
            

            buffer=demux(rmux) * FINAL_AMP;

            #endif /* ifdef MULTIBAND_COMPRESSION */
             if(avg_pre_clip<abs(buffer)){
              avg_pre_clip=abs(buffer);
            }
         
            #ifdef FINAL_CLIP 
              //buffer=run_limiter(migi,buffer*FINAL_AMP,30000,FINAL_CLIP_LOOKAHEAD_RELEASE );
              //buffer=sigmoidal_clipper_tanh(buffer*FINAL_AMP,31000,SIGMOIDAL_CO);
            #else
              buffer=sin_clip_bouncy(buffer,sin_clip_c1,32767,&mt2);
            #endif 

            buffer=run_f(rpassfinal,buffer);
           #ifdef HIGH_PASS
              buffer=run_f(rbassc2,buffer);
            #endif /* ifdef MACRO */
            #ifdef FINAL_CLIP 
              //buffer=run_limiter(migi_h,buffer*FINAL_AMP,31760,FINAL_CLIP_LOOKAHEAD_RELEASE );
              //buffer=sigmoidal_clipper_tanh(buffer*FINAL_AMP,31760,SIGMOIDAL_CO,&clip_tracker2);
              buffer=apply_sigmoidal(sigmoidal2,buffer);
            #endif

            if(avg_post_clip<abs(buffer)){
              avg_post_clip=abs(buffer);
            }

          }
        }else{
          buffer=0;
        }
        #else

          buffer=(*start);
          buffer=apply_sigmoidal(sigmoidal1,buffer);
       
        #endif /* ifdef BYPASS */
        buffer = buffer * 65538.0;
        *helper_dr=buffer;
        //*helper_dr=buffer*65200.0;
        helper_dr++;
        count=~count;
      }
   
      resample_up_stereo(helper_buffer,buffer_o,helper_buffer_end,input_buffer_prop);
      #ifdef MPX_ENABLE
        if(rate2 == 96000||rate2 == 192000){
	  int* right;
          for(int* loop=buffer_o;loop<o_buffer_end;loop=loop+2){
            right=loop+1;
            
         
            double mpx=get_mpx_next_value(*loop,*right,PERCENT_STEREO,PERCENT_MONO);

		#ifdef RIGHT_MPX
            		*right=mpx;
		#else
	    		*right=0;
		#endif
		#ifdef LEFT_MPX
            		*loop=mpx;
		#else
	    		*loop=0;
		#endif
          }
      }
      
      #endif /* ifdef MPX_ENABLE */
      queue_audio(buffer_o);
      if(GUI==1){
        draw_ui(c,&stereo_gain,&stereo_on,abs(avg_post_agc),abs(avg_pre_agc),abs(avg_post_clip),abs(avg_pre_clip),lmbt,rmbt);
        c=wgetch_nblk();
      }
    }
  }


  if(GUI==1)
    deguchi_nara();

  free_f(rbassc);
  free_f(lbassc);

  free_f(rbassc2);
  free_f(lbassc2);

  free_f(rbassc3);
  free_f(lbassc3);

  free_sigmoidal(sigmoidal1);
  free_sigmoidal(sigmoidal2);
  /*free_limiter(migi);
  free_limiter(hidari);
  free_limiter(migi_h);
  free_limiter(hidari_h);*/

  free(dxr);
  free(dxl);
  
  free(gains);
  free(pvals);

  free_mpx_cache();


  if(Composite_clip!=NULL){
    free_limiter(Composite_clip);
  }

  free_multiband(lmbt);
  free_multiband(rmbt);
  free_multiband(mmbt);
  
  free_f(lpassfinal);
  free_f(rpassfinal);

  alsa_pipe_exit();
}
