

#include <stdint.h>
#include "system.h"
#include <stdio.h>

#define WAIT_UNITL_FALSE(x) while((x)){}
#define WAIT_UNITL_TRUE(x) while(!(x)){}

#define pio_p32 ((volatile uint32_t*)PIO_BASE)
#define digits_p32 ((volatile uint32_t*)(DIGITS_BASE+0x40))
#define timer_p32 ((volatile uint32_t*)(TIMER_BASE+0x40))

#define PIO_LED0	    0
#define PIO_SW0	    8
#define PIO_LED_PACKED	    16
#define PIO_SW_PACKED	    17


#define SEGM_0             0
#define SEGM_1             3
#define SEGM_2             2
#define SEGM_3             1
#define SEGM_PACK          4
#define TIMER_CNT          0
#define TIMER_MODULO       3
#define TIMER_CTRL_STATUS  1
#define TIMER_MAGIC        2
#define TIMER_RESET_FLAG   2
#define TIMER_PAUSE_FLAG   3
#define TIMER_WRAP_FLAG    0
#define TIMER_WRAPPED_FLAG 1
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

typedef struct {

	uint32_t cnt;
	uint32_t modulo;
	uint32_t ctrl_status;
	uint32_t magic;
	uint32_t reset;
	uint32_t pause;
	uint32_t wrap;
	uint32_t wrapped;

} bf_timer;
#define timer (*((volatile bf_timer*)TIMER_BASE))


int main() {
	printf("TIMER_CNT         = 0x%08x\n", timer_p32[TIMER_CNT]);
	printf("TIMER_MODULO      = 0x%08x\n", timer_p32[TIMER_MODULO]);
	printf("TIMER_CTRL_STATUS = 0x%08x\n", timer_p32[TIMER_CTRL_STATUS]);
	printf("TIMER_MAGIC       = 0x%08x\n", timer_p32[TIMER_MAGIC]);
	printf("TIMER_RESET       = 0x%08x\n", timer_p32[TIMER_RESET]);
	printf("TIMER_PAUSE       = 0x%08x\n", timer_p32[TIMER_PAUSE]);
	printf("TIMER_WRAP        = 0x%08x\n", timer_p32[TIMER_WRAP]);
	printf("TIMER_WRAPPED     = 0x%08x\n", timer_p32[TIMER_WRAPPED]);
	
	//pio.led_packed = 0x81; // For debugging purposes.

	uint8_t cnt = 0;
	pio.led_packed = cnt;

	timer.modulo = 12000000; // modulo.
	timer.ctrl_status = 0; // Start it.

	if(timer.magic != 0xbabadeda){
		pio.led_packed = 0b11111111;
	}

	uint16_t beli = 0;
	uint16_t crni = 0;
	

	while(1){
		///////////////////
		// Read inputs.

#if 1
		// Over unpacked.
		// Poll wrapped flag.
		WAIT_UNITL_TRUE(timer.wrapped);
		// Clear wrapped flag.
		timer.wrapped = 0;
#else
		// Over packed.
		// Poll wrapped flag.
	//	WAIT_UNITL_TRUE(timer_p32[TIMER_CTRL_STATUS] & 1<<TIMER_WRAPPED_FLAG);
		// Clear wrapped flag.
	//	timer_p32[TIMER_CTRL_STATUS] &= ~(1<<TIMER_WRAPPED_FLAG);
#endif

		///////////////////
		// Calculate state.

		//cnt++;
		
		//printf("%d\n", cnt);

		///////////////////
		// Write outputs.
		
		//pio.led_packed = cnt;
		
		
		if(pio_p32[PIO_SW0] == 1){
			
			beli = 0;
			crni = 0;
			
			digits_p32[SEGM_PACK] = 0;
			
			pio.led_packed = 0;
		
		} else {
		
			if(pio_p32[PIO_SW0+1] == 1){

				digits_p32[SEGM_PACK] = (beli/10%10<<12) | (crni%10<<8) | (crni/10%10<<4) | (beli%10<<0);
				
			} else {
			
				if(pio_p32[PIO_SW0+7] == 0){
					beli++;
					
					pio.led_packed = 0b00000001;
					
				} else {
					crni++;
					
					pio.led_packed = 0b10000000;
				}
			}
			
		}
		
		
		
		
#if 1
		//digits_p32[SEGM_0] = cnt & 0xf;
		//digits_p32[SEGM_1] = cnt>>4;
		
		digits_p32[SEGM_PACK] = (beli/10%10<<12) | (crni%10<<8) | (crni/10%10<<4) | (beli%10<<0);
#else
		//digits_p32[SEGM_PACK] = cnt;
#endif
		
		///////////////////
		// Other things to do.
		
	}

	return 0;
}
