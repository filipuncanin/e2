
///////////////////////////////////////////////////////////////////////////////

#include "system.h"
#include "sys/alt_irq.h"
#include "config.h"

#include <SFML/Graphics.hpp>

#include <thread>
#include <mutex>

#include <stdexcept>
#include <sstream>
#include <cstring>
#include <cmath>

#include <cstddef>

#include <iostream>
#include <cstdio>

using namespace std;
using namespace sf;

///////////////////////////////////////////////////////////////////////////////
// Helpers.

#define DEBUG(var) do{cout << #var << " = " << var << endl; }while(false)


///////////////////////////////////////////////////////////////////////////////
// Memory mappings.

volatile void* __pio_base;
volatile void* __digits_base;
volatile void* __timer_base;


struct lprs2_simple_pio_mem_map {
	struct {
		unsigned led0      :  1;
		unsigned _res_led0 : 31;
		unsigned led1      :  1;
		unsigned _res_led1 : 31;
		unsigned led2      :  1;
		unsigned _res_led2 : 31;
		unsigned led3      :  1;
		unsigned _res_led3 : 31;
		unsigned led4      :  1;
		unsigned _res_led4 : 31;
		unsigned led5      :  1;
		unsigned _res_led5 : 31;
		unsigned led6      :  1;
		unsigned _res_led6 : 31;
		unsigned led7      :  1;
		unsigned _res_led7 : 31;
		unsigned sw0       :  1;
		unsigned _res_sw0  : 31;
		unsigned sw1       :  1;
		unsigned _res_sw1  : 31;
		unsigned sw2       :  1;
		unsigned _res_sw2  : 31;
		unsigned sw3       :  1;
		unsigned _res_sw3  : 31;
		unsigned sw4       :  1;
		unsigned _res_sw4  : 31;
		unsigned sw5       :  1;
		unsigned _res_sw5  : 31;
		unsigned sw6       :  1;
		unsigned _res_sw6  : 31;
		unsigned sw7       :  1;
		unsigned _res_sw7  : 31;
	} u;
	struct{
		unsigned led0     :  1;
		unsigned led1     :  1;
		unsigned led2     :  1;
		unsigned led3     :  1;
		unsigned led4     :  1;
		unsigned led5     :  1;
		unsigned led6     :  1;
		unsigned led7     :  1;
		unsigned _res_led : 24;
		unsigned sw0      :  1;
		unsigned sw1      :  1;
		unsigned sw2      :  1;
		unsigned sw3      :  1;
		unsigned sw4      :  1;
		unsigned sw5      :  1;
		unsigned sw6      :  1;
		unsigned sw7      :  1;
		unsigned _res_sw  : 24;
	} p;
	uint32_t _res[14];
};

struct lprs2_7segm_mem_map {
	struct {
		struct {
			unsigned segm0 :  4;
			unsigned       : 28;
			unsigned segm1 :  4;
			unsigned       : 28;
			unsigned segm2 :  4;
			unsigned       : 28;
			unsigned segm3 :  4;
			unsigned       : 28;
		} u;
		struct {
			unsigned segm0 :  4;
			unsigned segm1 :  4;
			unsigned segm2 :  4;
			unsigned segm3 :  4;
			unsigned       : 16;
		} p;
		uint32_t _res[3];
	} g31;
	struct {
		struct {
			unsigned segm3 :  4;
			unsigned       : 28;
			unsigned segm0 :  4;
			unsigned       : 28;
			unsigned segm2 :  4;
			unsigned       : 28;
			unsigned segm1 :  4;
			unsigned       : 28;
		} u;
		struct {
			unsigned segm3 :  4;
			unsigned segm0 :  4;
			unsigned segm2 :  4;
			unsigned segm1 :  4;
			unsigned       : 16;
		} p;
		uint32_t _res[3];
	} g32;
	struct {
		struct {
			unsigned segm0 :  4;
			unsigned       : 28;
			unsigned segm3 :  4;
			unsigned       : 28;
			unsigned segm2 :  4;
			unsigned       : 28;
			unsigned segm1 :  4;
			unsigned       : 28;
		} u;
		struct {
			unsigned segm0 :  4;
			unsigned segm3 :  4;
			unsigned segm2 :  4;
			unsigned segm1 :  4;
			unsigned       : 16;
		} p;
		uint32_t _res[3];
	} g33;
	struct {
		struct {
			unsigned segm0 :  4;
			unsigned       : 28;
			unsigned segm3 :  4;
			unsigned       : 28;
			unsigned segm2 :  4;
			unsigned       : 28;
			unsigned segm1 :  4;
			unsigned       : 28;
		} u;
		struct {
			unsigned segm0 :  4;
			unsigned segm3 :  4;
			unsigned segm2 :  4;
			unsigned segm1 :  4;
			unsigned       : 16;
		} p;
		uint32_t _res[3];
	} g34;

};

