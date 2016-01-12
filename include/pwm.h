#ifndef PWM_H
#define PWM_H

#define SOUND_IDX_BEGIN 0
#define SOUND_IDX_END 25

void audio_test();
void pause(int);
void audio_init(void);

// call this method to playback an audio file. Pass 0 or 1 as param to playback songs. Pass any value between 2 and 25 to playback synth tones. 
void playSound(int);
void audio_tick();
#endif

