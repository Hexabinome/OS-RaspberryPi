#include "pwm.h"
#include "hw.h"

extern char _binary_sounds_tune_wav_start;
extern char _binary_sounds_B4_wav_start;
extern char _binary_sounds_C1_1_wav_start;
extern char _binary_sounds_D_4_wav_start;
extern char _binary_sounds_E1_wav_start;
extern char _binary_sounds_F3_wav_start;
extern char _binary_sounds_G6_wav_start;
extern char _binary_sounds_fbe1_wav_start;
extern char _binary_sounds_108bitmono_wav_start;
extern char _binary_sounds_118bitmono_wav_start;
extern char _binary_sounds_128bitmono_wav_start;
extern char _binary_sounds_138bitmono_wav_start;
extern char _binary_sounds_148bitmono_wav_start;
extern char _binary_sounds_158bitmono_wav_start;
extern char _binary_sounds_168bitmono_wav_start;
extern char _binary_sounds_178bitmono_wav_start;
extern char _binary_sounds_188bitmono_wav_start;
extern char _binary_sounds_18bitmono_wav_start;
extern char _binary_sounds_198bitmono_wav_start;
extern char _binary_sounds_208bitmono_wav_start;
extern char _binary_sounds_218bitmono_wav_start;
extern char _binary_sounds_228bitmono_wav_start;
extern char _binary_sounds_238bitmono_wav_start;
extern char _binary_sounds_248bitmono_wav_start;
extern char _binary_sounds_28bitmono_wav_start;
extern char _binary_sounds_38bitmono_wav_start;
extern char _binary_sounds_48bitmono_wav_start;
extern char _binary_sounds_58bitmono_wav_start;
extern char _binary_sounds_68bitmono_wav_start;
extern char _binary_sounds_78bitmono_wav_start;
extern char _binary_sounds_88bitmono_wav_start;
extern char _binary_sounds_98bitmono_wav_start;


static volatile unsigned* gpio = (void*)GPIO_BASE;
static volatile unsigned* clk = (void*)CLOCK_BASE;
static volatile unsigned* pwm = (void*)PWM_BASE;

/* Decomment this in order to get sound */
//for i in *mono*; do echo -n "char* audio_data"; echo -n $(basename -s .wav $i);echo -n "= &_binary_sounds_"; echo -n $(basename -s .wav $i) ; echo "_wav_start;"; done
char* audio_data = &_binary_sounds_tune_wav_start;
char* audio_data1 = &_binary_sounds_B4_wav_start;
char* audio_data2 = &_binary_sounds_C1_1_wav_start;
char* audio_data3 = &_binary_sounds_D_4_wav_start;
char* audio_data4 = &_binary_sounds_E1_wav_start;
char* audio_data5 = &_binary_sounds_F3_wav_start;
char* audio_data6 = &_binary_sounds_G6_wav_start;
char* audio_data7 = &_binary_sounds_fbe1_wav_start;
char* audio_data108bitmono= &_binary_sounds_108bitmono_wav_start;
char* audio_data118bitmono= &_binary_sounds_118bitmono_wav_start;
char* audio_data128bitmono= &_binary_sounds_128bitmono_wav_start;
char* audio_data138bitmono= &_binary_sounds_138bitmono_wav_start;
char* audio_data148bitmono= &_binary_sounds_148bitmono_wav_start;
char* audio_data158bitmono= &_binary_sounds_158bitmono_wav_start;
char* audio_data168bitmono= &_binary_sounds_168bitmono_wav_start;
char* audio_data178bitmono= &_binary_sounds_178bitmono_wav_start;
char* audio_data188bitmono= &_binary_sounds_188bitmono_wav_start;
char* audio_data18bitmono= &_binary_sounds_18bitmono_wav_start;
char* audio_data198bitmono= &_binary_sounds_198bitmono_wav_start;
char* audio_data208bitmono= &_binary_sounds_208bitmono_wav_start;
char* audio_data218bitmono= &_binary_sounds_218bitmono_wav_start;
char* audio_data228bitmono= &_binary_sounds_228bitmono_wav_start;
char* audio_data238bitmono= &_binary_sounds_238bitmono_wav_start;
char* audio_data248bitmono= &_binary_sounds_248bitmono_wav_start;
char* audio_data28bitmono= &_binary_sounds_28bitmono_wav_start;
char* audio_data38bitmono= &_binary_sounds_38bitmono_wav_start;
char* audio_data48bitmono= &_binary_sounds_48bitmono_wav_start;
char* audio_data58bitmono= &_binary_sounds_58bitmono_wav_start;
char* audio_data68bitmono= &_binary_sounds_68bitmono_wav_start;
char* audio_data78bitmono= &_binary_sounds_78bitmono_wav_start;
char* audio_data88bitmono= &_binary_sounds_88bitmono_wav_start;
char* audio_data98bitmono= &_binary_sounds_98bitmono_wav_start;


