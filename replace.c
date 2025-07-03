#include "defs.h"
#include "convert.c"

#ifndef __DEMCO_REPLACE_H
#define __DEMCO_REPLACE_H


void Ui_replace_buttons(GlobalState* s, Rectangle rec) {
	PopupStack* popups  = &(s->popups);

    const float UI_SCALE = s->config.font_scale;
    const int FONT_DEFAULT = FONT_SIZE * UI_SCALE;
    const int FONT_HEADER = FONT_DEFAULT * 2;

    IGNORE_VALUE FONT_HEADER;

    static bool checked;
    bool* toggles = s->replace_toggles;
    Rectangle elements[2];

    Rectangle button_block = rec;
    Rectangle buttons[3];
    
    const char* button_labels[] = {"Watchmode", "Recursive", "Replace"};
    const float pad = 4;
    
    // button 1
    Ui_hsplit(elements, button_block, MeasureText(button_labels[0], FONT_DEFAULT));
    elements[1] = Ui_hstep(elements[1],pad);
    elements[1] = Ui_hchop(elements[1],pad);
    buttons[0] = elements[0];
    // button 2
    Ui_hsplit(elements, elements[1], MeasureText(button_labels[1], FONT_DEFAULT));
    elements[1] = Ui_hstep(elements[1],pad);
    elements[1] = Ui_hchop(elements[1],pad);
    buttons[1] = elements[0];
    // button 3
    buttons[2] = elements[1];


    GuiToggle(buttons[0], "Automatic", toggles);
    GuiToggle(buttons[1], "Recursive", toggles+1);
    

    if (toggles[0]) {
        GuiToggle(buttons[2], "Auto Replace", toggles+2);
        if (toggles[2]) {
            if (!strlen(s->input_directory)) {
                PopupStack_push(popups,TextFormat("No target directory provided"));
                s->convert_mode = 0;
                s->replace_toggles[REPLACE_TOGGLE_CONVERT] = 0;
            } else if (!strlen(s->input_directory)) {
                PopupStack_push(popups,TextFormat("Dont have the scan path!\nProvide any"));
                s->convert_mode = 0;
                s->replace_toggles[REPLACE_TOGGLE_CONVERT] = 0;
            }   else if (strcmp(in_file_extension(s),out_file_extension(s))==0) {
                PopupStack_push(popups,TextFormat("Nothing to do (%s -> %s)",
                            in_file_extension(s),
                            out_file_extension(s)
                ));
                s->convert_mode = 0;
                s->replace_toggles[REPLACE_TOGGLE_CONVERT] = 0;
            } else {

                if (!checked) {
                    PopupStack_push(popups,TextFormat("Started replace watchmode"));
                    checked = true;
                }

                s->convert_mode = FFMPEG_REPLACE;
            }
        }
        else {
            checked = false;
            s->convert_mode = 0;
        }
    }
    else {
        if (GuiButton(buttons[2], "Replace Once")) {
            if (!strlen(s->input_directory)) {
                PopupStack_push(popups,TextFormat("No target directory provided"));
            } else if (!strlen(s->input_directory)) {
                PopupStack_push(popups,TextFormat("Dont have the scan path!\nProvide any"));
            } else {
                s->convert_mode = FFMPEG_REPLACE;
            }
        }
        toggles[2] = 0;
    }
}

void Ui_replace_textbox(GlobalState* s, Rectangle rec) {
    FilePathList dropped = {0};
	PopupStack* popups = &(s->popups);
	
    Vector2 mouse = GetMousePosition();
	char* text = (s->input_directory);
    
    static bool edit_mode;

    if (GuiTextBox(rec, text, sizeof(str1024), edit_mode) ) {
            edit_mode = !edit_mode;
        }
        // NOTE: maybe add autocompletion in future?
        if (edit_mode && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_C)) 
            memset(text,0,sizeof(str1024));

        if (!edit_mode && !strlen(text)) 
            GuiLabel(Ui_hstep(rec,16), "Provide target directory");

        if (IsFileDropped()) {
            dropped = s->drop_buffer; //LoadDroppedFiles(); - memory leak
        }

        if(dropped.paths && CheckCollisionPointRec(mouse,rec)) {
            printf("drop\n");
            if (dropped.count > 1)  {
                PopupStack_push(popups,TextFormat(
                            "Expected one output path, got %i",
                            dropped.count
                            ));
            } else if (IsPathFile(dropped.paths[0])) {
                PopupStack_push(popups,TextFormat(
                            "Expected directory, got file!",
                            dropped.count
                            ));
            } else if (!DirectoryExists(dropped.paths[0])) {
                PopupStack_push(popups,TextFormat( "Given directory, doesn't exist!"));
            }
            else 
                TextCopy(text,dropped.paths[0]);
        }
}