struct lprs2_timer_mem_map {
	struct {
		uint32_t cnt;
		uint32_t modulo;
		struct {
			unsigned reset   :  1;
			unsigned pause   :  1;
			unsigned wrap    :  1;
			unsigned wrapped :  1;
			unsigned _res    : 28;
		} flags;
		uint32_t magic;
		struct { unsigned b : 1; unsigned _res : 31; } reset;
		struct { unsigned b : 1; unsigned _res : 31; } pause;
		struct { unsigned b : 1; unsigned _res : 31; } wrap;
		struct { unsigned b : 1; unsigned _res : 31; } wrapped;
	} g31;
	struct {
		uint32_t cnt;
		uint32_t magic;
		struct {
			unsigned pause   :  1;
			unsigned wrap    :  1;
			unsigned wrapped :  1;
			unsigned reset   :  1;
			unsigned _res    : 28;
		} flags;
		uint32_t modulo;
		struct { unsigned b : 1; unsigned _res : 31; } pause;
		struct { unsigned b : 1; unsigned _res : 31; } wrap;
		struct { unsigned b : 1; unsigned _res : 31; } wrapped;
		struct { unsigned b : 1; unsigned _res : 31; } reset;
	} g32;
	struct {
		uint32_t cnt;
		struct {
			unsigned wrap    :  1;
			unsigned wrapped :  1;
			unsigned reset   :  1;
			unsigned pause   :  1;
			unsigned _res    : 28;
		} flags;
		uint32_t magic;
		uint32_t modulo;
		struct { unsigned b : 1; unsigned _res : 31; } wrap;
		struct { unsigned b : 1; unsigned _res : 31; } wrapped;
		struct { unsigned b : 1; unsigned _res : 31; } reset;
		struct { unsigned b : 1; unsigned _res : 31; } pause;
	} g33;
	struct {
		uint32_t magic;
		struct {
			unsigned pause   :  1;
			unsigned wrap    :  1;
			unsigned wrapped :  1;
			unsigned reset   :  1;
			unsigned _res    : 28;
		} flags;
		uint32_t modulo;
		uint32_t cnt;
		struct { unsigned b : 1; unsigned _res : 31; } pause;
		struct { unsigned b : 1; unsigned _res : 31; } wrap;
		struct { unsigned b : 1; unsigned _res : 31; } wrapped;
		struct { unsigned b : 1; unsigned _res : 31; } reset;
	} g34;
};



template<typename T>
T unvolatile(volatile T& v){
	return *(T*)&v;
}

#define UPDATE_1( \
	mm0, \
	mem_to_update \
) \
	do{ \
		(mm0) = (mem_to_update); \
	}while(0)
	
#define UPDATE_2( \
	mm0, \
	mm1, \
	mem_to_update \
) \
	do{ \
		(mm0) = (mem_to_update); \
		(mm1) = (mem_to_update); \
	}while(0)

#define UPDATE_4( \
	mm0, \
	mm1, \
	mm2, \
	mm3, \
	mem_to_update \
) \
	do{ \
		(mm0) = (mem_to_update); \
		(mm1) = (mem_to_update); \
		(mm2) = (mem_to_update); \
		(mm3) = (mem_to_update); \
	}while(0)
	
#define UPDATE_8( \
	mm0, \
	mm1, \
	mm2, \
	mm3, \
	mm4, \
	mm5, \
	mm6, \
	mm7, \
	mem_to_update \
) \
	do{ \
		(mm0) = (mem_to_update); \
		(mm1) = (mem_to_update); \
		(mm2) = (mem_to_update); \
		(mm3) = (mem_to_update); \
		(mm4) = (mem_to_update); \
		(mm5) = (mem_to_update); \
		(mm6) = (mem_to_update); \
		(mm7) = (mem_to_update); \
	}while(0)

#define UPDATE_IF_CHANGED_1( \
	mm0_under_test, \
	mem \
) \
	do{ \
		auto x = mm0_under_test; \
		if(x != (mem)){ \
			(mem) = x; \
		} \
	}while(0)

#define UPDATE_IF_CHANGED_2( \
	mm0, \
	mm1, \
	mem \
) \
	do{ \
		{ \
			auto x = mm0; \
			if(x != (mem)){ \
				(mem) = x; \
				(mm1) = x; \
			} \
		} \
		{ \
			auto x = mm1; \
			if(x != (mem)){ \
				(mem) = x; \
				(mm0) = x; \
			} \
		} \
	}while(0)

