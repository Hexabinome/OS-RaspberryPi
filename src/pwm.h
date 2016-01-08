#ifndef PWM_H
#define PWM_H

void audio_test();
void pause(int );
void audio_init(void);

// call this method to playback an audio file. Pass int value to specify which file to play.
void playSound(int);
void audio_tick();
#endif

