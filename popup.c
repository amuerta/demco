#ifndef __DEMCO_POPUP_H
#define __DEMCO_POPUP_H

#include "defs.h"

// Popup system / Popup stack
// simple burst-stack-based popup system


// all popups have SAME LIFETIME.
void PopupStack_push(PopupStack* s, const char* message) {
	//void* stack_ptr 	= s->stack;
	//void* top 			= s->stack + s->top;
	const u16 popup_head_sz 	
						= sizeof(Popup);
	const u8 NULL_TREM	= 1;

	size_t message_len = strlen(message);
	assert(message_len < 65000 && "Message for popup too big");

	Popup* popup = ptrcast(s->stack + s->top);

	const u16 popup_size_bytes =
		popup_head_sz + message_len + NULL_TREM;

	popup->lifetime = POPUP_LIFETIME;
	popup->message_length = message_len;
	memcpy(popup->message,message,message_len); 
	// zero should be automatically set from beginning
	// after element is removed its spot is memset 
	// to 0 in PopupStack_update(PopupStack*).

	s->top += popup_size_bytes;
}

const Vector2 POPUP_BASE_SIZE = {
	250,50
};

float Easing_easeOutCirc(float x) {
	return sqrt(1 - pow(x - 1, 2));
}

// TODO: dynamically size popups depending on the width of message text
void Popup_draw(Popup* p, u16 index, ImmUiCallback popup_draw) {

	const Vector2 window_sz = {GetScreenWidth(),GetScreenHeight()};
	
	u16 ANIMATION_TIME = 30;
	float lerp_in = 1;
	float lerp_out = 0;

	u16 duration_in = p->lifetime - (POPUP_LIFETIME - ANIMATION_TIME);
	u16 duration_out = p->lifetime;

	const bool within_start_time = p->lifetime < ANIMATION_TIME;
	const bool within_end_time = p->lifetime > p->lifetime - ANIMATION_TIME;
	const bool valid_duration = duration_in > 0 && duration_in < POPUP_LIFETIME;

	if (within_end_time && valid_duration)
		lerp_in =  1 - ((float)duration_in/(float)ANIMATION_TIME);
	else lerp_in = 1;

	if (within_start_time)
		lerp_out = ((float)duration_out/(float)ANIMATION_TIME);
	else lerp_out = 1;

	//printf("duration: %u, lerp: %f\n",duration_out,lerp_out);

	Rectangle geometry = {
		.x = window_sz.x - POPUP_BASE_SIZE.x 
			* Easing_easeOutCirc(lerp_in) * Easing_easeOutCirc(lerp_out),
		.y = window_sz.y - (POPUP_BASE_SIZE.y * (index + 1)),// * lerp_out,
		POPUP_BASE_SIZE.x,
		POPUP_BASE_SIZE.y,
	};

	//const float font_size = 16;
	const char* text = p->message;
#if 0
		TextFormat("m: `%s`, t: %u", 
			p->message,
			p->lifetime
	);
#endif

	if(p->lifetime) {
		if (popup_draw) popup_draw(geometry,MUTSTR(text),NULL);
		else {
            Color color = GetColor(GuiGetStyle(DEFAULT,BORDER_COLOR_FOCUSED));
            GuiOutlineRec(geometry, text);
            DrawRectangleLinesEx(geometry, 1, color);
        }
		
	}
}

void PopupStack_update(PopupStack* s) {
	//void* current 		= s->stack;
	void* stack_ptr 	= s->stack;
	void* top 			= s->stack + s->top;
	const u16 popup_head_sz 	
						= sizeof(Popup);
	const u8 NULL_TREM	= 1;
	u16	index = 0;
	u16	alive_count = 0;

	// TEMP OVERFLOW CHECK

    if (s->top > 65000) {
        s->top = 0;
        memset(stack_ptr, 0, POPUP_STACK_SIZE);
    }

    assert(s->top < 65000 && "Stack was oveflow'n in update");

	if (stack_ptr == top)
		return;
	
	while(stack_ptr < top) {
		Popup* popup = stack_ptr;
		const u16 popup_size_bytes =
			popup_head_sz + popup->message_length + NULL_TREM;

		// TODO: better popup theming
		Popup_draw(popup, index, NULL);

		if (popup->lifetime > 0) {
			popup->lifetime--;
			alive_count++;
		}


		index++;
		stack_ptr += popup_size_bytes;
	}

	if (!alive_count) {
		memset(s,0,sizeof(*s));
	}
}


#endif//__DEMCO_POPUP_H