#define UPDATE_IF_CHANGED_4( \
	mm0, \
	mm1, \
	mm2, \
	mm3, \
	mem \
) \
	do{ \
		{ \
			auto x = mm0; \
			if(x != (mem)){ \
				(mem) = x; \
				(mm1) = x; \
				(mm2) = x; \
				(mm3) = x; \
			} \
		} \
		{ \
			auto x = mm1; \
			if(x != (mem)){ \
				(mem) = x; \
				(mm0) = x; \
				(mm2) = x; \
				(mm3) = x; \
			} \
		} \
		{ \
			auto x = mm2; \
			if(x != (mem)){ \
				(mem) = x; \
				(mm0) = x; \
				(mm1) = x; \
				(mm3) = x; \
			} \
		} \
		{ \
			auto x = mm3; \
			if(x != (mem)){ \
				(mem) = x; \
				(mm0) = x; \
				(mm1) = x; \
				(mm2) = x; \
			} \
		} \
	}while(0)
	
#define UPDATE_IF_CHANGED_8( \
	mm0, \
	mm1, \
	mm2, \
	mm3, \
	mm4, \
	mm5, \
	mm6, \
	mm7, \
	mem \
) \
	do{ \
		{ \
			auto x = mm0; \
			if(x != (mem)){ \
				(mem) = x; \
				(mm1) = x; \
				(mm2) = x; \
				(mm3) = x; \
				(mm4) = x; \
				(mm5) = x; \
				(mm6) = x; \
				(mm7) = x; \
			} \
		} \
		{ \
			auto x = mm1; \
			if(x != (mem)){ \
				(mm0) = x; \
				(mem) = x; \
				(mm2) = x; \
				(mm3) = x; \
				(mm4) = x; \
				(mm5) = x; \
				(mm6) = x; \
				(mm7) = x; \
			} \
		} \
		{ \
			auto x = mm2; \
			if(x != (mem)){ \
				(mm0) = x; \
				(mm1) = x; \
				(mem) = x; \
				(mm3) = x; \
				(mm4) = x; \
				(mm5) = x; \
				(mm6) = x; \
				(mm7) = x; \
			} \
		} \
		{ \
			auto x = mm3; \
			if(x != (mem)){ \
				(mm0) = x; \
				(mm1) = x; \
				(mm2) = x; \
				(mem) = x; \
				(mm4) = x; \
				(mm5) = x; \
				(mm6) = x; \
				(mm7) = x; \
			} \
		} \
		{ \
			auto x = mm4; \
			if(x != (mem)){ \
				(mm0) = x; \
				(mm1) = x; \
				(mm2) = x; \
				(mm3) = x; \
				(mem) = x; \
				(mm5) = x; \
				(mm6) = x; \
				(mm7) = x; \
			} \
		} \
		{ \
			auto x = mm5; \
			if(x != (mem)){ \
				(mm0) = x; \
				(mm1) = x; \
				(mm2) = x; \
				(mm3) = x; \
				(mm4) = x; \
				(mem) = x; \
				(mm6) = x; \
				(mm7) = x; \
			} \
		} \
		{ \
			auto x = mm6; \
			if(x != (mem)){ \
				(mm0) = x; \
				(mm1) = x; \
				(mm2) = x; \
				(mm3) = x; \
				(mm4) = x; \
				(mm5) = x; \
				(mem) = x; \
				(mm7) = x; \
			} \
		} \
		{ \
			auto x = mm7; \
			if(x != (mem)){ \
				(mm0) = x; \
				(mm1) = x; \
				(mm2) = x; \
				(mm3) = x; \
				(mm4) = x; \
				(mm5) = x; \
				(mm6) = x; \
				(mem) = x; \
			} \
		} \
	}while(0)
	
///////////////////////////////////////////////////////////////////////////////

class LinearAtlas {
	uint32_t w;
	uint32_t h;
	Texture texture;
	Sprite sprite;
public:
	LinearAtlas(
		const string& file_name,
		uint32_t unit_width,
		uint32_t unit_height
	) {
		w = unit_width;
		h = unit_height;
		
		if(!texture.loadFromFile(file_name)){
			ostringstream oss;
			//"Cannot load texture from file"
			throw runtime_error("!");
		}
		sprite.setTexture(texture);
	}
	
	uint32_t getUnitWidth() const {
		return w;
	}
	uint32_t getUnitHeight() const {
		return h;
	}
	
	void draw(
		RenderWindow* window,
		uint32_t win_x,
		uint32_t win_y,
		uint32_t unit_col,
		uint32_t unit_row = 0
	) {
		IntRect atlas_rect(
			unit_col*w,
			unit_row*h,
			w,
			h
		);
		sprite.setTextureRect(atlas_rect);
		sprite.setPosition(win_x, win_y);
		window->draw(sprite);
	}
};


///////////////////////////////////////////////////////////////////////////////
// Emulator.

static alt_isr_func _timer_isr = NULL;
static void* _timer_isr_context = NULL;

void alt_ic_isr_register(
	alt_u32 ic_id,
	alt_u32 irq,
	alt_isr_func isr,
	void *isr_context,
	void *flags
) {
	// Non-used.
	(void) ic_id;
	(void) irq;
	(void) flags;
	
	_timer_isr = isr;
	_timer_isr_context = isr_context;
}

