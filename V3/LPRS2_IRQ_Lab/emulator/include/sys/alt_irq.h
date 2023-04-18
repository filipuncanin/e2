
#ifndef ALT_IRQ_H
#define ALT_IRQ_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>


typedef uint32_t alt_u32;
typedef void (*alt_isr_func)(void* context);


void alt_ic_isr_register(
	alt_u32 ic_id,
	alt_u32 irq,
	alt_isr_func isr,
	void *isr_context,
	void *flags
);


#ifdef __cplusplus
}
#endif

#endif // ALT_IRQ_H