// Skipper is the amount of samples to be skipped after every played sample.
// Example 0: play all samples, 3: play one out of 4 samples.
int skipper = 2;
unsigned int idiv = 2;

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
    
    // The playback pitch correction can be adjusted using the ratio of idiv to (skipper+1)
    // The optimal ratio is close to a pitch correction of = 1.14
    // Unfortunately the played back audio quality suffers under-sampling as skipper increases, 
    // So we have to find a trade of between reaching the propper frequency and preserving audio quality.
    // This is doable by picking skipper = 1, idiv = 2
    // Note: this cannot be acomplished without skipper ans idiv=1, since idiv does not work if <2

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

void updateSkipper(int newSkipper)
{
	skipper=newSkipper;
}

void updateIdiv(int newIdiv)
{
	idiv=newIdiv;
}

void playPitchedSound(int soundNumber, int nextSkipper, int nextIdiv)
{
	skipper = nextSkipper;
	idiv = nextIdiv;
	audio_init();
	playSound(soundNumber);
}

void playSound(int soundNumber)
{   
//    audio_init();

    int i=0;
    long status;
    char* soundToPlay;
    
    switch(soundNumber)
    {
		case 0:
			soundToPlay	= audio_data;
			break;
		case 1:	
			soundToPlay	= audio_data7;
			break;
		case 2:	
			soundToPlay	= audio_data18bitmono;
			break;
		case 3:	
			soundToPlay	= audio_data28bitmono;
			break;
		case 4:	
			soundToPlay	= audio_data38bitmono;
			break;
		case 5:	
			soundToPlay	= audio_data48bitmono;
			break;
		case 6:	
			soundToPlay	= audio_data58bitmono;
			break;
		case 7:	
			soundToPlay	= audio_data68bitmono;
			break;
		case 8:	
			soundToPlay	= audio_data78bitmono;
			break;
		case 9:	
			soundToPlay	= audio_data88bitmono;
			break;
		case 10:	
			soundToPlay	= audio_data98bitmono;
			break;
		case 11:	
			soundToPlay	= audio_data108bitmono;
			break;
		case 12:	
			soundToPlay	= audio_data118bitmono;
			break;
		case 13:	
			soundToPlay	= audio_data128bitmono;
			break;
		case 14:	
			soundToPlay	= audio_data138bitmono;
			break;
		case 15:	
			soundToPlay	= audio_data148bitmono;
			break;
		case 16:	
			soundToPlay	= audio_data158bitmono;
			break;
		case 17:	
			soundToPlay	= audio_data168bitmono;
			break;
		case 18:	
			soundToPlay	= audio_data178bitmono;
			break;
		case 19:	
			soundToPlay	= audio_data188bitmono;
			break;
		case 20:	
			soundToPlay	= audio_data198bitmono;
			break;
		case 21:	
			soundToPlay	= audio_data208bitmono;
			break;
		case 22:	
			soundToPlay	= audio_data218bitmono;
			break;
		case 23:	
			soundToPlay	= audio_data228bitmono;
			break;
		case 24:	
			soundToPlay	= audio_data238bitmono;
			break;
		case 25:	
			soundToPlay	= audio_data248bitmono;
			break;
    }
    
	while (i < 125000)
	{
		status =  *(pwm + BCM2835_PWM_STATUS);
		if (!(status & BCM2835_FULL1))
		{
			/* Decomment this in order to get sound */
			*(pwm+BCM2835_PWM_FIFO) = soundToPlay[i*(skipper+1)];
			i++;
		}
		  
		if ((status & ERRORMASK))
		{
			//                uart_print("error: ");
			//                hexstring(status);
			//                uart_print("\r\n");
			*(pwm+BCM2835_PWM_STATUS) = ERRORMASK;
		}
	}
	i=0;
}

void audio_test()
{
    audio_init();
    
    playSound(8);
}
