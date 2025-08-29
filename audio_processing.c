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

int main(){
  float mt1,mt2;
  mt1 = 32767; mt2 = 32767; 
  int ch1=2;
  int ch2=2;
  int rate1=48000;
  int rate2=RATE;
  int buffer_size=40000;
  if(setup_alsa_pipe(RECORDING_IFACE,PLAYBACK_IFACE,&ch1,&ch2,&rate1,&rate2,buffer_size)==-1){
    return -1;
  }
  int input_buffer_prop = rate2/rate1;
  int i_buffer_size = buffer_size/input_buffer_prop;
  short buffer_o[buffer_size];//--

  printf("starting rates: input: %d, output: %d\n",rate1, rate2);
  
  short buffer_t[i_buffer_size];//--
  short* buffer_end=buffer_t+i_buffer_size;
  short* o_buffer_end=buffer_o+buffer_size;
  memset(buffer_t,0,i_buffer_size<<1);
  //frequency splitter and combiner
  fmux lmux=create_fmux_from_pre(5,rate1,fdef,fdef_size);
  fmux rmux=create_fmux_from_pre(5,rate1,fdef,fdef_size);

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


  //final limiter
  Limiter migi = create_limiter(FINAL_CLIP_LOOKAHEAD);
  Limiter hidari = create_limiter(FINAL_CLIP_LOOKAHEAD);
  

  //composite_clipper
  Limiter Composite_clip = NULL;
  #ifdef COMPOSITE_CLIPPER
    Composite_clip = create_limiter(COMPOSITE_CLIPPER_LOOKAHEAD);
  #endif /* ifdef COMPOSITE_CLIP */


  set_compressor_defaults(lmbt);
  set_compressor_defaults(rmbt);

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



  while(c!='q' && c!=CTRLC){
    //printf("buffer1\n");
    if(get_audio(buffer_t,i_buffer_size)!=-1){
    //printf("buffer1_out\n");
      if(stereo_on==1)
        amplify_stereo_plex(buffer_t,buffer_end,stereo_gain);
      else
        demux_mono(buffer_t,buffer_end);

      float buffer;
      int count=0;
      float ch_nobass;
      avg_pre_agc=0;
      avg_post_agc=0;

      avg_post_clip=0;
      avg_pre_clip=0;
      

      for(short* start=buffer_t;start<buffer_end;start++){
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
              #else
             ch_nobass=run_f(lbassc,buffer);

             #endif /* ifdef MACRO */
            
            }else{
             #ifdef HIGH_PASS
              ch_nobass=run_f(rbassc3,buffer);
              ch_nobass=run_f(rbassc,ch_nobass);
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
            //hidari
            mux(lmux,buffer);
            gained(lmux);

            run_compressors(lmbt);
            //printf("%f\n",get_amplitude_at(lmbt,0));

            buffer=demux(lmux);
            

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

            if(avg_post_clip<abs(buffer)){
              avg_post_clip=abs(buffer);
            }


          }else{
            //migi
            mux(rmux,buffer);
            gained(rmux);
            
            run_compressors(rmbt);

            buffer=demux(rmux);

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
            if(avg_post_clip<abs(buffer)){
              avg_post_clip=abs(buffer);
            }

          }
        }else{
          buffer=(*start);
        }
       
        *start=buffer;
        count=~count;
      }
      
      resample_up_stereo(buffer_t,buffer_o,buffer_end,input_buffer_prop);
      #ifdef MPX_ENABLE
        if(rate2 == 96000||rate2 == 192000){
          for(short* loop=buffer_o;loop<o_buffer_end;loop=loop+2){
            short* right=loop+1;
            float mpx=get_mpx_next_value(*loop,*right,rate2,PERCENT_PILOT,PERCENT_MONO,Composite_clip,COMPOSITE_CLIPPER_LOOKAHEAD_RELEASE );
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




  if(Composite_clip!=NULL){
    free_limiter(Composite_clip);
  }

  free_multiband(lmbt);
  free_multiband(rmbt);
  
  free_f(lpassfinal);
  free_f(rpassfinal);

  alsa_pipe_exit();
}
