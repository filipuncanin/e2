

#include <stdint.h>
#include "system.h"
#include <stdio.h>

#define WAIT_UNITL_FALSE(x) while((x)){}
#define WAIT_UNITL_TRUE(x) while(!(x)){}

#define pio_p32 ((volatile uint32_t*)PIO_BASE)
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

typedef struct {
	// Unpacked.
	// reg 0-7
	uint32_t led_unpacked[8];
	// reg 8-15
	uint32_t sw_unpacked[8];
	// Packed.
	// reg 16
	unsigned led_packed : 8;
	unsigned sw_packed  : 8;
	unsigned            : 16;
	uint32_t babadeda[15];
} bf_pio;
#define pio (*((volatile bf_pio*)PIO_BASE))

int main() {
	printf("TIMER_CNT         = 0x%08x\n", timer_p32[TIMER_CNT]);
	printf("TIMER_MODULO      = 0x%08x\n", timer_p32[TIMER_MODULO]);
	printf("TIMER_CTRL_STATUS = 0x%08x\n", timer_p32[TIMER_CTRL_STATUS]);
	printf("TIMER_MAGIC       = 0x%08x\n", timer_p32[TIMER_MAGIC]);
	printf("TIMER_RESET       = 0x%08x\n", timer_p32[TIMER_RESET]);
	printf("TIMER_PAUSE       = 0x%08x\n", timer_p32[TIMER_PAUSE]);
	printf("TIMER_WRAP        = 0x%08x\n", timer_p32[TIMER_WRAP]);
	printf("TIMER_WRAPPED     = 0x%08x\n", timer_p32[TIMER_WRAPPED]);
	
	pio.led_packed = 0x81; // For debugging purposes.

	uint8_t cnt = 0;
	pio.led_packed = cnt;

	timer_p32[TIMER_MODULO] = 12000000; // modulo.
	timer_p32[TIMER_CTRL_STATUS] = 0; // Start it.


	while(1){
		///////////////////
		// Read inputs.

#if 1
		// Over unpacked.
		// Poll wrapped flag.
		WAIT_UNITL_TRUE(timer_p32[TIMER_WRAPPED]);
		// Clear wrapped flag.
		timer_p32[TIMER_WRAPPED] = 0;
#else
		// Over packed.
		// Poll wrapped flag.
		WAIT_UNITL_TRUE(timer_p32[TIMER_CTRL_STATUS] & 1<<TIMER_WRAPPED_FLAG);
		// Clear wrapped flag.
		timer_p32[TIMER_CTRL_STATUS] &= ~(1<<TIMER_WRAPPED_FLAG);
#endif

		///////////////////
		// Calculate state.

		cnt++;
		//printf("%d\n", cnt);

		///////////////////
		// Write outputs.
		
		pio.led_packed = cnt;
		
#if 1
		digits_p32[SEGM_0] = cnt & 0xf;
		digits_p32[SEGM_1] = cnt>>4;
#else
		digits_p32[SEGM_PACK] = cnt;
#endif
		
		///////////////////
		// Other things to do.
		
	}

	return 0;
}
