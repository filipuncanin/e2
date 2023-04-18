

#include <stdint.h>
#include "system.h"
#include <stdio.h>
#include <stdbool.h>
#include "sys/alt_irq.h"

#define pio_p32 ((volatile uint32_t*)PIO_BASE)
#define digits_p32 ((volatile uint32_t*)DIGITS_BASE)
#define timer_p32 ((volatile uint32_t *)TIMER_BASE)

#define WAIT_UNITL_FALSE(x) while((x)){}
#define WAIT_UNITL_TRUE(x) while(!(x)){}

#define SEGM_0             0
#define SEGM_1             1
#define SEGM_2             2
#define SEGM_3             3
#define SEGM_PACK          4

#define TIMER_CNT          0
#define TIMER_MODULO       1
#define TIMER_CTRL_STATUS  2
#define TIMER_MAGIC        3
#define TIMER_RESET_FLAG   0
#define TIMER_PAUSE_FLAG   1
#define TIMER_WRAP_FLAG    2
#define TIMER_WRAPPED_FLAG 3
#define TIMER_RESET        (TIMER_RESET_FLAG+4)
#define TIMER_PAUSE        (TIMER_PAUSE_FLAG+4)
#define TIMER_WRAP         (TIMER_WRAP_FLAG+4)
#define TIMER_WRAPPED      (TIMER_WRAPPED_FLAG+4)

static void timer_isr(void* context) {
	digits_p32[SEGM_PACK]++;
}

int main() {

	digits_p32[SEGM_PACK] = 0;
	//pio_p32[16] = 0;	// sve led diode na 0;
	//pio_p32[16] = 0xca;	// diode na 11001010

	// Init IRQ.
	alt_ic_isr_register(
		TIMER_IRQ_INTERRUPT_CONTROLLER_ID, //alt_u32 ic_id
		TIMER_IRQ, //alt_u32 irq
		timer_isr, //alt_isr_func isr
		NULL, //void *isr_context
		NULL //void *flags
	);

	
	timer_p32 [TIMER_MODULO] = 12000000;
	timer_p32 [TIMER_CTRL_STATUS] = 0;

	while(1){
		
		//WAIT_UNITL_TRUE ( timer_p32[TIMER_WRAPPED]);
		//timer_p32[TIMER_WRAPPED] = 0;	
		
		pio_p32[16] = pio_p32[17];	// leds show switches	
		
	}

	return 0;
}







