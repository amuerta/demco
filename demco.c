/*
    NOTE ZONE:


*/

// Definitions of all constants
#include "defs.h"

// Utility + deprecated(unused) code

// i was going with some sort of ui callback system
// where you can have very customizable components,
// but now it's kinda not used else were besides the
// tab-bar. *shrug*
//
//  1.
// Also i learned that things i wanted can be customized 
// in raygui via GuiSetStyle(int,int,int) so it's even more
// useless -\(o.o)/-
#include "uiadds.c"
#include "utils.c"

// Config files
#include "binary_config.c"
//#include "json_config.c"

// UI
#include "popup.c"
#include "toolbar.c"

// Data + cmd
#include "validation.c"
#include "ffmpeg_integration.c"

// Pages
#include "options.c"
#include "convert.c"
#include "replace.c"

// DEFAULT THEMES
#include "themes/dark.h"
#include "themes/evergreen.h"
#include "themes/light.h"

void Ui_root(GlobalState* s, Rectangle rec) {
    
	Rectangle elements[2];  

	Ui_vsplit(elements, rec, TOOLBAR_HEIGHT);
	Rectangle 
		toolbar = elements[0],
		page	= elements[1]
	;

	Ui_toolbar(s,toolbar);
	Ui_page(s,page);
}

void Ui_page(GlobalState* s, Rectangle rec) {
	// TODO: finish all pages
	const UiElementFn pages[] = { Ui_page_convert, Ui_page_replace, Ui_page_options , NULL };
    
	if (s->selected_page < UI_PAGES_LEN && pages[s->selected_page]) 
		pages[s->selected_page](s,rec);
	else 
		Ui_page_default(s,rec);
}


void Ui_page_default	(GlobalState* s, Rectangle rec) {
	const char* message = 
        "Demco - DEsktop Media COnverter"       "\n"
        "Program made by amuerta"               "\n"
        "demco distributed under MIT license"   "\n"
	;

    const float UI_SCALE = s->config.font_scale;
    const int FONT_DEFAULT = FONT_SIZE * UI_SCALE;
    const int FONT_HEADER = FONT_DEFAULT * 2.5;
	Rectangle   elements[2] = {0};

    Rectangle r = RectangleExpand(rec,-64);

    Ui_vsplit(elements, r, r.height - FONT_HEADER*2);
    Rectangle text = elements[0];

    Ui_vsplit(elements, elements[1], FONT_HEADER);
    Rectangle github_button = RectangleExpand(elements[0],-4);

    Ui_vsplit(elements, elements[1], FONT_HEADER);
    Rectangle usage_button = RectangleExpand(elements[0],-4);

    GuiLabel(text, message);
    if (GuiButton(github_button, "Github source")) 
        OpenURL("https://github.com/amuerta/demco");
    if(GuiButton(usage_button, "Documentation"))
        OpenURL("https://github.com/amuerta/demco/blob/master/README.md");
}

InputBuffer InputBuffer_alloc(usize count, usize size) {
	InputBuffer buffer = {
        .names = RL_MALLOC(sizeof(char*) * count),
		.paths = RL_MALLOC(sizeof(char*) * count),
		.count_limit = count,
		.length_limit = size,
		.count = 0,
	};
	for (usize i = 0; i < size; i++)  {
		buffer.names[i] = RL_MALLOC(size);
		buffer.paths[i] = RL_MALLOC(size);
		memset(buffer.names[i],0,size);
		memset(buffer.paths[i],0,size);
	}

	return buffer;
}

void InputBuffer_free(InputBuffer* b) {
	for (usize i = 0; i < b->count_limit; i++) {
        RL_FREE(b->names[i]);
		RL_FREE(b->paths[i]);
    }
	RL_FREE(b->paths);
	RL_FREE(b->names);
}

#define BORDER_WIDTH 1

void Ui_overlay(GlobalState* s) {
    //Rectangle border = {1,1,GetScreenWidth()-BORDER_WIDTH*2, GetScreenHeight()-BORDER_WIDTH*2};
    Rectangle border = {0,0,GetScreenWidth(), GetScreenHeight()};
    PopupStack* popups = &(s->popups);
	PopupStack_update(popups);
    Color outline = GetColor(GuiGetStyle(BUTTON,BORDER_COLOR_FOCUSED));
	DrawRectangleLinesEx(border,1,outline);
}

FILE* App_log_stdout(const char* out_path) {
#if 0
    #if _WIN32
        const char* stdop = "CON";
    #else // posix
        const char* stdop = "/dev/tty";
    #endif
#endif
    // stdop
    TraceLog(LOG_INFO, "STARTING TO LOG ALL STDOUT INTO '%s'", out_path);
    return freopen(out_path, "a+", stdout);
}