void Ui_replace_selector(GlobalState* s, Rectangle rec) {

    const float UI_SCALE = s->config.font_scale;
    const int FONT_DEFAULT = FONT_SIZE * UI_SCALE;
    const int FONT_HEADER = FONT_DEFAULT * 2;
    
    IGNORE_VALUE FONT_HEADER;

    Rectangle elements[2];

    const float bspacing = 8;
	static int iactive = 0;
	static int itypeid = 0;
	static int oactive = 0;
	static int otypeid = 0;

    const size_t type_str_counts[] = {
        UI_FORMAT_IMAGE_LEN,
        UI_FORMAT_AUDIO_LEN,
        UI_FORMAT_VIDEO_LEN
    };

    const char* type_names[] = {
        UI_FORMAT_IMAGE_STR,
        UI_FORMAT_AUDIO_STR,
        UI_FORMAT_VIDEO_STR
    };

    const char** type_vectors[] = {
        UI_FORMAT_IMAGE,
        UI_FORMAT_AUDIO,
        UI_FORMAT_VIDEO
    };

    IGNORE_VALUE type_vectors;
    IGNORE_VALUE type_str_counts;

    Rectangle combos[2][2];
    Rectangle labels[2];

    elements[1] = rec;


    // Large blocks
    elements[1] = Ui_vstep(elements[1],bspacing);
    Ui_vsplit(elements, elements[1], FONT_DEFAULT);
    labels[0]   = elements[0];

    elements[1] = Ui_vstep(elements[1],bspacing);
    Ui_vsplit(elements, elements[1], FONT_DEFAULT*2);
    combos[0][0]   = elements[0];

    elements[1] = Ui_vstep(elements[1],bspacing);
    Ui_vsplit(elements, elements[1], FONT_DEFAULT);
    labels[1]   = elements[0];

    elements[1] = Ui_vstep(elements[1],bspacing);
    Ui_vsplit(elements, elements[1], FONT_DEFAULT*2);
    combos[1][0]   = elements[0];


    // combos subdivision
    Ui_hsplit(elements, combos[0][0], combos[0][0].width/3);
    combos[0][0]   = elements[0];
    combos[0][1]   = elements[1];


    Ui_hsplit(elements, combos[1][0], combos[1][0].width/3);
    combos[1][0]   = elements[0];
    combos[1][1]   = elements[1];

    const char* type = type_names[ iactive ];

    GuiLabel(labels[0], "Replace all: ");
    GuiLabel(labels[1], "To the: ");

    if (s->convert_mode) {
        GuiLock();
        GuiDisable();
    }

    GuiComboBox(combos[0][0], UI_MEDIA_TYPES_STR,&iactive);
    GuiComboBox(combos[0][1], type, &itypeid) ;
    GuiComboBox(combos[1][0], UI_MEDIA_TYPES_STR,&oactive);
    GuiComboBox(combos[1][1], type, &otypeid) ;

    GuiUnlock();
    GuiEnable();

    s->media_type_input[0]    = iactive;
    s->media_type_input[1]    = itypeid;
    s->media_type_output[0]   = oactive;
    s->media_type_output[1]   = otypeid;
}

// TODO: redo ui layout
void Ui_page_replace(GlobalState* s, Rectangle rec) {
	Rectangle   elements[2] = {0};
    //Rectangle* 	element_ptr = (&elements[0]);


    const float padding = -16;
    const float spacing =  24;
    const float bspacing = 8;
    const float UI_SCALE = s->config.font_scale;
    const int FONT_DEFAULT = FONT_SIZE * UI_SCALE;
    const int FONT_HEADER = FONT_DEFAULT * 2;
    //const int BUTTON_COUNT = 3;
    const float ALL_ELEMENTS_SIZE = FONT_HEADER*6;

    //  label pathbar 
    Ui_vsplit(elements,RectangleExpand(rec, padding), FONT_DEFAULT);
    Rectangle path_label = elements[0];

    Ui_vsplit(elements,elements[1], FONT_HEADER);
    Rectangle pathbar = elements[0];

    elements[1] = Ui_vstep(elements[1],bspacing);
    Ui_vsplit(elements, elements[1], MAX(FONT_HEADER * 4, rec.height - ALL_ELEMENTS_SIZE));
    Rectangle combos   = elements[0];

    // Note 
    Ui_vsplit(elements, elements[1], FONT_SIZE*2 + spacing);
    Rectangle info = elements[0];

    // buttons 
    Ui_vsplit(elements, elements[1], FONT_HEADER);
    Rectangle button_block = elements[0];
    


    //
    GuiLabel(path_label, "Replace path: ");
    GuiLabel(info, "Note: Program will block ui input when busy doing replacements.\n"
            "To schedule auto-replace or replace interrupt, press Backspace!");
    
    Ui_replace_textbox(s,pathbar);
    Ui_replace_selector(s,combos);
    Ui_replace_buttons(s,button_block);

}

#endif//__DEMCO_REPLACE_H
