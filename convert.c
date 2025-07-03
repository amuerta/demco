#include "defs.h"
#include "popup.c"
#include "generic_ui.c"

#ifndef __DEMCO_COVERT_H
#define __DEMCO_COVERT_H

void Ui_page_convert(GlobalState* s, Rectangle rec) {
	//const char* page_name = UI_PAGES[s->selected_page];
	const float padding = 4;
	const u8 EL_COUNT = 16;
    const char* minimize_label = s->show_infobar? ">\n>\n>\n" : "<\n<\n<\n";

	Rectangle 	elements[EL_COUNT];
	Rectangle* 	element_ptr = (&elements[0]);
	
	rec = RectangleExpand(rec,-padding);
	
	// split main content
	
    //Ui_hsplit(element_ptr, rec, Ui_percent(rec.w,55));
	Ui_hsplit(element_ptr, rec, 250);
	
    Rectangle
		pick 	= !s->show_infobar ? rec : element_ptr[0],
		options	= element_ptr[1]
	;

	// split picker 
    // rec   = body / minimize_infobar
    // body  = content / button
    Ui_hsplit(element_ptr, pick, 12);
	Ui_hswap(element_ptr);

    Rectangle
        body                = element_ptr[0],
        minimize_infobar    = element_ptr[1];

	Ui_vsplit   (element_ptr, body, 50);
	Ui_vswap    (element_ptr);
	Rectangle
		select_pane		= element_ptr[0],
		submit_button 	= element_ptr[1]
	;

	// Apply paddings 
	submit_button = RectangleExpand(submit_button,-4*2);
	
	// elements
	
	Ui_selection(s,select_pane);
	
    if (s->show_infobar)
        Ui_infobar(s,RectangleExpand(options,-padding));
	
    bool minimize = GuiLabelButton(minimize_infobar, minimize_label);
	
    bool add = GuiButton(submit_button,"Convert!");

	//PopupStack* popups = &(s->popups);

    // draw
    if (add) {
        s->convert_mode = FFMPEG_CONVERT;
    }
		//PopupStack_push(popups,"im a popup");

    if (minimize)
        s->show_infobar = !s->show_infobar;

    if (!s->convert_mode)
        DrawRectangleRec(submit_button, GetColor(0x11DD1155));

}


void Ui_infobar_filelist(GlobalState* s, Rectangle rec) {
	Rectangle elements[2];
	Rectangle body = rec;
	PopupStack* popups = &(s->popups);
	
    const float UI_SCALE = s->config.font_scale;
    const int FONT_DEFAULT = FONT_SIZE * UI_SCALE;
    const int FONT_HEADER = FONT_DEFAULT * 2;

    IGNORE_VALUE FONT_HEADER;

	body = Ui_vchop(body,4);
	Ui_vsplit(elements, body, 32);
	
	Rectangle
		body_label		= elements[0], 
		body_list 		= elements[1]
	;
	
    const char** names = (const char**) s->input_files.names;
	//const char** paths = (const char**) s->input_files.paths;
	u32 		 count = s->input_files.count;
	
	static int scroll_i = 0;
	int active_i = -1;
	static int focus_i = 0;


	const char* help_message = "Left click to remove";
	const char* clear_text  = "Clear";

    Ui_hsplit(elements, RectangleExpand(body_label,-4), MeasureText(clear_text, FONT_DEFAULT));
    Ui_hswap(elements);
    GuiLabel(elements[0], help_message);
	
    bool clear = GuiButton(elements[1], "Clear");

    GuiListViewEx( 
			RectangleExpand(body_list,-4), 
			names, count, 
			&scroll_i, &active_i, &focus_i);
	DrawRectangleLinesEx(body,1,GRAY);
    size_t MAX_PATH_SIZE = s->input_files.length_limit;
	
    if (clear && s->input_files.count != 0) {
        PopupStack_push(popups,TextFormat(
                "Cleared convert queue"
        ));
        for(size_t i = 0; i < s->input_files.count; i++) {
            memset(s->input_files.paths[i],0,MAX_PATH_SIZE);
            memset(s->input_files.names[i],0,MAX_PATH_SIZE);
        }
        s->input_files.count = 0;
    }

    if (active_i != -1) {
        strncpy(s->input_files.paths[active_i],"[REMOVED]", MAX_PATH_SIZE);
        strncpy(s->input_files.names[active_i],"[REMOVED]", MAX_PATH_SIZE);
    }
}

void Ui_infobar 	(GlobalState* s, Rectangle rec) {
    //static bool toggle;
    u32 *active = &(s->selected_infobar_tab);
	Rectangle elements[2];

	Rectangle* 	element_ptr = (&elements[0]);
	const float padding = 0;
		;
	//rec = RectangleExpand(rec,-padding);
    Ui_vsplit(element_ptr, rec, 24);

	Rectangle
		tabline 		= elements[0],
		body			= elements[1]
	;
	
	GuiDummyRec(body,"");

    if (s->show_infobar) {
        switch(s->selected_infobar_tab) {
            case UiInfobar_list:
                Ui_infobar_filelist(s, body); break;
            default: 
                break;
        }
    }
	UiBlockBar(
			tabline, 
			(const char**) UI_INFOBAR_TABS, 
			UI_INFOBAR_TABS_LEN, 
			active, 
			padding, 0);
}



void Ui_progressbar_screen(Rectangle screen, const char* filename, size_t i, size_t c) {
    Vector2 barsize = {GetScreenWidth()/1.5,GetScreenHeight()/10};
    Vector2 pos = {
        (screen.width - barsize.x)/2,
        (screen.height - barsize.y)/2,
    };
    Rectangle bar_rec = {
        UnfoldVec2(pos),
        UnfoldVec2(barsize)
    };
    Rectangle bar_name = {
        pos.x, 
        pos.y - barsize.y,
        screen.width,
        barsize.y
    };

    float val = i;
    float min = 0;
    float max = c;

    GuiDummyRec(screen,"");
    GuiProgressBar(bar_rec, TextFormat("%i",(int)val), TextFormat("%i", (int)max), &val, min, max);
    GuiLabel(bar_name,TextFormat("%s",filename));

}




#endif//__DEMCO_COVERT_H