void App_init(GlobalState* s) {
    //load default labels
    memcpy(&(s->cache.themelist),THEMES_STR, strlen(THEMES_STR));

    // config loading
    Config cfg = {0};
    if(load_binary_config(&cfg, s->config_file)) {
        TraceLog(LOG_INFO, "Loaded config");
        const bool 
            fit_toolbar          = cfg.ui_flags & UICONFIG_FIT_TOOLBAR_BUTTONS_TO_PANEL_HEIGHT,
            space_buttons_evenly = cfg.ui_flags & UICONFIG_SPACE_TOOLBAR_BUTTONS_EVENLY
        ;

        // copy entire config
        memcpy(&(s->config), &cfg, sizeof(cfg));

        // toggle off defaults if said so in cfg
        // prevent weird behaviour of invalid values, like.. font_size being 0
        if (!fit_toolbar)
            s->config.ui_flags ^= UICONFIG_FIT_TOOLBAR_BUTTONS_TO_PANEL_HEIGHT;
        if (!space_buttons_evenly)
            s->config.ui_flags ^= UICONFIG_SPACE_TOOLBAR_BUTTONS_EVENLY;
        if (s->config.font_scale < 1.0) 
            s->config.font_scale = 1.0;
        
    } else 
        TraceLog(LOG_INFO, "Have not found binary config file, skipping attempt at loading it");
    
    
    if (s->config.feature_flags & FEATURE_LOG_STDOUT_TO_FILE)
        s->logger = App_log_stdout(LOG_PATH);
}

void App_deinit(GlobalState* s) {
    if (save_binray_config(s->config, s->config_file)) {
        TraceLog(LOG_INFO, "Saved config to %s", s->config_file);
    } else 
        TraceLog(LOG_ERROR, "Failed to save config. errno: '%i'\n\t", errno);
    InputBuffer_free(&(s->input_files));

    if (s->logger)
        fclose(s->logger);
    else
        TraceLog(LOG_WARNING, "Failed to save log");
} 

void App_logstate(GlobalState* s) {
    debug("replace input types:  [ %i, %i ]\n", s->media_type_input[0],   s->media_type_input[1] );
    debug("replace output types: [ %i, %i ]\n", s->media_type_output[0],  s->media_type_output[1]);
}

void App_react_on_change(GlobalState* s) {
    Config* cfg = &(s->config);
    static u8 prev_theme;

    if (cfg->theme != prev_theme) {
        switch(cfg->theme) {
            case THEME_DARK: 
                GuiLoadStyleDark(); break;
            case THEME_EVERGREEN:
                GuiLoadStyleJungle(); break;
            case THEME_LIGHT:
                GuiLoadStyleBluish(); break;

                // TODO: Load used theme
            default: 
                break;
        } 
    }

    prev_theme = cfg->theme;
}


Color Ui_get_background(GlobalState* s) {
    Color bg = ColorTint(
            GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)), 
            s->config.theme == THEME_LIGHT ? WHITE : GetColor(0xAAAAAAFF)
    );
    return bg;
}

int main(void) {
	const Rectangle WINDOW_REC = {0,0,500,400};
	
	GlobalState app = {
        .selected_page = UiPage_Convert,
        .show_infobar = true,
		.input_files = InputBuffer_alloc(1024,1024),
		.popups	= {0},
        .drop_buffer = {0},

        .ffmpeg_path = DEFAULT_FFMPEG_PATH,

        // defaults
        .config_file = get_config_dir_path(),
        .config = {
            .ui_flags = 0  
                | UICONFIG_FIT_TOOLBAR_BUTTONS_TO_PANEL_HEIGHT
                | UICONFIG_SPACE_TOOLBAR_BUTTONS_EVENLY,
            .font_scale = 1.0,
        }
	};

    TraceLog(LOG_INFO,"SYSTEM CFG PATH: %s",app.config_file);

    App_init(&app);
    SetTargetFPS(FPS);
    SetConfigFlags(
            FLAG_MSAA_4X_HINT 
            | FLAG_VSYNC_HINT 
            //| FLAG_WINDOW_RESIZABLE
            | FLAG_WINDOW_ALWAYS_RUN
    );

	InitWindow(
			RecDimensions(WINDOW_REC),
			"Desktop Media Converter"
	);
#ifdef RELEASE
    SetExitKey(KEY_F4);
#endif

    GuiLoadStyleDark();
    GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE);

	while(!WindowShouldClose()) {
        Vector2 screen = {GetScreenWidth(),GetScreenHeight()};
        
        // load drop_buffer
        if (IsFileDropped()) 
            app.drop_buffer = LoadDroppedFiles();

        // update software state on config change
        App_react_on_change(&app);
        
        // single "cycle"
        BeginDrawing(); {
            ClearBackground(Ui_get_background(&app));

            if (app.convert_mode != FFMPEG_CONVERT) {
                Ui_root(&app,REC {0,0,UnfoldVec2(screen)});
            }
            Ui_overlay(&app);

            // input blocking convert (single threaded)
            if (app.convert_mode == FFMPEG_CONVERT) {
                ffmpeg_convert(&app);
            } else if (app.convert_mode == FFMPEG_REPLACE) {
                ffmpeg_replace(&app);
            }
        } EndDrawing();

        // unload drop_buffer
        UnloadDroppedFiles(app.drop_buffer);
        app.drop_buffer = (FilePathList){0};
	}

    App_deinit(&app);
	CloseWindow();

    App_logstate(&app);
}
