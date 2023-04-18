

#include <stdint.h>
#include "system.h"
#include <stdio.h>
#include <stdbool.h>
#include "sys/alt_irq.h"

#define pio_p32 ((volatile uint32_t*)PIO_BASE)

#define POOL_0_IRQ_1 1;


#define WAIT_UNITL_FALSE(x) while((x)){}
#define WAIT_UNITL_TRUE(x) while(!(x)){}

#define digits_p32 ((volatile uint32_t*)DIGITS_BASE)
#define timer_p32 ((volatile uint32_t*)TIMER_BASE)

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

// kopirano iz nedelja3 / app_01_poling
typedef struct {
	// Unpacked.
	// reg 0-7
	uint32_t led_unpacked[8];
	// reg 8-15
	uint32_t sw_unpacked[8];
	// Packed.
	// reg 16
	unsigned led_packed : 8;
	unsigned 	    : 24;
	// reg 17
	unsigned sw_packed  : 8;
	unsigned            : 24;
	uint32_t babadeda[14];
} bf_pio;
#define pio (*((volatile bf_pio*)PIO_BASE))

#if POLL_0_IRQ_1
static volatile bool passed_5_sec = false;
static void timer_isr(void* context) {
	passed_5_sec = true;
}
#endif

int main() {

	digits_p32[SEGM_PACK] = 0;
	
#if POLL_0_IRQ_1	
	// Init IRQ.
	alt_ic_isr_register(
		TIMER_IRQ_INTERRUPT_CONTROLLER_ID, //alt_u32 ic_id
		TIMER_IRQ, //alt_u32 irq
		timer_isr, //alt_isr_func isr
		NULL, //void *isr_context
		NULL //void *flags
	);
#endif

	timer_p32[TIMER_MODULO] = 12000000*3; // modulo. 5 SEKUNDI	
	timer_p32[TIMER_CTRL_STATUS] = 0; // Start it.

	while(1){
		///////////////////
		// Read inputs.

#if !POLL_0_IRQ_1
		bool passed_5_sec = false;
		
		//WAIT_UNITL_TRUE(timer_p32[TIMER_WRAPPED]);
		//timer_p32[TIMER_WRAPPED] = 0;
		
		if(timer_p32[TIMER_WRAPPED]){
			timer_p32[TIMER_WRAPPED] = 0;
			passed_5_sec = true;
		}
#endif
		
		//uint8_t x = pio_p32[17];
		uint8_t x = pio.sw_packed & 0x0f; // SW[3:0]

		///////////////////
		// Calculate state.
		
		uint8_t b0 = x>>0 & 1;
		uint8_t b1 = x>>1 & 1;
		uint8_t b2 = x>>2 & 1;
		uint8_t b3 = x>>3 & 1;
		//uint8_t parity = (b0 + b1 + b2 + b3 + 1) & 1; 
		uint8_t parity = b0 ^ b1 ^ b2 ^ b3;

		///////////////////
		// Write outputs.
		
		if(pio.sw_unpacked[7]){	// reset na sw 7
			pio.led_packed = 0;
			digits_p32[SEGM_PACK] = 0;
			pio.sw_unpacked[7] = 0;
		} else {
			pio.led_packed = parity<<4 | x;
			if(passed_5_sec) {
				if(parity){
					digits_p32[SEGM_2]++;
				} else {
					digits_p32[SEGM_0]++;
				}
				passed_5_sec = false;
			}
		}
		
		///////////////////
		// Other things to do.
		
	}

	return 0;
}
