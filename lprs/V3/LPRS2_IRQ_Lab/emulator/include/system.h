
#ifndef SYSTEM_H
#define SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif


extern volatile void* __pio_base;
#define PIO_BASE __pio_base

extern volatile void* __digits_base;
#define DIGITS_BASE __digits_base

extern volatile void* __timer_base;
#define TIMER_BASE __timer_base

#define TIMER_IRQ_INTERRUPT_CONTROLLER_ID 0
#define TIMER_IRQ 0



#ifdef __cplusplus
}
#endif

#endif // SYSTEM_H
