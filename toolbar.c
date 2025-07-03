#include "defs.h"
#include "utils.c"

#ifndef __DEMCO_TOOLBAR_H
#define __DEMCO_TOOLBAR_H


#if 0
void UiTabBar(
		Rectangle rec, 
		const char** text, 
		u32 count, 
		u32* active, 
		float padding,
		float space_between)
{
	const Vector2 button_sz = { 
		( (rec.w - space_between*(count-2)) /count),
		rec.h
	};

	for(u32 i = 0; i < count; i++) {
		bool button_active = ((*active) == i);
		Rectangle button_rec = {
			padding + rec.x  + i*button_sz.x	+ space_between*i,
			rec.y + padding 						,
			button_sz.x - padding*2	- space_between	,
			button_sz.y - padding*2 ,
		};
		

		if (button_active) {
			GuiSetState(STATE_FOCUSED);
			DrawRectangleLinesEx(button_rec,1,WHITE);
		}


		if (GuiButton(button_rec, text[i])) {
			(*active) = i;
		}

		GuiSetState(STATE_NORMAL);
	}
}
#endif

void Ui_toolbar(GlobalState* s, Rectangle rec) {
	//const bool fit = (s->config.ui_flags & UICONFIG_FIT_TOOLBAR_BUTTONS_TO_PANEL_HEIGHT);
	//const bool space_evenly = (s->config.ui_flags & UICONFIG_SPACE_TOOLBAR_BUTTONS_EVENLY);

    Color darkenbg = ColorTint(
            GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)), 
            s->config.theme == THEME_LIGHT ? GetColor(0xEEEEEEFF) : GetColor(0x777777FF)
    );

    /*
	const Vector2 button_sz = { 
		100, 
		(fit) ? rec.height : BUTTON_SMALL_SIZE 
	};
    */

	const u32 padding 		= 4;
    /*
       const float spaceoffset = (space_evenly) ?
       rec.w - UI_PAGES_LEN * (button_sz.x) : 0;

       const float fitoffset = (fit) 	?
       rec.height - button_sz.y : 0 ;
       */
	//DrawRectangleLinesEx(RectangleExpand(rec,2),2,s->config.theme == THEME_LIGHT? darkenbg : GetColor(0x999999FF));
	DrawRectangleRec(rec, darkenbg);

	//float offsets[2] = { fitoffset, spaceoffset };

    const bool block =
            (s->selected_page == UiPage_Convert && s->convert_mode == FFMPEG_CONVERT) ||
            (s->selected_page == UiPage_Replace && s->convert_mode == FFMPEG_REPLACE) ;

    if (block)  GuiLock();
	UiTabBar(rec, (const char**) UI_PAGES, UI_PAGES_LEN, (u32*) &(s->selected_page), padding, 0);
    if (block) 
        UiLockOverlay(rec);
    GuiUnlock();

#if 0
	for(u32 i = 0; i < UI_PAGES_LEN; i++) {
		bool button_active = (s->selected_page == i);

		Rectangle button_rec = {
			spaceoffset*i + fitoffset + padding +
				rec.x  + i*button_sz.x				,
			fitoffset + rec.y + padding 			,
			button_sz.x - padding*2					,
			button_sz.y - padding*2  				,
		};
		if (GuiButton(button_rec, UI_PAGES[i])) {
			s->selected_page = i;
		}
		if (button_active)
			DrawRectangleLinesEx(button_rec,1,RED);
	}
#endif
}

#endif //__DEMCO_TOOLBAR_H
