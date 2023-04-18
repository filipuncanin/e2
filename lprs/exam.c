

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

uint32_t reg = 0x80;
uint32_t smer = 0;
uint32_t cnt_l = 0;
uint32_t cnt_r = 0;

static void timer_isr(void* context) {
	if(smer == 0) {
		reg = reg >> 1;
		if(reg == 1) {
			smer = 1;
			cnt_r++;
		}
	} else {
		reg = reg << 1;
		if(reg == 0x80) {
			smer = 0;
			cnt_l++;	
		}
	}
	
	pio_p32[16] = reg;
	digits_p32[SEGM_0] = cnt_r;
	digits_p32[SEGM_2] = cnt_l;
}

int main() {

	digits_p32[SEGM_PACK] = 0;

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
		if(pio_p32[8] == 0 && pio_p32[9] == 0){
			timer_p32 [TIMER_MODULO] = 12000000*2;
			timer_p32 [TIMER_CTRL_STATUS] = 0;
		} else if(pio_p32[8] == 1 && pio_p32[9] == 0){
			timer_p32 [TIMER_MODULO] = 12000000;
			timer_p32 [TIMER_CTRL_STATUS] = 0;
		} else if(pio_p32[8] == 0 && pio_p32[9] == 1){
			timer_p32 [TIMER_MODULO] = 12000000/2;
			timer_p32 [TIMER_CTRL_STATUS] = 0;
		} else if(pio_p32[8] == 1 && pio_p32[9] == 1){
			timer_p32 [TIMER_MODULO] = 12000000/4;
			timer_p32 [TIMER_CTRL_STATUS] = 0;
		}	
		
		if(pio_p32[15] == 1) {
			reg = 0x80;
			digits_p32[SEGM_PACK] = 0;
		}
	}

	return 0;
}