#define SW_EVENT(n) \
	case Keyboard::Num##n: \
	case Keyboard::Numpad##n: \
		sw[n] = !sw[n]; \
		break;

class LPRS1_MAX1000_Board_Emulator {
private:
	RenderWindow* window;
	mutex window_mutex;
	thread* main_thread;
	
	volatile lprs2_simple_pio_mem_map pio_mm;
	lprs2_simple_pio_mem_map prev_pio_mm;
	volatile bool led[8];
	volatile bool sw[8];
	volatile lprs2_7segm_mem_map digits_mm;
	volatile uint8_t digits[4];
	Clock timer_clock;
	volatile lprs2_timer_mem_map timer_mm;
	lprs2_timer_mem_map prev_timer_mm;
	struct timer_t {
		uint32_t cnt;
		uint32_t modulo;
		bool reset;
		bool pause;
		bool wrapped;
	};
	volatile timer_t timer;
	volatile timer_t prev_timer;
	
	thread* mm_thread;
	
	LinearAtlas led_atlas;
	LinearAtlas sw_atlas;
	LinearAtlas digits_atlas;
	
	Clock frame_clock;
#if SHOW_HEAD_UP_DISPLAY
	Font font;
	Text text;
	uint32_t fps;
	uint32_t Hz;
	char text_buf[30];
#endif
	
	void close_window() {
		{
			unique_lock<mutex> lock(window_mutex);
			window->close();
		}
		exit(0);
	}
	bool is_window_open() {
		unique_lock<mutex> lock(window_mutex);
		return window->isOpen();
	}
	
	void update_inputs() {
		Event event;
		{
			unique_lock<mutex> lock(window_mutex);
			if(!window->pollEvent(event)){
				return;
			}
		}
		
		if(event.type == Event::Closed){
			close_window();
			return;
		}
		if(
			event.type == Event::KeyPressed &&
			event.key.code == Keyboard::Escape
		){
			close_window();
			return;
		}

		if(
			event.type == Event::KeyPressed
		){
			switch(event.key.code){
			SW_EVENT(0)
			SW_EVENT(1)
			SW_EVENT(2)
			SW_EVENT(3)
			SW_EVENT(4)
			SW_EVENT(5)
			SW_EVENT(6)
			SW_EVENT(7)
			default:
				break;
			}
		}
	}
	
	void wait_until_us(int us) {
		while(1){
			update_inputs();
			
			if(frame_clock.getElapsedTime() > microseconds(us)){
				break;
			}
		}
	}

