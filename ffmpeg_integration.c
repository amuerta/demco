#include "defs.h"
#include "popup.c"
#include "validation.c"
#include "convert.c"

#ifndef __DEMCO_FFMPEG_INTEGRATION_H
#define __DEMCO_FFMPEG_INTEGRATION_H

void ffmpeg_replace(GlobalState* s) {
    Nob_Cmd*    cmd     = &(s->cmd);
    //Nob_Proc*   proc    = &(s->proc);
	PopupStack* popups  = &(s->popups);
    const char* ffmpeg = CONSTSTR(s->ffmpeg_path);

    #define REPLACE_END  \
        loop = false; goto replace_end;

    Rectangle screen = {0,0,GetScreenWidth(),GetScreenHeight()};
    const char* filter = TextFormat(".%s", in_file_extension(s));
    
    bool loop       = s->replace_toggles[0];
    const bool dive = s->replace_toggles[1];
    FilePathList dirs = LoadDirectoryFilesEx(s->input_directory, filter, dive);
    

    if (!loop && dirs.count == 0 && s->convert_mode) {
        PopupStack_push(popups,TextFormat("No '%s' files found", filter));
        REPLACE_END;
    } 


    if (!loop) debug("\tReplace with filter '%s':\n", filter);
    TraceLog(LOG_INFO,"--- REPLACE ---");
    
    for(size_t i = 0; i < dirs.count; i++) {
        const char* in = dirs.paths[i];
        const char* out = nob_temp_sprintf(
                "%s%c%s.%s",
                GetDirectoryPath(in), 
                PATH_CHARACTER,
                GetFileNameWithoutExt(in),
                out_file_extension(s)
            );
        
        TraceLog(LOG_INFO,"\t%s -> %s", in , out);


        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyDown(KEY_BACKSPACE)) {
            PopupStack_push(popups,TextFormat("Interupted replace watchmode\n(Backspace pressed)"));
            s->convert_mode = 0;
            s->replace_toggles[2] = 0;
            REPLACE_END;
        }

        // reset drawing state
        EndDrawing();
        BeginDrawing();
        Ui_progressbar_screen(screen, GetFileName(in), i, dirs.count);   
        EndDrawing();
        BeginDrawing();

        // TODO: Bandage fix
        nob_cmd_append(cmd, ffmpeg, "-i");
        //nob_cmd_append(cmd, "ffmpeg.exe", "-i");
        //nob_cmd_append(cmd, nob_temp_sprintf("\"%s\"",in) );
        //nob_cmd_append(cmd, nob_temp_sprintf("\"%s\"",out));
        nob_cmd_append(cmd, in );
        nob_cmd_append(cmd, out);
        nob_cmd_append(cmd, "-y", "-hide_banner", "-loglevel", "error");

        Nob_Proc p = nob_cmd_run_async_and_reset(cmd);

        if (p == NOB_INVALID_PROC || !nob_proc_wait(p)) {
            PopupStack_push(popups,TextFormat(
                        "Failed to replace the file: \n'%s'",
                        GetFileNameWithoutExt(in)
            ));
            s->convert_mode = 0;
            s->replace_toggles[2] = 0;
            break;
        } else {
            int errcode = 0;
            bool isfile = IsPathFile(in);
            
            if (isfile) {
                errcode = remove(in);
            } 
            if (errcode != 0) {
                const char* error = TextFormat("Failed to remove file `%s` due to '%s'", in, strerror(errcode));
                PopupStack_push(popups,error);
                TraceLog(LOG_ERROR,"%s", error);
            }         
        }

    }

replace_end:
    UnloadDirectoryFiles(dirs);
    if (!loop)
        s->convert_mode = 0;
}



void ffmpeg_convert(GlobalState *s) {
    // ffmpeg -i <IN> <OUT> -y -hide_banner -loglevel error   
    Nob_Cmd*    cmd     = &(s->cmd);
    //Nob_Proc*   proc    = &(s->proc);
	PopupStack* popups  = &(s->popups);
    size_t count = s->input_files.count;
    //size_t skip = 0;
    Rectangle screen = {0,0,GetScreenWidth(),GetScreenHeight()};
    const char* ffmpeg = CONSTSTR(s->ffmpeg_path);

    if (count == 0) {
        PopupStack_push(popups,TextFormat("No files provided"));
        s->convert_mode = 0;
        return;
    } else if (!strlen(s->output_file)) {
        PopupStack_push(popups,TextFormat("Dont have the output path!\nProvide any"));
        s->convert_mode = 0;
        return;
    }

#if 0
    for(size_t i = 0; i < count; i++)  {
        const char* item = s->input_files.input_file_paths[i];
        if (strcmp(item,"[REMOVED]")==0) 
            skip++;
    }
#endif
    Nob_Proc p;

    for(size_t i = 0; i < count; i++) {
        

        const char* in = s->input_files.paths[i];
        const char* out = nob_temp_sprintf(
                "%s%c%s.%s",
                s->output_file, 
                PATH_CHARACTER,
                GetFileNameWithoutExt(in),
                out_file_extension(s)
        );

        TraceLog(LOG_INFO, "\tConverting '%s' to '%s'", in, out);

        // TODO: do something more sophisticated
        // like using bitmask array instead of strcmp(fpath, "[REMOVED]");
        if (strcmp(in,"[REMOVED]")==0) {
            continue;
        }

        
        // reset drawing state
        EndDrawing();
        BeginDrawing();
        Ui_progressbar_screen(screen, GetFileName(in), i, count);   
        EndDrawing();
        BeginDrawing();

        // TODO: bandage fix
        nob_cmd_append(cmd, ffmpeg, "-i");
        //nob_cmd_append(cmd, "ffmpeg", "-i");
        nob_cmd_append(cmd, in );
        nob_cmd_append(cmd, out);
        //nob_cmd_append(cmd, nob_temp_sprintf("\"%s\"",in) );
        //nob_cmd_append(cmd, nob_temp_sprintf("\"%s\"",out));
        nob_cmd_append(cmd, "-y", "-hide_banner", "-loglevel", "error");

        p = nob_cmd_run_async_and_reset(cmd);
        if (p == NOB_INVALID_PROC || !nob_proc_wait(p)) {
            PopupStack_push(popups,TextFormat(
                        "Failed to convert the file: \n'%s'",
                        in
            ));
            break;
        } 
    }

    if (s->convert_mode && p != NOB_INVALID_PROC) {
        PopupStack_push(popups, TextFormat("Sucessfuly converted %lu file%c", count, count == 1 ? ' ' : 's'));
    }

    s->convert_mode = 0;
}

#endif//__DEMCO_FFMPEG_INTEGRATION_H
