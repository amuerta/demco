#ifndef __DEMCO_DEFINITIONS_H
#define __DEMCO_DEFINITIONS_H

#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define NOB_IMPLEMENTATION
#include "nob.h"


//types
typedef char 					i8;
typedef short  					i16;
typedef int  					i32;
typedef unsigned char 			u8;
typedef unsigned short  		u16;
typedef unsigned int  			u32;
typedef size_t					usize;
typedef unsigned char 			bitmask8_t;
typedef unsigned long long int 	bitmask64_t;
typedef struct 	{ u8 c[256]; } 	str256;
typedef struct 	{ u8 c[1024]; } str1024;



// biased opinions
#define IGNORE_VALUE		(void)
#define ptrcast(P)			( (void*) (P) )
#define w 					width
#define h					height
#define REC					(Rectangle)
#define MIN(L,R)			((L) > (R)) ? (R) : (L)
#define MAX(L,R)			((L) < (R)) ? (R) : (L)
#define UnfoldVec2(V) 		V.x, V.y
#define UnfoldRec(V) 		V.x, V.y, V.width, V.height
#define RecDimensions(V) 	(V).width, (V).height
#define CONSTSTR(S)			((const char*) (S))
#define MUTSTR(S)			((char*) (S))
#define ADDRESSTO(O)		((void*) (O))

// WINDOWS >:( 

#ifdef _WIN32
#   define PATH_CHARACTER '\\'
#else
#   define PATH_CHARACTER '/'
#endif

// set default ffmpeg_path
// for windows it's where executable is
// for linux it's gloably installed ffmpeg via $PATH
#if _WIN32
#   define DEFAULT_FFMPEG_PATH ".\\ffmpeg.exe"
#else
#   define DEFAULT_FFMPEG_PATH "ffmpeg"
#endif

// DEBUG
#ifndef RELEASE
#   define debug(...) TraceLog(LOG_DEBUG,__VA_ARGS__)
#endif

#ifdef RELEASE
#   define debug(...) /*__VA_ARGS__*/
#endif

// CONSTANS

#define CONFIG_NAME "demco.bincfg"
#define LOG_PATH "demcolog.txt"

#define FPS 60
#define POPUP_LIFETIME 4 * FPS

#define MAX_POPUPS 16
#define POPUP_MAX_MESSAGE_LENGTH 256

// idk good enough amount in my op1n1on
#define POPUP_STACK_SIZE 65535

#define TOOLBAR_HEIGHT 32
#define BUTTON_SMALL_SIZE 32
#define FONT_SIZE 16

typedef struct {
	u16				top;
	unsigned char 	stack[POPUP_STACK_SIZE];
} PopupStack;

typedef struct {
	u16 lifetime; // in ticks (60 ticks per 1 second)
	u16 message_length;
	char message[];
} Popup;

typedef enum {
	UiPage_Convert,
	UiPage_Replace,
	UiPage_Options,
	UiPage_About,
} UiPage;

typedef enum {
	UiMediaType_Image		 ,
	UiMediaType_Audio		 ,
	UiMediaType_Video		 ,
	UiMediaType_AudioToVideo ,
} UiMediaType;

typedef enum {
	FormatImage_bmp,
	FormatImage_png,
	FormatImage_tga,
	FormatImage_jpg,
	FormatImage_qoi,
	FormatImage_tiff,
	FormatImage_webp,
} FormatImage;

typedef enum {
	FormatAudio_mp3,
	FormatAudio_wav,
	FormatAudio_ogg,
	FormatAudio_flac,
	FormatAudio_xm,
	FormatAudio_mod,
	FormatAudio_qoa
} FormatAudio;

typedef enum {
	FormatVideo_mp4,
	FormatVideo_mkv,
	FormatVideo_mov,
	FormatVideo_webm
} FormatVideo;

// AUDIO SERIALIZED
const char* UI_FORMAT_AUDIO[] = {
	"mp3",
	"wav",
	"ogg",
	"flac",
	"xm",
	"mod",
	"qoa"
};
const char* UI_FORMAT_AUDIO_STR =
	"mp3"	"\n"	
	"wav"	"\n"
	"ogg"	"\n"
	"flac"	"\n"
	"xm" 	"\n"
	"mod"	"\n"
	"qoa"	
;
const u32 	UI_FORMAT_AUDIO_LEN = NOB_ARRAY_LEN(UI_FORMAT_AUDIO);

// IMAGE SERIALIZED
const char* UI_FORMAT_IMAGE[] = {
	"png",
	"bmp",
	"tga",
	"jpg",
	"qoi",
	"tiff",
	"webp"
};

const char* UI_FORMAT_IMAGE_STR =
	"png" "\n"
	"bmp" "\n"
	"tga" "\n"
	"jpg" "\n"
	"qoi" "\n"
	"tiff" "\n"
	"webp"
;
const u32 	UI_FORMAT_IMAGE_LEN = NOB_ARRAY_LEN(UI_FORMAT_IMAGE);

