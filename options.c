#include "defs.h"
#include "utils.c"

#ifndef __DEMCO_OPTIONS_H
#define __DEMCO_OPTIONS_H

void Ui_page_options(GlobalState* s, Rectangle rec) {
    const float padding = 10;
    float       ELEMENT_HEIGHT = 30;
    float       RIGHT_PADDING = 10;
    float       LEFT_PADDING  = 30;
    float       SPLIT_WIDTH   = 250;
    int         counter = 0;
    const float ELEMENT_SPACING = 4;

	Rectangle 
		panel_rec 	= RectangleExpand(rec,-padding),
		full_rec	= {0,0,rec.width-padding*3.5,1000},
		view_rec	= {0};
	static Vector2 scroll = {0};
    static int    UI_STATE[256];

    const float UI_SCALE = s->config.font_scale;
    const int label_size_default = FONT_SIZE * UI_SCALE;
    const int label_size_header  = label_size_default * 2;

    int default_color = GuiGetStyle(DEFAULT,BORDER_COLOR_NORMAL);
    int focused_color = GuiGetStyle(DEFAULT,BORDER_COLOR_FOCUSED);
    int pressed_color = GuiGetStyle(DEFAULT,BORDER_COLOR_PRESSED);

    // i don't like flicker on click/hover that GuiScrollPanel has.
    GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, default_color);
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, default_color);
    
    GuiScrollPanel(panel_rec, NULL, full_rec,&scroll,&view_rec);
    
    GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, pressed_color);
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, focused_color);

    BeginScissorMode(UnfoldRec(view_rec)); 
    { 
        Rectangle content = {
            panel_rec.x+scroll.x,
            panel_rec.y+scroll.y,
            view_rec.width,
            view_rec.height
        };
        // local defs
        Rectangle element_dimensions = {content.x,content.y,content.width, ELEMENT_HEIGHT};

        #define GUI_HEADER(S) {\
            element_dimensions.height = label_size_header;\
            GuiSetStyle(DEFAULT, TEXT_SIZE, label_size_header);\
            GuiLabel(\
                    Ui_hchop(Ui_hstep(element_dimensions,RIGHT_PADDING),RIGHT_PADDING),\
            S);\
            element_dimensions = Ui_vstep(element_dimensions,element_dimensions.height + ELEMENT_SPACING);\
            GuiSetStyle(DEFAULT, TEXT_SIZE, label_size_default);\
            element_dimensions.height = ELEMENT_HEIGHT;\
            counter++;\
        }

        #define GUI_LINE(S) {\
            element_dimensions.height = label_size_default;\
            GuiLine(element_dimensions, S);\
            element_dimensions = Ui_vstep(element_dimensions,element_dimensions.height);\
            counter++;\
        }

        #define GUI_SPACE(SPACE) {\
            element_dimensions = Ui_vstep(element_dimensions,SPACE);\
            counter++;\
        }

        #define GUI_COMBOBOX(L,S,PTR) { \
            bool* edit = (bool*)(UI_STATE + counter);\
            element_dimensions.height = ELEMENT_HEIGHT;\
            Rectangle items[2];\
            Ui_hsplit(items, element_dimensions, SPLIT_WIDTH);\
            Ui_hswap(items);\
            bool click = \
            GuiComboBox(Ui_hchop(Ui_hstep(items[1],RIGHT_PADDING),RIGHT_PADDING+LEFT_PADDING), S, (int*)PTR);\
            GuiLabel(\
                    Ui_hchop(Ui_hstep(items[0],RIGHT_PADDING),RIGHT_PADDING),\
            L);\
            if (click) *edit = !(*edit);\
            element_dimensions = Ui_vstep(element_dimensions,element_dimensions.height + ELEMENT_SPACING);\
            counter++;\
        }
        
        #define GUI_BUTTON(L,S,BOOL, SHOW) { \
            element_dimensions.height = ELEMENT_HEIGHT;\
            Rectangle items[2];\
            Ui_hsplit(items, element_dimensions, SPLIT_WIDTH);\
            Ui_hswap(items);\
            if (SHOW) GuiSetState(STATE_PRESSED);\
            if(GuiButton(Ui_hchop(Ui_hstep(items[1],RIGHT_PADDING),RIGHT_PADDING+LEFT_PADDING), S))\
                BOOL = !BOOL;\
            GuiSetState(STATE_NORMAL);\
            GuiLabel(\
                    Ui_hchop(Ui_hstep(items[0],RIGHT_PADDING),RIGHT_PADDING),\
                    L);\
            element_dimensions = Ui_vstep(element_dimensions,element_dimensions.height + ELEMENT_SPACING);\
            counter++;\
        }


        #define GUI_SLIDER(LABEL,LL,RL,PTR) { \
            float* valbuf = (float*)(UI_STATE + counter);\
            element_dimensions.height = ELEMENT_HEIGHT;\
            Rectangle items[2];\
            Ui_hsplit(items, element_dimensions, SPLIT_WIDTH);\
            Ui_hswap(items);\
            Rectangle r = Ui_hchop(Ui_hstep(items[1],RIGHT_PADDING),RIGHT_PADDING+LEFT_PADDING);\
            GuiSliderBar(\
                    r,\
                    TextFormat("%.1f",(float)*PTR),\
                    TextFormat("%.1f",(float)RL),\
                    valbuf,LL,RL);\
            GuiLabel(\
                    Ui_hchop(Ui_hstep(items[0],RIGHT_PADDING),RIGHT_PADDING),\
            LABEL);\
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) *((float*)PTR) = *valbuf;\
            element_dimensions = Ui_vstep(element_dimensions,element_dimensions.height + ELEMENT_SPACING);\
            counter++;\
        }

        
        Cache* cache            = &(s->cache);
        Config* cfg             = &(s->config);
        bitmask64_t* features   = &(s->config.feature_flags);

        u8*     theme       = &(cfg->theme);
        float*  fscale      = &(cfg->font_scale);
        bool    log_toggle  = false;//= features & FEATURE_LOG_STDOUT_TO_FILE;

        // ELEMENTS 
        {
            GUI_SPACE(10);
            GUI_HEADER("User Interface");
            GUI_LINE(NULL);

            GUI_COMBOBOX("Theme",cache->themelist, theme);
            GUI_SLIDER("Font Scale", 1, 2, fscale);

            GUI_SPACE(10);
            GUI_HEADER("Logging");
            GUI_LINE(NULL);

            GUI_BUTTON("Log info to file",
                    *features & FEATURE_LOG_STDOUT_TO_FILE ? "Enabled" : "Disabled",
                    log_toggle, *features & FEATURE_LOG_STDOUT_TO_FILE);
            if (log_toggle)
                *features ^= FEATURE_LOG_STDOUT_TO_FILE;
                
        }

    }
    EndScissorMode();
}

#endif//__DEMCO_OPTIONS_H