	void mm_body() {
		int64_t prev_us;
		timer_clock.restart();
		
		
		while(1){

			// Save to prev.
			uint32_t prev_timer_cnt = timer.cnt;
			// Timer logic.
			if(timer.reset){
				timer.cnt = 0;
			}else{
				int64_t now_us = timer_clock.getElapsedTime().asMicroseconds();
				if(!timer.pause){
					int32_t cnt_diff = (now_us - prev_us)*12;
					timer.cnt += cnt_diff;
					if(timer.cnt >= timer.modulo){
						timer.cnt -= timer.modulo;
						
						// Safety feature:
						// Longer delay or someone lowered modulo.
						if(timer.cnt >= timer.modulo){
							timer.cnt = 0;
						}
						
						timer.wrapped = 1;
						UPDATE_8(
							timer_mm.g31.flags.wrapped,
							timer_mm.g31.wrapped.b,
							timer_mm.g32.flags.wrapped,
							timer_mm.g32.wrapped.b,
							timer_mm.g33.flags.wrapped,
							timer_mm.g33.wrapped.b,
							timer_mm.g34.flags.wrapped,
							timer_mm.g34.wrapped.b,
							timer.wrapped
						);
						if(_timer_isr){
							_timer_isr(_timer_isr_context);
						}
					}
				}
				prev_us = now_us;
			}
			if(timer.cnt != prev_timer_cnt){
				UPDATE_4(
					timer_mm.g31.cnt,
					timer_mm.g32.cnt,
					timer_mm.g33.cnt,
					timer_mm.g34.cnt,
					timer.cnt
				);
			}
			
			
			sleep(microseconds(1));
			
			
			UPDATE_IF_CHANGED_2(
				pio_mm.u.led0,
				pio_mm.p.led0,
				led[0]
			);
			UPDATE_IF_CHANGED_2(
				pio_mm.u.led1,
				pio_mm.p.led1,
				led[1]
			);
			UPDATE_IF_CHANGED_2(
				pio_mm.u.led2,
				pio_mm.p.led2,
				led[2]
			);
			UPDATE_IF_CHANGED_2(
				pio_mm.u.led3,
				pio_mm.p.led3,
				led[3]
			);
			UPDATE_IF_CHANGED_2(
				pio_mm.u.led4,
				pio_mm.p.led4,
				led[4]
			);
			UPDATE_IF_CHANGED_2(
				pio_mm.u.led5,
				pio_mm.p.led5,
				led[5]
			);
			UPDATE_IF_CHANGED_2(
				pio_mm.u.led6,
				pio_mm.p.led6,
				led[6]
			);
			UPDATE_IF_CHANGED_2(
				pio_mm.u.led7,
				pio_mm.p.led7,
				led[7]
			);
			UPDATE_2(
				pio_mm.u.sw0,
				pio_mm.p.sw0,
				sw[0]
			);
			UPDATE_2(
				pio_mm.u.sw1,
				pio_mm.p.sw1,
				sw[1]
			);
			UPDATE_2(
				pio_mm.u.sw2,
				pio_mm.p.sw2,
				sw[2]
			);
			UPDATE_2(
				pio_mm.u.sw3,
				pio_mm.p.sw3,
				sw[3]
			);
			UPDATE_2(
				pio_mm.u.sw4,
				pio_mm.p.sw4,
				sw[4]
			);
			UPDATE_2(
				pio_mm.u.sw5,
				pio_mm.p.sw5,
				sw[5]
			);
			UPDATE_2(
				pio_mm.u.sw6,
				pio_mm.p.sw6,
				sw[6]
			);
			UPDATE_2(
				pio_mm.u.sw7,
				pio_mm.p.sw7,
				sw[7]
			);
			UPDATE_8(
				pio_mm.u._res_led0,
				pio_mm.u._res_led1,
				pio_mm.u._res_led2,
				pio_mm.u._res_led3,
				pio_mm.u._res_led4,
				pio_mm.u._res_led5,
				pio_mm.u._res_led6,
				pio_mm.u._res_led7,
				0
			);
			UPDATE_8(
				pio_mm.u._res_sw0,
				pio_mm.u._res_sw1,
				pio_mm.u._res_sw2,
				pio_mm.u._res_sw3,
				pio_mm.u._res_sw4,
				pio_mm.u._res_sw5,
				pio_mm.u._res_sw6,
				pio_mm.u._res_sw7,
				0
			);
			UPDATE_2(
				pio_mm.p._res_led,
				pio_mm.p._res_sw,
				0
			);
			for(int i = 0; i < 14; i++){
				pio_mm._res[i] = 0xbabadeda;
			}
			
			
			
			UPDATE_IF_CHANGED_8(
				digits_mm.g31.u.segm0,
				digits_mm.g31.p.segm0,
				digits_mm.g32.u.segm0,
				digits_mm.g32.p.segm0,
				digits_mm.g33.u.segm0,
				digits_mm.g33.p.segm0,
				digits_mm.g34.u.segm0,
				digits_mm.g34.p.segm0,
				digits[0]
			);
			UPDATE_IF_CHANGED_8(
				digits_mm.g31.u.segm1,
				digits_mm.g31.p.segm1,
				digits_mm.g32.u.segm1,
				digits_mm.g32.p.segm1,
				digits_mm.g33.u.segm1,
				digits_mm.g33.p.segm1,
				digits_mm.g34.u.segm1,
				digits_mm.g34.p.segm1,
				digits[1]
			);
			UPDATE_IF_CHANGED_8(
				digits_mm.g31.u.segm2,
				digits_mm.g31.p.segm2,
				digits_mm.g32.u.segm2,
				digits_mm.g32.p.segm2,
				digits_mm.g33.u.segm2,
				digits_mm.g33.p.segm2,
				digits_mm.g34.u.segm2,
				digits_mm.g34.p.segm2,
				digits[2]
			);
			UPDATE_IF_CHANGED_8(
				digits_mm.g31.u.segm3,
				digits_mm.g31.p.segm3,
				digits_mm.g32.u.segm3,
				digits_mm.g32.p.segm3,
				digits_mm.g33.u.segm3,
				digits_mm.g33.p.segm3,
				digits_mm.g34.u.segm3,
				digits_mm.g34.p.segm3,
				digits[3]
			);
			for(int i = 0; i < 3; i++){
				digits_mm.g31._res[i] = 0;
				digits_mm.g32._res[i] = 0;
				digits_mm.g33._res[i] = 0;
				digits_mm.g34._res[i] = 0;
			}
			
			
			
			UPDATE_IF_CHANGED_4(
				timer_mm.g31.cnt,
				timer_mm.g32.cnt,
				timer_mm.g33.cnt,
				timer_mm.g34.cnt,
				timer.cnt
			);
			UPDATE_IF_CHANGED_4(
				timer_mm.g31.modulo,
				timer_mm.g32.modulo,
				timer_mm.g33.modulo,
				timer_mm.g34.modulo,
				timer.modulo
			);
			UPDATE_4(
				timer_mm.g31.magic,
				timer_mm.g32.magic,
				timer_mm.g33.magic,
				timer_mm.g34.magic,
				0xbabadeda
			);
			UPDATE_IF_CHANGED_8(
				timer_mm.g31.flags.reset,
				timer_mm.g31.reset.b,
				timer_mm.g32.flags.reset,
				timer_mm.g32.reset.b,
				timer_mm.g33.flags.reset,
				timer_mm.g33.reset.b,
				timer_mm.g34.flags.reset,
				timer_mm.g34.reset.b,
				timer.reset
			);
			UPDATE_IF_CHANGED_8(
				timer_mm.g31.flags.pause,
				timer_mm.g31.pause.b,
				timer_mm.g32.flags.pause,
				timer_mm.g32.pause.b,
				timer_mm.g33.flags.pause,
				timer_mm.g33.pause.b,
				timer_mm.g34.flags.pause,
				timer_mm.g34.pause.b,
				timer.pause
			);
			// Not realy using it.
			UPDATE_8(
				timer_mm.g31.flags.wrap,
				timer_mm.g31.wrap.b,
				timer_mm.g32.flags.wrap,
				timer_mm.g32.wrap.b,
				timer_mm.g33.flags.wrap,
				timer_mm.g33.wrap.b,
				timer_mm.g34.flags.wrap,
				timer_mm.g34.wrap.b,
				0
			);
			UPDATE_IF_CHANGED_8(
				timer_mm.g31.flags.wrapped,
				timer_mm.g31.wrapped.b,
				timer_mm.g32.flags.wrapped,
				timer_mm.g32.wrapped.b,
				timer_mm.g33.flags.wrapped,
				timer_mm.g33.wrapped.b,
				timer_mm.g34.flags.wrapped,
				timer_mm.g34.wrapped.b,
				timer.wrapped
			);
			UPDATE_4(
				timer_mm.g31.flags._res,
				timer_mm.g32.flags._res,
				timer_mm.g33.flags._res,
				timer_mm.g34.flags._res,
				0
			);
			UPDATE_4(
				timer_mm.g31.reset._res,
				timer_mm.g32.reset._res,
				timer_mm.g33.reset._res,
				timer_mm.g34.reset._res,
				0
			);
			UPDATE_4(
				timer_mm.g31.pause._res,
				timer_mm.g32.pause._res,
				timer_mm.g33.pause._res,
				timer_mm.g34.pause._res,
				0
			);
			UPDATE_4(
				timer_mm.g31.wrap._res,
				timer_mm.g32.wrap._res,
				timer_mm.g33.wrap._res,
				timer_mm.g34.wrap._res,
				0
			);
			UPDATE_4(
				timer_mm.g31.wrapped._res,
				timer_mm.g32.wrapped._res,
				timer_mm.g33.wrapped._res,
				timer_mm.g34.wrapped._res,
				0
			);

		}
	}
	
