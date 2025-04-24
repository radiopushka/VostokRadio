#include "alsa_pipe/alsa_pipe.h"
#include "agc/agc.c"
#include "stereo_demux.c"
#include "./clippers/sin_clip.c"
#include "./stereo/stereo_amp.c"
#include "./multiband_compressor/mbc.h"
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


int main(){
  int ch1=2;
  int ch2=2;
  int rate1=RATE;
  int rate2=RATE;
  int buffer_size=5000;
  short buffer_t[buffer_size];
  short* buffer_end=buffer_t+buffer_size;
  memset(buffer_t,0,buffer_size<<1);
  if(setup_alsa_pipe(RECORDING_IFACE,PLAYBACK_IFACE,&ch1,&ch2,&rate1,&rate2,buffer_size)==-1){
    return -1;
  }
  
  //frequency splitter and combiner
  fmux lmux=create_fmux_from_pre(5,RATE,fdef,fdef_size);
  fmux rmux=create_fmux_from_pre(5,RATE,fdef,fdef_size);

  //final low pass (this is the setting for AM 15khz)
  //pretty soon FM MPX will be supported
  afilter lpassfinal=poled_f(RATE,15000,5,0);
  afilter rpassfinal=poled_f(RATE,15000,5,0);


  //pre agc bass filter
  afilter rbassc=poled_f(RATE,200,1,1);
  afilter lbassc=poled_f(RATE,200,1,1);
  

  //multiband compression


  Multiband lmbt=create_mbt(lmux);
  Multiband rmbt=create_mbt(rmux);

  set_compressor_defaults(lmbt);
  set_compressor_defaults(rmbt);

  if(GUI==1)
    init_inputs();

  //clipper coefficient
  //this value is the maximum expected input value
  float sin_clip_c1=get_sin_clip_coeff(32760);

  int time_off=0;
  int is_silence=80000;

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
    if(get_audio(buffer_t)!=-1){
    //printf("buffer1_out\n");
      if(stereo_on==1)
        amplify_stereo_plex(buffer_t,buffer_end,stereo_gain);
      else
        demux_mono(buffer_t,buffer_end);

      float buffer;
      int count=0;
      int ch_nobass;
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
            if(count%2==0){
            
             ch_nobass=run_f(lbassc,*start);
            }else{

              ch_nobass=run_f(rbassc,*start);
            }
          
            if(avg_pre_agc<abs(*start)){
              avg_pre_agc=abs(*start);
            }
            buffer=apply_agc(*start,agc_targ,agc_speed,agc_gate,ch_nobass);
            if(avg_post_agc<abs(buffer)){
              avg_post_agc=abs(buffer);
            }

            //this value is the maximum value the clipper can reach
            buffer=sin_clip(buffer,sin_clip_c1,32760);
          }else{
            buffer=0;
          }
          if(count==0){
            //hidari
            mux(lmux,buffer);

            run_compressors(lmbt);
            //printf("%f\n",get_amplitude_at(lmbt,0));

            buffer=demux(lmux);
            

            if(avg_pre_clip<abs(buffer)){
              avg_pre_clip=abs(buffer);
            }
            buffer=sin_clip_sigmoidal(buffer,sin_clip_c1,32760);
            if(avg_post_clip<abs(buffer)){
              avg_post_clip=abs(buffer);
            }
            
            buffer=run_f(lpassfinal,buffer);
          }else{
            //migi
            mux(rmux,buffer);
            
            run_compressors(rmbt);

            buffer=demux(rmux);

             if(avg_pre_clip<abs(buffer)){
              avg_pre_clip=abs(buffer);
            }
            buffer=sin_clip_sigmoidal(buffer,sin_clip_c1,32760);
            if(avg_post_clip<abs(buffer)){
              avg_post_clip=abs(buffer);
            }

            buffer=run_f(rpassfinal,buffer);

          }
        }else{
          buffer=*start;
        }
        //*start=sin_clip_sigmoidal(buffer,sin_clip_c1,32760);
        *start=buffer;
        count=~count;
      }
      queue_audio(buffer_t);
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

  free_multiband(lmbt);
  free_multiband(rmbt);
  
  free_f(lpassfinal);
  free_f(rpassfinal);

  alsa_pipe_exit();
}
