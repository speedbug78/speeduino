//#if defined(MCU_STM32F103C8) || defined(MCU_STM32F103CB)
//#if defined(PILL_F103XX)

#include "board_stm32_blue_pill.h"
#include "globals.h"
#include "auxiliaries.h"
#include "idle.h"
#include "scheduler.h"
#include "HardwareTimer.h"


#if defined(ARDUINO_ARCH_STM32) && defined(STM32_CORE_VERSION)
    //These should really be in the stm32 libmaple libs, but for somereason they only have timers 1-4
    //#include <stm32_TIM_variant_11.h>
    //HardwareTimer Timer5(TIM5, chip_tim5, sizeof(chip_tim5) / sizeof(chip_tim5[0]));
    //HardwareTimer Timer8(TIM8, chip_tim8, sizeof(chip_tim8) / sizeof(chip_tim8[0]));
#endif

void initBoard()
{
    /*
    ***********************************************************************************************************
    * General
    */
    #ifndef FLASH_LENGTH
      #define FLASH_LENGTH 8192
    #endif
    delay(10);
    /*
    ***********************************************************************************************************
    * Idle
    */
    if( (configPage6.iacAlgorithm == IAC_ALGORITHM_PWM_OL) || (configPage6.iacAlgorithm == IAC_ALGORITHM_PWM_CL) )
    {
        idle_pwm_max_count = 1000000L / (configPage6.idleFreq * 2); //Converts the frequency in Hz to the number of ticks (at 2uS) it takes to complete 1 cycle. Note that the frequency is divided by 2 coming from TS to allow for up to 5KHz
    } 

    //This must happen at the end of the idle init
    Timer1.setMode(4, TIMER_OUTPUT_COMPARE);
    //timer_set_mode(TIMER1, 4, TIMER_OUTPUT_COMPARE;
    if(idle_pwm_max_count > 0) { Timer1.attachInterrupt(4, idleInterrupt); } //on first flash the configPage4.iacAlgorithm is invalid


    /*
    ***********************************************************************************************************
    * Timers
    */
    Timer4.setOverflow(1000, MICROSEC_FORMAT);  // Set up period
    Timer4.setMode(1, TIMER_OUTPUT_COMPARE);
    Timer4.attachInterrupt(1, oneMSInterval);
    Timer4.resume(); //Start Timer

    pinMode(LED_BUILTIN, OUTPUT); //Visual WDT

    /*
    ***********************************************************************************************************
    * Auxilliaries
    */
    //2uS resolution Min 8Hz, Max 5KHz
    boost_pwm_max_count = 1000000L / (2 * configPage6.boostFreq * 2); //Converts the frequency in Hz to the number of ticks (at 2uS) it takes to complete 1 cycle. The x2 is there because the frequency is stored at half value (in a byte) to allow freqneucies up to 511Hz
    vvt_pwm_max_count = 1000000L / (2 * configPage6.vvtFreq * 2); //Converts the frequency in Hz to the number of ticks (at 2uS) it takes to complete 1 cycle

    //Need to be initialised last due to instant interrupt
    Timer1.setMode(2, TIMER_OUTPUT_COMPARE);
    Timer1.setMode(3, TIMER_OUTPUT_COMPARE);
    if(boost_pwm_max_count > 0) { Timer1.attachInterrupt(2, boostInterrupt);}
    if(vvt_pwm_max_count > 0) { Timer1.attachInterrupt(3, vvtInterrupt);}

    /*
    ***********************************************************************************************************
    * Schedules
    */
    //#if defined (STM32F1) || defined(__STM32F1__)
	//(CYCLES_PER_MICROSECOND == 72, APB2 at 72MHz, APB1 at 36MHz).
	//Timer2 to 4 is on APB1, Timer1 on APB2.   www.st.com/resource/en/datasheet/stm32f103cb.pdf sheet 12
	Timer1.setPrescaleFactor((72 * 2)-1); //2us resolution
	Timer2.setPrescaleFactor((36 * 2)-1); //2us resolution
	Timer3.setPrescaleFactor((36 * 2)-1); //2us resolution

    Timer2.setMode(1, TIMER_OUTPUT_COMPARE);
    Timer2.setMode(2, TIMER_OUTPUT_COMPARE);
    Timer2.setMode(3, TIMER_OUTPUT_COMPARE);
    Timer2.setMode(4, TIMER_OUTPUT_COMPARE);

    Timer3.setMode(1, TIMER_OUTPUT_COMPARE);
    Timer3.setMode(2, TIMER_OUTPUT_COMPARE);
    Timer3.setMode(3, TIMER_OUTPUT_COMPARE);
    Timer3.setMode(4, TIMER_OUTPUT_COMPARE);
    Timer1.setMode(1, TIMER_OUTPUT_COMPARE);

    //Attach interupt functions
    //Injection
    Timer2.attachInterrupt(1, fuelSchedule1Interrupt);
    Timer2.attachInterrupt(2, fuelSchedule2Interrupt);
    Timer2.attachInterrupt(3, fuelSchedule3Interrupt);
    Timer2.attachInterrupt(4, fuelSchedule4Interrupt);

    //Ignition
    Timer3.attachInterrupt(1, ignitionSchedule1Interrupt); 
    Timer3.attachInterrupt(2, ignitionSchedule2Interrupt);
    Timer3.attachInterrupt(3, ignitionSchedule3Interrupt);
    Timer3.attachInterrupt(4, ignitionSchedule4Interrupt);

    Timer1.setOverflow(0xFFFF);
    Timer1.resume();
    Timer2.setOverflow(0xFFFF);
    Timer2.resume();
    Timer3.setOverflow(0xFFFF);
    Timer3.resume();
}

uint16_t freeRam()
{
    char top = 't';
    return &top - reinterpret_cast<char*>(sbrk(0));
}

  /*
  ***********************************************************************************************************
  * Interrupt callback functions
  */
  void oneMSInterval(HardwareTimer*){oneMSInterval();}
  void boostInterrupt(HardwareTimer*){boostInterrupt();}
  void fuelSchedule1Interrupt(HardwareTimer*){fuelSchedule1Interrupt();}
  void fuelSchedule2Interrupt(HardwareTimer*){fuelSchedule2Interrupt();}
  void fuelSchedule3Interrupt(HardwareTimer*){fuelSchedule3Interrupt();}
  void fuelSchedule4Interrupt(HardwareTimer*){fuelSchedule4Interrupt();}
  void idleInterrupt(HardwareTimer*){idleInterrupt();}
  void vvtInterrupt(HardwareTimer*){vvtInterrupt();}
  void ignitionSchedule1Interrupt(HardwareTimer*){ignitionSchedule1Interrupt();}
  void ignitionSchedule2Interrupt(HardwareTimer*){ignitionSchedule2Interrupt();}
  void ignitionSchedule3Interrupt(HardwareTimer*){ignitionSchedule3Interrupt();}
  void ignitionSchedule4Interrupt(HardwareTimer*){ignitionSchedule4Interrupt();}

//#endif
