#include "defs.h"
#include <errno.h>

#ifndef __DEMCO_CONFIG_H
#define __DEMCO_CONFIG_H

size_t file_size(const char* fpath) {
    size_t fsize = 0;
    FILE* fp = fopen(fpath,"r");
    if (!fp) 
        return (size_t)(-1);

    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    fclose(fp);
    return fsize;
}

bool load_binary_config(Config* c, const char* fpath) {
    assert(c);
    if (!FileExists(fpath))
        return false;

    char* data = LoadFileText(fpath);
    if(data) {
        if (strlen(data) != sizeof(*c)) {
            TraceLog(LOG_WARNING, "Loaded config and expected config have mismatched sizes! INFO: %lu :: %lu", sizeof(*c), strlen(data));
            memcpy(c, data, sizeof(*c));
        }
        else if (strlen(data) < sizeof(*c))
            TraceLog(LOG_ERROR, "Loaded config is smaller than expected config, can't load it");
        else 
            memcpy(c, data, sizeof(*c));
    } else TraceLog(LOG_ERROR, "LoadFileText(\"%s\") has failed by returning NULL", fpath);
    
    UnloadFileText(data);
    return true;
}

bool save_binray_config(Config c, const char* fpath) {
    return SaveFileData(fpath, &c, sizeof(c));
}


#endif//__DEMCO_CONFIG_H
