
#ifndef ALSA_PIPE
#define ALSA_PIPE


//initialization:
void set_latency(int latency_buffers); //must be called before setup
int setup_alsa_pipe(char* recording_iface, 
    char* playback_iface, int* channels_in,
    int* channels_out,int* input_rate,int* output_rate,
    int buffer_Size);
//recording interface, playback interface
//channels incomming, channels outgoing
//ATTENTION: IF YOU HAVE TWO CHANNELS YOUR BUFFER WILL CONTAIN 
//BOTH CHANNELS, IF YOUR BUFFER SIZE IS 1500 THEN YOU WILL HAVE
//750 FOR EACH CHANNEL. THE ORDER IN THE BUFFER IS AS FOLLOWS: 
//CHANNEL1-0
//CHANNEL2-1
//CHANNEL1-2
//CHANNEL2-4

//the data buffer should be the same size as set in setup
int get_audio(int* data,int bsize);
int queue_audio(int* data);
//puts in the next segment to be played


//free all memory and terminate the helper thread:
void alsa_pipe_exit();
#endif // !ALSA_PIPE
