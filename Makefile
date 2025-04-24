COMPRESSOR=./multiband_compressor/compressor/main.c
MUXER=./multiband_compressor/freq_mux/main.c
MULTIBAND=./multiband_compressor/main.c
MULTIPOLE=./multiband_compressor/multi_pole/multi_pole.c
RC=./multiband_compressor/multi_pole/RC/rc.c
ALSA=alsa_pipe/main.c

FLAGS=-O2 -g -march=native


all:
	$(CC) audio_processing.c $(COMPRESSOR) $(MUXER) $(MULTIBAND) $(MULTIPOLE) $(RC) $(ALSA) -lm -lasound $(FLAGS) -Wall -o touhouradio