	void draw_setup() {
		// Setup for drawing.
		{
			unique_lock<mutex> lock(window_mutex);
			window->setActive(true);
		}
		
		
		
#if SHOW_HEAD_UP_DISPLAY
		if(!font.loadFromFile(FONT_PATH)){
			throw runtime_error("Cannot load font!");
		}
		text.setFont(font);
		text.setCharacterSize(24); // [Pixels]
		text.setStyle(Text::Bold);
#endif
	}
	
	void draw() {
#if SHOW_HEAD_UP_DISPLAY
		Color text_color = Color::Magenta;
#if (SFML_VERSION_MAJOR<<16 | SFML_VERSION_MINOR) >= 0x00020005
		text.setFillColor(text_color);
		text.setOutlineColor(Color::Transparent);
#else
		text.setColor(text_color);
#endif
		snprintf(text_buf, 30, "FPS: %5d   Hz: %4d", fps, Hz);
		text.setString(text_buf);
#endif
		{
			unique_lock<mutex> lock(window_mutex);
			
			window->clear(Color(0, 0x60, 0));
			
			for(int i = 0; i < 8; i++){
				led_atlas.draw(
					window,
					350+(7-i)*20,
					100,
					led[i]
				);
			}
			
			for(int i = 0; i < 8; i++){
				sw_atlas.draw(
					window,
					350+(7-i)*20,
					200,
					sw[i]
				);
			}
			
			for(int d = 0; d < 4; d++){
				digits_atlas.draw(
					window,
					(3-d)*(digits_atlas.getUnitWidth()+2),
					40,
					digits[d],
					d>>1
				);
			}
			
			
			
			
#if SHOW_HEAD_UP_DISPLAY
			window->draw(text);
#endif
			window->display();
		}
	}
	
