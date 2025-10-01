# AM/FM Audio Processor with MPX Encoder

A high-performance, low-latency AM/FM audio processor and MPX encoder written in C. This software is designed for professional radio broadcasting applications, providing multiband compression, AGC, and MPX generation with minimal CPU usage.

## Features

- **Multiband Compression**: n-band compressor with configurable parameters per band
- **Advanced AGC**: Automatic gain control with adjustable target and response
- **MPX Encoding**: Stereo encoder with pilot tone generation for FM broadcasting
- **High Efficiency**: ~10-16% CPU usage on Intel i5 12th gen at 192kHz
- **ALSA Support**: Native Linux audio support
- **Lookahead Clipping**: Advanced clipping prevention algorithm
- **FM Composite Clipping**: boosts loudness by sacrificing stereo information for less distortion and louder sound. Uses hyperbolic waveshaping function tanh.
- **Advanced Clipper**: fast lookahead limiter works in conjunction with the clipper to minimize distortion

## Installation & Compilation

### Prerequisites
- GCC compiler
- ALSA development libraries
- Must be running Linux, no Mac or FreeBSD. Windows is out of question.
- Linux user must be in audio group
- Pulseaudio must be off if you are generating MPX signals (otherwise you can have it on and use it)
- 192khz S32 capable sound card (most laptop sound cards)

### Building
1. Clone the repository:
   ```bash
   git clone https://github.com/radiopushka/RadioProcessor
   cd RadioProcessor
   ```

2. Configure the processor by editing `defaults.h`:
   ```bash
   nvim DEFAULTS.h  # or use your preferred editor
   ```
   Adjust the parameters according to your needs (see Configuration section below).

3. Compile the program:
   ```bash
   make
   ```
   

4. Run the processor:
   ```bash
   ./touhouradio
   ```

## Configuration

As of right now, all configuration is done by modifying `DEFAULTS.h` and recompiling the program. 
The plan is to make a user friendly command line UI eventually.
Here's a detailed but outdated explanation of the parameters:

### Multiband Compression
```c
int fdef[]={190,500,3000,7000,15000}; // Frequency bands for multiband compression
int fdef_size=5; // Number of bands

float def_attack[]={0.0000000001,0.000001,0.0001,0.0002,0.001}; // Attack times per band
float def_release[]={0.00000000000001,0.000000001,0.000003,0.000003,0.0005}; // Release times per band
float def_target[]={15000,17000,17000,17000,24000}; // Target volume per band
float def_m_gain[]={400,400,400,400,400}; // Maximum gain per band
float pre_amp[]={1,2,1,4,40}; // Pre-compression gain per band
float def_gate[]={6000,3000,4000,4000,4000}; // Noise gate threshold per band
int bypass[]={0,0,0,0,0}; // Bypass bands (1 to bypass, 0 to process)
float post_amp[]={1,1,1,1,1}; // Post-compression gain per band
int types[]={COMP_RMS,COMP_RMS,COMP_PEAK,COMP_PEAK,COMP_PEAK}; // Compressor types
```

### Global Settings
```c
#define FINAL_AMP 1 // Global gain after multiband compression
#define FINAL_CLIP // Enable final clipping (comment to disable)
#define FINAL_CLIP_LOOKAHEAD 1000 // Lookahead samples for clipping prevention
#define FINAL_CLIP_LOOKAHEAD_RELEASE 0.0001 // Release coefficient for clipper
```

### ALSA Configuration
```c
#define RECORDING_IFACE "default" // Input interface
#define PLAYBACK_IFACE "default"  // Output interface
#define RATE 192000 // Output sample rate (for MPX)
```

### Stereo & AGC Settings
```c
#define STEREO 1 // Enable stereo processing (1) or mono (0)
#define STEREO_GAIN 1.6 // Stereo amplification coefficient
#define AGC_TARG 12000 // AGC target level
#define AGC_SPEED 0.0001 // AGC response coefficient
#define AGC_GATE 3000 // AGC noise gate threshold
```

### FM MPX Settings
```c
//#define MPX_ENABLE // Uncomment to enable MPX encoding
#define COMPOSITE_CLIPPER // Enable composite clipper
#define COMPOSITE_CLIPPER_LOOKAHEAD 100 // Lookahead samples
#define COMPOSITE_CLIPPER_LOOKAHEAD_RELEASE 0.001 // Release coefficient
#define PERCENT_PILOT 0.09 // Pilot tone percentage (19kHz)
#define PERCENT_MONO 0.45 // Mono signal percentage
```

### GUI Settings
```c
#define GUI 0 // Enable GUI (currently non-functional)
```

## Usage Notes

1. The program always records at 48kHz but outputs at the rate specified by `RATE` (192kHz recommended for MPX).

2. For FM broadcasting with MPX:
   - Uncomment `#define MPX_ENABLE`
   - Set `RATE` to 192000
   - Ensure `STEREO` is set to 1

3. For AM broadcasting:
   - Comment out `#define MPX_ENABLE`
   - Set `STEREO` to 0
   - You can use a lower output sample rate

4. The compressor types can be:
   - `COMP_RMS`: RMS-based compression (smoother)
   - `COMP_PEAK`: Peak-based compression (more aggressive)
5. The Program already comes pre tuned for Pop music

## Pre-Tune
   - Tested and tuned on the Vostok Telecom hikari FM transmitter for pop music.

## Performance

The processor is highly optimized:
- ~10-16% CPU usage on one thread of an Intel i5 12th gen
- ~30-40% CPU usage on one thread of an Intel i5 6200U
- ~50-65% CPU usage on one thread of an Intel i5 520M and slightly faster on Intel i5 2nd gen
- 100% CPU usage on one thread of an old Intel Atom notebook processor.
- Processes audio at 192kHz sample rate
- Low latency processing suitable for live broadcasting

## Troubleshooting

1. If you get ALSA errors, check your audio interfaces with `arecord -L` and `aplay -L` and update `RECORDING_IFACE`/`PLAYBACK_IFACE` accordingly.

2. If experiencing distortion:
   - Reduce `FINAL_AMP`
   - Adjust compressor settings
   - Check that input levels are appropriate
   - Play with the pre clip limiter
   - Use larger lookaheads

3. For CPU usage issues:
   - Reduce `RATE` (if not using MPX)
   - Simplify compression settings
   - Use smaller lookaheads

## License

GPL, use at your own risk

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## Disclaimer

This software is intended for educational and experimental purposes. Ensure compliance with local regulations when transmitting radio signals.

<img width="1628" height="1091" alt="2025-09-24_10-55" src="https://github.com/user-attachments/assets/22844413-17af-4eae-ae71-2a6e21b36332" />
with these settings:
<img width="1867" height="790" alt="2025-09-27_23-45" src="https://github.com/user-attachments/assets/193cc903-ca14-4493-b557-b86169f94058" />
<img width="841" height="318" alt="2025-09-27_23-46" src="https://github.com/user-attachments/assets/846420e5-182f-4814-8d9f-c03e5f9aab91" />


