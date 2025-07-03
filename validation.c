#ifndef __DEMCO_VALIDATION_H
#define __DEMCO_VALIDATION_H

bool str_belongs_to_list(
		char* str, 
		const char** list, 
		size_t length) 
{
	if (str && list)
		for(size_t i = 0; i < length; i++) 
			if (list[i] && strcmp(str,list[i]) == 0)
				return true;

	return false;
}

bool extenstion_belongs_to_supported(char* extension) {
	return 
		str_belongs_to_list(extension, UI_FORMAT_AUDIO, UI_FORMAT_AUDIO_LEN) &&
		str_belongs_to_list(extension, UI_FORMAT_IMAGE, UI_FORMAT_IMAGE_LEN) &&
		str_belongs_to_list(extension, UI_FORMAT_VIDEO, UI_FORMAT_VIDEO_LEN)
	;
}

const char* in_file_extension(GlobalState* s) {
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

    IGNORE_VALUE type_names;
    IGNORE_VALUE type_str_counts;

    u32* mt = s->media_type_input;
    return type_vectors[mt[0]][mt[1]];
}

const char* out_file_extension(GlobalState* s) {
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

    IGNORE_VALUE type_names;
    IGNORE_VALUE type_str_counts;

    u32* mt = s->media_type_output;
    return type_vectors[mt[0]][mt[1]];
}

char* get_config_dir_path(void) {
    static char path[sizeof(str1024)];
    char PATH_CHARACTER_STR[2] = {PATH_CHARACTER, 0};

#if _WIN32
    char* appdata = getenv("AppData");
    strncpy(path, appdata, MIN(strlen(appdata), sizeof(str1024)));
#else
    char* cfg = getenv("HOME");;
    strncpy(path, cfg, MIN(strlen(cfg), sizeof(str1024)));
    strncat(path, "/.config",sizeof(str1024)-1);
#endif

    strncat(path, PATH_CHARACTER_STR, sizeof(str1024)-1);
    strncat(path, "demco", sizeof(str1024)-1);
#if 1
    if(!DirectoryExists(path)) {
        if(MakeDirectory(path) != 0) 
            TraceLog(LOG_ERROR, "Failed to create directory 'demco' at %s", path);
        else 
            TraceLog(LOG_INFO, "Made directory: %s", path);
    }

    strncat(path, PATH_CHARACTER_STR, sizeof(str1024)-1);
    strncat(path, CONFIG_NAME, sizeof(str1024)-1);
    if(!FileExists(path)) {
        if(!SaveFileText(path, "")) 
            TraceLog(LOG_ERROR, "Failed to create file for config: %s", path);
        else 
            TraceLog(LOG_INFO, "Made empty file: %s", path);
    }
#endif

    return path;
}

#endif// __DEMCO_VALIDATION_H
