#include <stdio.h>
#include "tools.h"
#include "isomedia.h"
#import "fileimport.h"

Bool keep_sys_tracks = (Bool)0;

extern int mux(char *output_name,
        char *input_name1,
        char *input_name2) {

    GF_ISOFile *output_file;
    GF_Err error;
    Double force_fps = 20;
    u32 import_flags = 0;
    u32 frames_per_sample = 0;
    Double interleaving_time = 0.0;

//    u32 level = GF_LOG_DEBUG;
//
//    gf_log_set_tool_level(GF_LOG_CONTAINER, level);
//    gf_log_set_tool_level(GF_LOG_SCENE, level);
//    gf_log_set_tool_level(GF_LOG_PARSER, level);
//    gf_log_set_tool_level(GF_LOG_AUTHOR, level);
//    gf_log_set_tool_level(GF_LOG_CODING, level);

    output_file = gf_isom_open(output_name, GF_ISOM_WRITE_EDIT, NULL);
    if (!output_file) {
        return 1;
    }

    error = import_file(output_file, input_name1, import_flags, force_fps, frames_per_sample);
    if (error) {
        return 2;
    }

    error = import_file(output_file, input_name2, import_flags, force_fps, frames_per_sample);
    if (error) {
        return 3;
    }

    error = gf_isom_make_interleave(output_file, interleaving_time);
    if (error) {
        return 4;
    }

    error = gf_isom_set_storage_mode(output_file, GF_ISOM_STORE_DRIFT_INTERLEAVED);
    if (error) {
        return 5;
    }

    gf_isom_set_final_name(output_file, output_name);
    error = gf_isom_close(output_file);
    if (error) {
        return 6;
    }

    return 0;
}