	void main_body() {
		window = new RenderWindow(
			VideoMode(SCREEN_W, SCREEN_H),
			"LPRS1 MAX1000 Board Emulator"
		);
		
		draw_setup();
		
		mm_thread = new thread([&]{ mm_body(); });
		
		
		
		while(is_window_open()){
			// VSync timing.
			frame_clock.restart();
			
			draw();
			
#if SHOW_HEAD_UP_DISPLAY
			fps = round(1000000.0/frame_clock.getElapsedTime().asMicroseconds());
#endif
			
			// Wait end of frame.
			wait_until_us(FRAME_US);
			
#if SHOW_HEAD_UP_DISPLAY
			Hz = round(1000000.0/frame_clock.getElapsedTime().asMicroseconds());
#endif
			
		}
		
	}
	

public:
	
	LPRS1_MAX1000_Board_Emulator()
	: 
		led_atlas(
			SHARE_PATH"led_atlas.png",
			16,
			16
		),
		sw_atlas(
			SHARE_PATH"sw_atlas.png",
			16,
			32
		),
		digits_atlas(
			SHARE_PATH"7segm_digit_atlas.png",
			32*2,
			64*2
		) 
	{
		// For testing.
		
		
		__pio_base
			= reinterpret_cast<volatile void*>(&pio_mm);
		__digits_base 
			= reinterpret_cast<volatile void*>(&digits_mm);
		__timer_base
			= reinterpret_cast<volatile void*>(&timer_mm);
		
		for(int i = 0; i < 8; i++){
			sw[i] = 0;
			led[i] = 0;
		}
		sw[1] = sw[3] = 1; // 0x0a
		led[0] = led[2] = 1; // 0x05
		digits[0] = 0;
		digits[1] = 1;
		digits[2] = 2;
		digits[3] = 3;
		timer.cnt = 0;
		timer.modulo = 1;
		timer.reset = 1;
		timer.pause = 1;
		timer.wrapped = 0;
		
		
		
		UPDATE_2(
			pio_mm.u.led0,
			pio_mm.p.led0,
			led[0]
		);
		UPDATE_2(
			pio_mm.u.led1,
			pio_mm.p.led1,
			led[1]
		);
		UPDATE_2(
			pio_mm.u.led2,
			pio_mm.p.led2,
			led[2]
		);
		UPDATE_2(
			pio_mm.u.led3,
			pio_mm.p.led3,
			led[3]
		);
		UPDATE_2(
			pio_mm.u.led4,
			pio_mm.p.led4,
			led[4]
		);
		UPDATE_2(
			pio_mm.u.led5,
			pio_mm.p.led5,
			led[5]
		);
		UPDATE_2(
			pio_mm.u.led6,
			pio_mm.p.led6,
			led[6]
		);
		UPDATE_2(
			pio_mm.u.led7,
			pio_mm.p.led7,
			led[7]
		);
		UPDATE_2(
			pio_mm.u.sw0,
			pio_mm.p.sw0,
			sw[0]
		);
		UPDATE_2(
			pio_mm.u.sw1,
			pio_mm.p.sw1,
			sw[1]
		);
		UPDATE_2(
			pio_mm.u.sw2,
			pio_mm.p.sw2,
			sw[2]
		);
		UPDATE_2(
			pio_mm.u.sw3,
			pio_mm.p.sw3,
			sw[3]
		);
		UPDATE_2(
			pio_mm.u.sw4,
			pio_mm.p.sw4,
			sw[4]
		);
		UPDATE_2(
			pio_mm.u.sw5,
			pio_mm.p.sw5,
			sw[5]
		);
		UPDATE_2(
			pio_mm.u.sw6,
			pio_mm.p.sw6,
			sw[6]
		);
		UPDATE_2(
			pio_mm.u.sw7,
			pio_mm.p.sw7,
			sw[7]
		);
		UPDATE_8(
			pio_mm.u._res_led0,
			pio_mm.u._res_led1,
			pio_mm.u._res_led2,
			pio_mm.u._res_led3,
			pio_mm.u._res_led4,
			pio_mm.u._res_led5,
			pio_mm.u._res_led6,
			pio_mm.u._res_led7,
			0
		);
		UPDATE_8(
			pio_mm.u._res_sw0,
			pio_mm.u._res_sw1,
			pio_mm.u._res_sw2,
			pio_mm.u._res_sw3,
			pio_mm.u._res_sw4,
			pio_mm.u._res_sw5,
			pio_mm.u._res_sw6,
			pio_mm.u._res_sw7,
			0
		);
		UPDATE_2(
			pio_mm.p._res_led,
			pio_mm.p._res_sw,
			0
		);
		for(int i = 0; i < 14; i++){
			pio_mm._res[i] = 0xbabadeda;
		}
		
		
		UPDATE_8(
			digits_mm.g31.u.segm0,
			digits_mm.g31.p.segm0,
			digits_mm.g32.u.segm0,
			digits_mm.g32.p.segm0,
			digits_mm.g33.u.segm0,
			digits_mm.g33.p.segm0,
			digits_mm.g34.u.segm0,
			digits_mm.g34.p.segm0,
			digits[0]
		);
		UPDATE_8(
			digits_mm.g31.u.segm1,
			digits_mm.g31.p.segm1,
			digits_mm.g32.u.segm1,
			digits_mm.g32.p.segm1,
			digits_mm.g33.u.segm1,
			digits_mm.g33.p.segm1,
			digits_mm.g34.u.segm1,
			digits_mm.g34.p.segm1,
			digits[1]
		);
		UPDATE_8(
			digits_mm.g31.u.segm2,
			digits_mm.g31.p.segm2,
			digits_mm.g32.u.segm2,
			digits_mm.g32.p.segm2,
			digits_mm.g33.u.segm2,
			digits_mm.g33.p.segm2,
			digits_mm.g34.u.segm2,
			digits_mm.g34.p.segm2,
			digits[2]
		);
		UPDATE_8(
			digits_mm.g31.u.segm3,
			digits_mm.g31.p.segm3,
			digits_mm.g32.u.segm3,
			digits_mm.g32.p.segm3,
			digits_mm.g33.u.segm3,
			digits_mm.g33.p.segm3,
			digits_mm.g34.u.segm3,
			digits_mm.g34.p.segm3,
			digits[3]
		);
		for(int i = 0; i < 3; i++){
			digits_mm.g31._res[i] = 0;
			digits_mm.g32._res[i] = 0;
			digits_mm.g33._res[i] = 0;
			digits_mm.g34._res[i] = 0;
		}
		
		
		UPDATE_4(
			timer_mm.g31.cnt,
			timer_mm.g32.cnt,
			timer_mm.g33.cnt,
			timer_mm.g34.cnt,
			timer.cnt
		);
		UPDATE_4(
			timer_mm.g31.modulo,
			timer_mm.g32.modulo,
			timer_mm.g33.modulo,
			timer_mm.g34.modulo,
			timer.modulo
		);
		UPDATE_4(
			timer_mm.g31.magic,
			timer_mm.g32.magic,
			timer_mm.g33.magic,
			timer_mm.g34.magic,
			0xbabadeda
		);
		UPDATE_8(
			timer_mm.g31.flags.reset,
			timer_mm.g31.reset.b,
			timer_mm.g32.flags.reset,
			timer_mm.g32.reset.b,
			timer_mm.g33.flags.reset,
			timer_mm.g33.reset.b,
			timer_mm.g34.flags.reset,
			timer_mm.g34.reset.b,
			timer.reset
		);
		UPDATE_8(
			timer_mm.g31.flags.pause,
			timer_mm.g31.pause.b,
			timer_mm.g32.flags.pause,
			timer_mm.g32.pause.b,
			timer_mm.g33.flags.pause,
			timer_mm.g33.pause.b,
			timer_mm.g34.flags.pause,
			timer_mm.g34.pause.b,
			timer.pause
		);
		// Not realy using it.
		UPDATE_8(
			timer_mm.g31.flags.wrap,
			timer_mm.g31.wrap.b,
			timer_mm.g32.flags.wrap,
			timer_mm.g32.wrap.b,
			timer_mm.g33.flags.wrap,
			timer_mm.g33.wrap.b,
			timer_mm.g34.flags.wrap,
			timer_mm.g34.wrap.b,
			0
		);
		UPDATE_8(
			timer_mm.g31.flags.wrapped,
			timer_mm.g31.wrapped.b,
			timer_mm.g32.flags.wrapped,
			timer_mm.g32.wrapped.b,
			timer_mm.g33.flags.wrapped,
			timer_mm.g33.wrapped.b,
			timer_mm.g34.flags.wrapped,
			timer_mm.g34.wrapped.b,
			timer.wrapped
		);
		UPDATE_4(
			timer_mm.g31.flags._res,
			timer_mm.g32.flags._res,
			timer_mm.g33.flags._res,
			timer_mm.g34.flags._res,
			0
		);
		UPDATE_4(
			timer_mm.g31.reset._res,
			timer_mm.g32.reset._res,
			timer_mm.g33.reset._res,
			timer_mm.g34.reset._res,
			0
		);
		UPDATE_4(
			timer_mm.g31.pause._res,
			timer_mm.g32.pause._res,
			timer_mm.g33.pause._res,
			timer_mm.g34.pause._res,
			0
		);
		UPDATE_4(
			timer_mm.g31.wrap._res,
			timer_mm.g32.wrap._res,
			timer_mm.g33.wrap._res,
			timer_mm.g34.wrap._res,
			0
		);
		UPDATE_4(
			timer_mm.g31.wrapped._res,
			timer_mm.g32.wrapped._res,
			timer_mm.g33.wrapped._res,
			timer_mm.g34.wrapped._res,
			0
		);
		
		
		main_thread = new thread([&]{ main_body(); });
	}
};

static LPRS1_MAX1000_Board_Emulator LPRS1_MAX1000_Board_Emulator;


///////////////////////////////////////////////////////////////////////////////
