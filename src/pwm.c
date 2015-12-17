#include "pwm.h"
#include "hw.h"

extern char _binary_sounds_tune_wav_start;
extern char _binary_sounds_B4_wav_start;
extern char _binary_sounds_C1_1_wav_start;
extern char _binary_sounds_D_4_wav_start;
extern char _binary_sounds_E1_wav_start;
extern char _binary_sounds_F3_wav_start;
extern char _binary_sounds_G6_wav_start;

static volatile unsigned* gpio = (void*)GPIO_BASE;
static volatile unsigned* clk = (void*)CLOCK_BASE;
static volatile unsigned* pwm = (void*)PWM_BASE;

/* Decomment this in order to get sound */
char* audio_data = &_binary_sounds_tune_wav_start;
char* audio_data1 = &_binary_sounds_B4_wav_start;
char* audio_data2 = &_binary_sounds_C1_1_wav_start;
char* audio_data3 = &_binary_sounds_D_4_wav_start;
char* audio_data4 = &_binary_sounds_E1_wav_start;
char* audio_data5 = &_binary_sounds_F3_wav_start;
char* audio_data6 = &_binary_sounds_G6_wav_start;

void pause(int t) {
    // Pause for about t ms
    int i;
    for (;t>0;t--) {
		for (i=5000;i>0;i--) 
		{	
			i++; i--;
		}
    }
}

void audio_init(void)
{
    unsigned int range = 0x400;
    unsigned int idiv = 2;
    //unsigned int pwmFrequency = (19200000 / idiv) / range; 

    SET_GPIO_ALT(40, 0);
    SET_GPIO_ALT(45, 0);
    pause(2);

    *(clk + BCM2835_PWMCLK_CNTL) = PM_PASSWORD | (1 << 5);    // stop clock
    *(clk + BCM2835_PWMCLK_DIV)  = PM_PASSWORD | (idiv<<12);  // set divisor
    *(clk + BCM2835_PWMCLK_CNTL) = PM_PASSWORD | 16 | 1;      // enable + oscillator (raspbian has this as plla)

    pause(2); 

    // disable PWM
    *(pwm + BCM2835_PWM_CONTROL) = 0;
       
    pause(2);

    *(pwm+BCM2835_PWM0_RANGE) = range;
    *(pwm+BCM2835_PWM1_RANGE) = range;

    *(pwm+BCM2835_PWM_CONTROL) =
	BCM2835_PWM1_USEFIFO | // Use FIFO and not PWM mode
	//          BCM2835_PWM1_REPEATFF |
	BCM2835_PWM1_ENABLE  | // enable channel 1
	BCM2835_PWM0_USEFIFO | // use FIFO and not PWM mode
	//          BCM2835_PWM0_REPEATFF |  */
	1<<6                 | // clear FIFO
	BCM2835_PWM0_ENABLE;   // enable channel 0

    pause(2);
}


void playSound(int soundNumber)
{
	int i=0;
    long status;
    char* soundToPlay;
    
    switch(soundNumber){
		case 0:
			soundToPlay	= audio_data;
			break;
		case 1:
			soundToPlay	= audio_data1;
			break;
		case 2:
			soundToPlay	= audio_data2;
			break;
		case 3:
			soundToPlay	= audio_data3;
			break;
		case 4:
			soundToPlay	= audio_data4;
			break;
		case 5:
			soundToPlay	= audio_data5;
			break;
		case 6:
			soundToPlay	= audio_data6;
			break;
	}
    
    while (i < 100000)
    {
		status =  *(pwm + BCM2835_PWM_STATUS);
		if (!(status & BCM2835_FULL1))
		{
			/* Decomment this in order to get sound */
			*(pwm+BCM2835_PWM_FIFO) = soundToPlay[i];
			i++;
		}
		  
		if ((status & ERRORMASK)) {
			//                uart_print("error: ");
			//                hexstring(status);
			//                uart_print("\r\n");
			*(pwm+BCM2835_PWM_STATUS) = ERRORMASK;
		}
    }
	
}

void audio_test()
{
    audio_init();
    
    playSound(6);
}

void audio_tick()
{
	int i=0;
    long status;
    char* soundToPlay = audio_data6;

    while (i < 2000)
    {
		status =  *(pwm + BCM2835_PWM_STATUS);
		if (!(status & BCM2835_FULL1))
		{
			/* Decomment this in order to get sound */
			*(pwm+BCM2835_PWM_FIFO) = soundToPlay[i];
			i++;
		}
		  
		if ((status & ERRORMASK)) {
			//                uart_print("error: ");
			//                hexstring(status);
			//                uart_print("\r\n");
			*(pwm+BCM2835_PWM_STATUS) = ERRORMASK;
		}
    }
}


