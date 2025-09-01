#include "alsa_pipe/alsa_pipe.h"
#include "agc/agc.c"
#include "stereo_demux.c"
#include "./clippers/sin_clip.c"
#include "./stereo/stereo_amp.c"
#include "./multiband_compressor/mbc.h"
#include "./lookahead_limiter/lookaheadlim.h"
#include "./MPX/generator.h"
#include "ui.c"
#include "DEFAULTS.h"


void set_compressor_defaults(Multiband mbt){

    int size=fdef_size;
    for(int i=0;i<size;i++){
      set_attack(mbt,i,def_attack[i]);
      set_release(mbt,i,def_release[i]);
      set_target(mbt,i,def_target[i]);
      set_gate(mbt,i,def_gate[i]);
      set_max_gain(mbt,i,def_m_gain[i]);
      set_post_amp(mbt,i,post_amp[i]);
      set_bypass(mbt,i,bypass[i]);
      set_type(mbt,i,types[i]);
      
    }
}


void gained(fmux muxf){
  for(int i=0; i<fdef_size;i++){
   float val = power_at(muxf,i);
   set_power_at(muxf,i,val*pre_amp[i]);
  }
}

fmux lmux;
fmux rmux;
//for mono compression in stereo audio
float* pvals;
float* gains;
float h_compressor_left(float signal,float gain,int location){

  gains[location]=gain;
  float amplitude=power_at(lmux,location);
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
  if(setup_alsa_pipe(RECORDING_IFACE,PLAYBACK_IFACE,&ch1,&ch2,&rate1,&rate2,buffer_size)==-1){
    return -1;
  }
  int input_buffer_prop = rate2/rate1;
  int i_buffer_size = buffer_size/input_buffer_prop;
  int buffer_o[buffer_size];//output buffer

  printf("starting rates: input: %d, output: %d\n",rate1, rate2);
  
  short buffer_t[i_buffer_size];//input buffer
  float buffer_tf[i_buffer_size];//input buffer
  int helper_buffer[i_buffer_size];
  int* helper_buffer_end=helper_buffer+i_buffer_size;
  short* buffer_end=buffer_t+i_buffer_size;
  float* buffer_endf=buffer_tf+i_buffer_size;
  int* o_buffer_end=buffer_o+buffer_size;
  memset(buffer_t,0,i_buffer_size<<1);
  //frequency splitter and combiner
  lmux=create_fmux_from_pre(5,rate1,fdef,fdef_size);
  rmux=create_fmux_from_pre(5,rate1,fdef,fdef_size);
  fmux mmux=create_fmux_from_pre(5,rate1,fdef,fdef_size);//mono mux

  //final low pass (this is the setting for AM 15khz)
  //pretty soon FM MPX will be supported
  afilter lpassfinal=poled_f(rate1,15000,5,0);
  afilter rpassfinal=poled_f(rate1,15000,5,0);


  //pre agc bass filter
  afilter rbassc=poled_f(rate1,200,1,1);
  afilter lbassc=poled_f(rate1,200,1,1);


  afilter rbassc2=poled_f(rate1,HIGH_PASS_CUTOFF,4,1);
  afilter lbassc2=poled_f(rate1,HIGH_PASS_CUTOFF,4,1);

  afilter rbassc3=poled_f(rate1,HIGH_PASS_CUTOFF,4,1);
  afilter lbassc3=poled_f(rate1,HIGH_PASS_CUTOFF,4,1);
  

  //multiband compression


  Multiband lmbt=create_mbt(lmux);
  Multiband rmbt=create_mbt(rmux);
  Multiband mmbt=create_mbt(mmux);//mono


  //final limiter
  Limiter migi = create_limiter(FINAL_CLIP_LOOKAHEAD);
  Limiter hidari = create_limiter(FINAL_CLIP_LOOKAHEAD);
  Limiter migi_h = create_limiter(FINAL_CLIP_LOOKAHEAD);
  Limiter hidari_h = create_limiter(FINAL_CLIP_LOOKAHEAD);

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
  int is_silence=200000;

  int avg_post_agc=0;
  int avg_pre_agc=0;

  int avg_post_clip=0;
  int avg_pre_clip=0;
  int c=0;

  float stereo_gain=STEREO_GAIN;

  float agc_targ=AGC_TARG;
  float agc_speed=AGC_SPEED;
  float agc_gate=AGC_GATE;

  int stereo_on=STEREO;

  float local_right=0;
  int taken_sample=0;

  float dc_removal_l=0;
  float dc_removal_r=0;

  gains=malloc(sizeof(float)*fdef_size);
  pvals=malloc(sizeof(float)*fdef_size);


  while(c!='q' && c!=CTRLC){
    //printf("buffer1\n");
    if(get_audio(buffer_t,i_buffer_size)!=-1){
    //printf("buffer1_out\n");
       //convert to float
      float* ittr=buffer_tf;
      for(short* pl=buffer_t;pl<buffer_end;pl++){
        *ittr=*pl;
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

      float buffer;
      int count=0;
      float ch_nobass;
      avg_pre_agc=0;
      avg_post_agc=0;

      avg_post_clip=0;
      avg_pre_clip=0;
      

      int* helper_dr=helper_buffer;
      for(float* start=buffer_tf;start<buffer_endf;start++){
        #ifndef BYPASS
        
        if(*start==0){
          if(time_off<=is_silence){
            time_off++;
          }
        }else{
          time_off=0;
        }
        if(time_off<is_silence){
          if(*start!=0){
            buffer = *start;
            
            if(count%2==0){
            
             #ifdef HIGH_PASS
              ch_nobass=run_f(lbassc3,buffer);
              ch_nobass=run_f(lbassc,ch_nobass);

               dc_removal_l = dc_removal_l + (buffer- dc_removal_l)*0.00001;
              buffer=buffer - dc_removal_l;

              #else
             ch_nobass=run_f(lbassc,buffer);

             #endif /* ifdef MACRO */
            
              
            }else{
             #ifdef HIGH_PASS
              ch_nobass=run_f(rbassc3,buffer);
              ch_nobass=run_f(rbassc,ch_nobass);

              dc_removal_r = dc_removal_r + (buffer- dc_removal_r)*0.00001;
              buffer=buffer - dc_removal_r;

             #else
              ch_nobass=run_f(rbassc,buffer);
             #endif /* ifdef MACRO */

            }


          
            if(avg_pre_agc<abs(*start)){
              avg_pre_agc=abs(*start);
            }
            buffer=apply_agc(buffer,agc_targ,agc_speed,agc_gate,ch_nobass);
            
            if(avg_post_agc<abs(buffer)){
              avg_post_agc=abs(buffer);
            }

            //this value is the maximum value the clipper can reach
            #ifdef DYNAMIC_COMPRESSOR
            
            buffer=dynamic_compressor(buffer,1);
            #endif /* ifdef DYNAMIC_COMPRESSOR */
          }else{
            buffer=0;
          }
          if(count==0){
            #ifdef MULTIBAND_COMPRESSION
             mux(lmux,buffer);
             gained(lmux);

            #ifdef MONO_COMPRESSION
              float value=(local_right+buffer)/2.0;
             
               mux(mmux,value);
               gained(mmux);
               run_compressors_advanced(mmbt,h_compressor_left);
              //printf("%f\n",get_amplitude_at(lmbt,0));

              taken_sample=1;

            #else
             
              run_compressors(lmbt);
              //printf("%f\n",get_amplitude_at(lmbt,0));


            #endif /* ifdef MACRO */
            buffer=demux(lmux);
                      

            #endif
            if(avg_pre_clip<abs(buffer)){
              avg_pre_clip=abs(buffer);
            }
                       
          
            #ifdef FINAL_CLIP 
              buffer=run_limiter(hidari,buffer*FINAL_AMP,32760,FINAL_CLIP_LOOKAHEAD_RELEASE );
            #else
              buffer=sin_clip_bouncy(buffer * FINAL_AMP,sin_clip_c1,32767,&mt1);
            #endif 

            buffer=run_f(lpassfinal,buffer);
            #ifdef HIGH_PASS
              buffer=run_f(lbassc2,buffer);
            #endif /* ifdef MACRO */
           #ifdef FINAL_CLIP 
              buffer=run_limiter(hidari_h,buffer*FINAL_AMP,32760,FINAL_CLIP_LOOKAHEAD_RELEASE );
            #endif

            if(avg_post_clip<abs(buffer)){
              avg_post_clip=abs(buffer);
            }


          }else{
            #ifdef MULTIBAND_COMPRESSION
            
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
            

            buffer=demux(rmux);

            #endif /* ifdef MULTIBAND_COMPRESSION */
             if(avg_pre_clip<abs(buffer)){
              avg_pre_clip=abs(buffer);
            }
         
            #ifdef FINAL_CLIP 
              buffer=run_limiter(migi,buffer*FINAL_AMP,32760,FINAL_CLIP_LOOKAHEAD_RELEASE );
            #else
              buffer=sin_clip_bouncy(buffer * FINAL_AMP,sin_clip_c1,32767,&mt2);
            #endif 

            buffer=run_f(rpassfinal,buffer);
           #ifdef HIGH_PASS
              buffer=run_f(rbassc2,buffer);
            #endif /* ifdef MACRO */
            #ifdef FINAL_CLIP 
              buffer=run_limiter(migi_h,buffer*FINAL_AMP,32760,FINAL_CLIP_LOOKAHEAD_RELEASE );
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
       
        #endif /* ifdef BYPASS */
        *helper_dr=buffer*65538.0;
        helper_dr++;
        count=~count;
      }
   
      resample_up_stereo(helper_buffer,buffer_o,helper_buffer_end,input_buffer_prop);
      #ifdef MPX_ENABLE
        if(rate2 == 96000||rate2 == 192000){
          for(int* loop=buffer_o;loop<o_buffer_end;loop=loop+2){
            int* right=loop+1;
            
            int synth=-1;

            #ifdef SYNTHESIZE_MPX_REALTIME
              synth=1;
            
            #endif /* ifdef SYNTHESIZE_MPX_REALTIME */

            float mpx=get_mpx_next_value(*loop,*right,rate2,PERCENT_PILOT,PERCENT_STEREO,PERCENT_MONO,Composite_clip,COMPOSITE_CLIPPER_LOOKAHEAD_RELEASE, 2147483640,synth);

            *right=mpx;
            *loop=mpx;
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

  free_limiter(migi);
  free_limiter(hidari);
  free_limiter(migi_h);
  free_limiter(hidari_h);

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
