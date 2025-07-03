#ifndef __DEMCO_GENERIC_UI_H
#define __DEMCO_GENERIC_UI_H

void Ui_convert_target(GlobalState *s, Rectangle rec) {
	
	const size_t TYPE_STR_COUNTS[] = {
		UI_FORMAT_IMAGE_LEN,
		UI_FORMAT_AUDIO_LEN,
		UI_FORMAT_VIDEO_LEN
	};

	const char* TYPE_NAMES[] = {
		UI_FORMAT_IMAGE_STR,
		UI_FORMAT_AUDIO_STR,
		UI_FORMAT_VIDEO_STR
	};

	const char** TYPE_VECTORS[] = {
		UI_FORMAT_IMAGE,
		UI_FORMAT_AUDIO,
		UI_FORMAT_VIDEO
	};

    IGNORE_VALUE TYPE_STR_COUNTS;

	static int active = 0;
	static int typeid = 0;
	//static bool editmode = 0;
	
	Rectangle elements[3] = {0};
	const float padding = 4 ;

	// vertical padding only
	rec = Ui_vstep(rec,padding);
	rec = Ui_vchop(rec,padding*3);

	Ui_vsplitn(elements,rec,3);

	Rectangle
		label				= elements[0],
		categoryselector	= elements[1],
		typeselector		= elements[2]
	;

	typeselector = Ui_vstep(typeselector, padding);

	const char* type = TYPE_NAMES[ active ];
	const char* label_str = TextFormat("Converting to %s %s", 
			TYPE_VECTORS[active][typeid],
            UI_MEDIA_TYPES[active]
	);
	GuiLabel(label,label_str);
	
		//editmode = !editmode;
	GuiComboBox(categoryselector,UI_MEDIA_TYPES_STR,&active);
	GuiComboBox(typeselector, type, &typeid) ;


    s->media_type_output[0] = active;
    s->media_type_output[1] = typeid;
}

void Ui_output_textbox(GlobalState* s,Rectangle rec);

void Ui_selection(GlobalState* s, Rectangle rec) {
	Rectangle elements[2] = {0};
	Rectangle* 	element_ptr = (&elements[0]);
	const float padding = 8;
	rec = RectangleExpand(rec,-padding);

	Ui_vsplit(element_ptr, rec, 160);
	Ui_vswap(element_ptr);
    Rectangle
        droparea = elements[0],
        other    = elements[1]
    ;
	Ui_vsplit(element_ptr, other, 32);
	Ui_vswap(element_ptr);

    Rectangle
		convert	 = elements[0],
		name	 = elements[1]
	;
	
	//UiCenteredText(droparea, "Drop down files to convert");
	
	Ui_droparea(s, droparea);
	Ui_convert_target(s, convert);
	Ui_output_textbox(s,name);
	
}


void Ui_output_textbox(GlobalState* s,Rectangle rec) {
	Vector2 mouse = GetMousePosition();
	PopupStack* popups = &(s->popups);
	char* text = (s->output_file);
	static bool edit_mode = false;
    FilePathList dropped = {0};

    if (IsFileDropped()) {
        dropped = s->drop_buffer; //LoadDroppedFiles(); - memory leak
    }

    if(dropped.paths && CheckCollisionPointRec(mouse,rec)) {
        debug("Got dropped files, count : %lu", dropped.count);
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


	if ( GuiTextBox(rec, text, sizeof(str1024), edit_mode) ) {
		edit_mode = !edit_mode;
	}

	// NOTE: maybe add autocompletion in future?
	if (edit_mode && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_C)) 
		memset(text,0,sizeof(str1024));

	if (!edit_mode && !strlen(text)) 
		GuiLabel(Ui_hstep(rec,16), "Provide output path");
}

void Ui_droparea(GlobalState* s, Rectangle rec) {

	PopupStack* popups = &(s->popups);
	Vector2 mouse = GetMousePosition();
	char** 	paths = s->input_files.paths;
	char** 	names = s->input_files.names;
	u16*	paths_count = &(s->input_files.count);
	bool 	same_type       = true;
	bool 	belong_to_image = true;
	FilePathList dropped = s->drop_buffer;

	if (dropped.paths && CheckCollisionPointRec(mouse,rec)) {
		const char* first_extension = GetFileExtension(dropped.paths[0]);

		// validate files are sutiable
		for(u16 i = 0; i < (u16) dropped.count; i++) {
			if (!IsPathFile(dropped.paths[i])) {
				PopupStack_push(popups, TextFormat(
							"Got directory '%s',\nexpected files (read help tab)",
							GetFileName(dropped.paths[i])
				));
				return;
			}
			else if (!GetFileExtension(dropped.paths[i])) {
				PopupStack_push(popups, TextFormat(
							"File '%s' doesn't have an extension",
							GetFileName(dropped.paths[i])
				));
				return;
			}

            char* extension = MUTSTR(GetFileExtension(dropped.paths[i]));
            if (extension && strlen(extension)>1)
                extension += 1; // skip '.' in ".extension"

            belong_to_image = belong_to_image && str_belongs_to_list(
                    extension,
                    UI_FORMAT_IMAGE,
                    UI_FORMAT_IMAGE_LEN
            );
            debug("extension :: %s\n", GetFileExtension(dropped.paths[i]));
            
			same_type = same_type && strcmp(
					first_extension,
					GetFileExtension(dropped.paths[i])) == 0;
		}

#if 0
		if (!same_type) {
			PopupStack_push(popups, "Expected files of the same type!");
			return;
		}
#endif

		if (!belong_to_image) {
			PopupStack_push(popups, "Expected files of image type!");
			return;
		}

		// TODO: implement recursive loading of the files, and directory loading
		for(u16 i = 0; i < (u16) dropped.count; i++) {
            const char* current = dropped.paths[i];
            bool exists = false;

            for(int f = 0; f < *paths_count; f++) {
                exists = (strcmp(current,paths[f]) == 0);
                if (exists) break;
            }
            if (exists) {
                //printf("exists: %s\n", current);
                continue;
            }

			u16 index = *paths_count;
			(*paths_count)++;
			
			TextCopy(paths[index],dropped.paths[i]);
			TextCopy(names[index],GetFileName(current));
			debug("got: '%s', written '%s'\n",current,paths[index]);
		}

	}
	GuiGroupBox(rec,"Drop File(s)");
}


#endif//__DEMCO_GENERIC_UI_H
