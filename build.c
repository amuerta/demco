#define NOB_IMPLEMENTATION
#include "nob.h"

#define EXEC_NAME "demco"

int main(int argc, char **argv)
{
	NOB_GO_REBUILD_URSELF(argc, argv);
	Nob_Cmd cmd = {0};
	
    enum {
        TARGET_WINDOW_MINGW,
        TARGET_WINDOW,
        RUN,
        DEBUG,
        SANITIZER
    };
    bool flags[] = {0, 0, 0, 0, 0};

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i],"run")==0) 
            flags[RUN] = true;
        
        if (strcmp(argv[i],"win-mingw")==0) 
            flags[TARGET_WINDOW_MINGW] = true;
        
        if (strcmp(argv[i],"win-native")==0) 
            flags[TARGET_WINDOW] = true;

        if (strcmp(argv[i],"debug")==0) 
            flags[DEBUG] = true;
        if (strcmp(argv[i],"memsan")==0) 
            flags[SANITIZER] = true;
    }

    if (flags[TARGET_WINDOW_MINGW]) {
        nob_cmd_append(&cmd, 
                "x86_64-w64-mingw32-gcc", 
                "-o", EXEC_NAME".exe", EXEC_NAME".c",
                "-Wall", "-Wextra", 
                "-I./lib_extern/lib/raylib-5.5_win64_mingw-w64/include/", 
                "-L./lib_extern/lib/raylib-5.5_win64_mingw-w64/lib/",
                "-lraylib",
                "-lwinmm",
                "-lgdi32"
                );
    } else if (flags[TARGET_WINDOW]) {
        nob_cmd_append(&cmd, 
                "gcc", 
                "-o", EXEC_NAME".exe", EXEC_NAME".c",
                "-Wall", "-Wextra", 
                "-I./lib_extern/lib/raylib-5.5_win64_mingw-w64/include/", 
                "-L./lib_extern/lib/raylib-5.5_win64_mingw-w64/lib/",
                "-lraylib",
                "-lwinmm",
                "-lgdi32"
                );
    } else {
        nob_cmd_append(&cmd, 
                "cc", 
                "-o", EXEC_NAME, EXEC_NAME".c",
                "-Wall", "-Wextra", 
                "-lraylib",
                "-lGL",
                "-lX11",
                "-lm",
                );
    }

    if(flags[DEBUG]) 
        nob_cmd_append(&cmd, 
                "-ggdb"
                );
    if (flags[SANITIZER])
        nob_cmd_append(&cmd, 
                "-fsanitize=address"
                );
    
	if (!nob_cmd_run_sync(cmd)) return 1;

    if (flags[RUN]) {
            cmd.count = 0;
            if (flags[TARGET_WINDOW])
                nob_cmd_append(&cmd, 
                        "./"EXEC_NAME".exe"
                        );
            else
                nob_cmd_append(&cmd, 
                        "./"EXEC_NAME
                        );
            if (!nob_cmd_run_sync(cmd)) return 1;
    }


	return 0;
}
