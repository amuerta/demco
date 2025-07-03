#include "defs.h"
#include "utils.c"

#ifndef __DEMCO_UIADDS_H
#define __DEMCO_UIADDS_H

typedef int (*ImmUiCallback) (Rectangle,char*, void*);

int __UiRayguiButton(Rectangle r, char* t, void* p) {
	IGNORE_VALUE (p);
	return GuiButton(r,t);
}

int __UiRayguiLabelOutline(Rectangle r, char* t, void* p) {
	IGNORE_VALUE (p);
	r = Ui_hstep(r,10);
	return GuiLabelButton(r,t);
}

int __UiSelectedOutline(Rectangle r, char* t, void* p){
	IGNORE_VALUE (p);
	IGNORE_VALUE (t);
    Color color = GetColor(GuiGetStyle(DEFAULT,BORDER_COLOR_FOCUSED));
	DrawRectangleLinesEx(r,1,color);
	return 0;
}

int __UiRayguiPane(Rectangle r, char* t, void* p) {
	IGNORE_VALUE (p);
	IGNORE_VALUE (t);
	GuiDummyRec(r,"");
    Color color = GetColor(GuiGetStyle(DEFAULT,BORDER_COLOR_NORMAL));
	DrawRectangleLinesEx(r,1,color);
	return 0;
}

void __UiBar(
		Rectangle rec, 
		const char** text, 
		u32 count, 
		u32* active, 
		float padding,
		float space_between,
		ImmUiCallback body_callback,
		ImmUiCallback button_callback,
		ImmUiCallback focused_callback)
{
	const Vector2 button_sz = { 
		( (rec.w - space_between*(count-2)) /count),
		rec.h
	};

	if (body_callback) 
		body_callback(rec,NULL,NULL);

	for(u32 i = 0; i < count; i++) {
		bool button_active = ((*active) == i);
		Rectangle button_rec = {
			padding + rec.x  + i*button_sz.x	+ space_between*i,
			rec.y + padding 						,
			button_sz.x - padding*2	- space_between	,
			button_sz.y - padding*2 ,
		};

		if (button_active)  {
			GuiSetState(STATE_FOCUSED);
			if (focused_callback) 
				focused_callback(button_rec, MUTSTR(text[i]),NULL);
		}
		if (button_callback && button_callback(button_rec, MUTSTR(text[i]),NULL)) 
			(*active) = i;
		
		
		GuiSetState(STATE_NORMAL);
	}
}

void UiBlockBar(
		Rectangle rec, 
		const char** text, 
		u32 count, 
		u32* active, 
		float padding,
		float space_between) 
{
	__UiBar(rec,text,count,
			active,
			padding,space_between,
			__UiRayguiPane,
			__UiRayguiLabelOutline,
			__UiSelectedOutline);
}

void UiTabBar(
		Rectangle rec, 
		const char** text, 
		u32 count, 
		u32* active, 
		float padding,
		float space_between) 
{
	__UiBar(rec,text,count,
			active,
			padding,space_between,
			NULL,__UiRayguiButton,NULL);
}

#define BLOCK_LINE_SPACING 20
#define BLOCK_LINE_THICKNESS 4
#define BLOCK_LINE_ANIMATION_SPEED 0.5

void UiLockOverlay(Rectangle rec) {
    static float time_ticks = 0;
    Vector2 base = {1,1};

    Color color = GetColor(0xFF5300A0);

    Rectangle lines = RectangleExpand(rec,-BLOCK_LINE_THICKNESS);
    BeginScissorMode(UnfoldRec(lines));
    int count = (int)(rec.width / BLOCK_LINE_SPACING);
    for(int i = -count*2; i < count*4; i++) {
        float offset = (float) ((size_t) time_ticks % (size_t)(rec.width * 2)); 
        //float speed = BLOCK_LINE_ANIMATION_SPEED;
        Vector2
            begin   = {rec.x + BLOCK_LINE_SPACING*i - rec.width/2 + (offset) , rec.y},
            end     = Vector2Add(begin, Vector2Scale(base, 1.5*rec.height));
        DrawLineEx(begin,end,BLOCK_LINE_THICKNESS,color);
    }
    EndScissorMode();

    DrawRectangleRec(rec, Fade(color,0.2));
    DrawRectangleLinesEx(RectangleExpand(rec,-BLOCK_LINE_THICKNESS/2),BLOCK_LINE_THICKNESS, color);
    time_ticks += BLOCK_LINE_ANIMATION_SPEED;
}

#endif //__DEMCO_UIADDS_H
