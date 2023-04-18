

#include <stdint.h>
#include "system.h"
#include "sys/alt_irq.h"
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
	// Unpacked.
	uint32_t segm_unpacked[4];
	// Packed.
	unsigned segm_packed : 16;
	unsigned            : 16;
} bf_segm;

#define segm (*((volatile bf_segm*)DIGITS_BASE))

static void timer_isr(void* context) {
	static uint16_t cnt = 0;
	
	///////////////////
	// Read inputs.
	
	///////////////////
	// Calculate state.
	
	
	
	
	
	///////////////////
	// Write outputs.
	
	
	
	
	//pio.led_packed = cnt & 0x0ff0;
	/*segm.segm_unpacked[0] = cnt & 0xf;
	segm.segm_unpacked[1] = cnt>>4;
	segm.segm_unpacked[2] = cnt>>8;
	segm.segm_unpacked[3] = cnt>>12;*/
	
	segm.segm_unpacked[0] = cnt%10;
	segm.segm_unpacked[1] = cnt/10%10;
	segm.segm_unpacked[2] = cnt/100%10;
	segm.segm_unpacked[3] = cnt/1000%10;
	
	
	
	if(pio_p32[PIO_SW_PACKED] & 0b00000001){
		segm.segm_unpacked[0] = 0;
		segm.segm_unpacked[1] = 0;
		segm.segm_unpacked[2] = 0;
		segm.segm_unpacked[3] = 0;
		
		cnt = 0;
		
		
		for(int i = 0; i < 8; i++){
			pio_p32[PIO_LED0+i] = 0;
		}
	}
	
	if(!(pio_p32[PIO_SW_PACKED] & 0b00000010) == 1){
		cnt++;
			
		for(int i=0; i < 8; i++)
		{
			pio_p32[PIO_LED0 + i] = ((cnt/10%10) >> i) & 1;
		}
		for(int i=4; i < 8; i++)
		{
			pio_p32[PIO_LED0 + i] = ((cnt/100%10) >> (i-4)) & 1;
		}
	}
	
}

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

	for(int i = 0; i < 8; i++){
		pio_p32[PIO_LED0+i] = 0;
	}
	
	if(timer_p32[TIMER_MAGIC] != 0xbabadeda){
		for(int i = 0; i < 8; i++){
			pio_p32[PIO_LED0+i] = 1;
		}
	}

	// Init IRQ.
	alt_ic_isr_register(
		TIMER_IRQ_INTERRUPT_CONTROLLER_ID, //alt_u32 ic_id
		TIMER_IRQ, //alt_u32 irq
		timer_isr, //alt_isr_func isr
		NULL, //void *isr_context
		NULL //void *flags
	);


	timer_p32[TIMER_MODULO] = 120000; // modulo.
	timer_p32[TIMER_CTRL_STATUS] = 0; // Start it.

	// Doing something in background.
#if 1
	printf("timer_p32 cnt reg:\n");
	for(int j = 0; j < 300; j++){
		printf("%9d\n", (int)timer_p32[TIMER_CNT]);
		
		// Busy wait.
		for(int i = 0; i < 10*1000*1000; i++){}
	}
	
	
	
#endif
	// Not to exit.
	
	while(1){}

	return 0;
}