// VIDEO SERIALIZED
const char* UI_FORMAT_VIDEO[] = {
	"mp4",
	"mkv",
	"mov",
	"webm"
};

const char* UI_FORMAT_VIDEO_STR =
	"mp4" "\n"
	"mkv" "\n"
	"mov" "\n"
	"webm"
;

const u32 	UI_FORMAT_VIDEO_LEN = NOB_ARRAY_LEN(UI_FORMAT_VIDEO);

// INFO BAR
typedef enum  {
	UiInfobar_list,
	UiInfobar_log,
	UiInfobar_options,
} UiInfobar;

const char* UI_INFOBAR_TABS[] = {
	"Files",
	//"Progress",
	//"Options",
};
const char* UI_INFOBAR_TABS_STR = 
	"Files" 		"\n"	
	"Progress"		//"\n"
	//"Options"
;
const u32 	UI_INFOBAR_TABS_LEN = NOB_ARRAY_LEN(UI_INFOBAR_TABS);

typedef enum {
	UiState_hovered 	= 1,
	UiState_interacted = 2,
} UiState;


typedef enum {
    THEME_DARK,
    THEME_EVERGREEN,
    THEME_LIGHT,
    THEME_CUSTOM,
} Theme;

typedef enum {
	UICONFIG_FIT_TOOLBAR_BUTTONS_TO_PANEL_HEIGHT 	= 1,
	UICONFIG_SPACE_TOOLBAR_BUTTONS_EVENLY 			= 2,
} UiFlags;

typedef enum {
    FEATURE_LOG_STDOUT_TO_FILE = 1,
} FeaturesFlags;

typedef enum {
    NONE,
    FFMPEG_CONVERT,
    FFMPEG_REPLACE
} FFmpegConvertMode;

typedef enum {
    // 1 2 4 8
    REPLACE_RECURSIVELY     = 16,
    REPLACE_CONTINUOUSLY    = 32,
} ConvertParameters;

typedef struct {
    char**          names;
	char**			paths;
	u16				count;
	u16				count_limit;
	u16				length_limit;
} InputBuffer;

typedef struct {
    u8              theme;
    float           font_scale;
    bitmask64_t		ui_flags;
    bitmask64_t     feature_flags;
} Config;

typedef struct {
    char themelist  [1024];
} Cache;

typedef enum {
    REPLACE_TOGGLE_WATCHMODE,
    REPLACE_TOGGLE_RESURSIVE,
    REPLACE_TOGGLE_CONVERT
} ReplaceToggles;

typedef enum {
    FILE_NOERROR,
    FILE_POINTER_ERROR,
    FILE_ZERO_SIZE
} FileError;

typedef struct {
    // numeric items
	UiInfobar		selected_infobar_tab;
	UiPage			selected_page;

    // replace, both media type and format type
    u32             media_type_input   [2];
    u32             media_type_output  [2];

    // state toggles
    u8              convert_mode;
    u8              convert_parameters;
    bool            show_infobar;
    bool            replace_toggles[3];

    // IO and retrieved data
	char			input_directory		    [1024]; // directory
	char			output_file	        	[1024];
    Cache           cache;
	InputBuffer	    input_files;
    FilePathList    drop_buffer;      
    FilePathList    watch_buffer;
    FILE*           logger;
   
    // ffmpeg runners
    char*           ffmpeg_path;
    Nob_Cmd         cmd;
    Nob_Proc        proc;
    
    // Popup data
    PopupStack	 	popups;
    
    // Options 
    char*           config_file;
    Config          config;
} GlobalState;

typedef void (*UiElementFn) (GlobalState*, Rectangle);


// DEFAULT THEME LABELS
const char* THEMES_STR = 
    "Dark"          "\n"
    "Evergreen"     "\n"
    "Light"         
;

// PAGES
const char* UI_PAGES[] = {
	"Convert",
	"Replace",
	"Options",
	"About" // Will be help in future
}; 
const u32 UI_PAGES_LEN = NOB_ARRAY_LEN(UI_PAGES);

// CONVERSION TYPES
const char* UI_MEDIA_TYPES[] = {
	"Image" 
    // TODO:
    //    ,
	//"Audio",
	//"Video" //,
	//"Video To Audio",
};
const char* UI_MEDIA_TYPES_STR = 
	"Image"	

    // TODO: 
    //"\n"
	//"Audio"	"\n"
	//"Video"	//"\n"
	//"Video To Audio"
;
const u32 	UI_MEDIA_TYPES_LEN = NOB_ARRAY_LEN(UI_MEDIA_TYPES);

// very cave man way of doing ui
void Ui_root			(GlobalState*, Rectangle);
void Ui_page			(GlobalState*, Rectangle);
void Ui_selection		(GlobalState*, Rectangle);
void Ui_infobar			(GlobalState*, Rectangle);
void Ui_page_convert	(GlobalState*, Rectangle);
void Ui_page_default	(GlobalState*, Rectangle);
void Ui_droparea        (GlobalState*, Rectangle);

#endif //__DEMCO_DEFINITIONS_H
