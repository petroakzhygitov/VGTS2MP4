cmake_minimum_required(VERSION 2.8.4)
project(ts2es)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    ac3.c
    ac3_fns.h
    accessunit.c
    accessunit_defns.h
    accessunit_fns.h
    adts.c
    adts_defns.h
    adts_fns.h
    audio.c
    audio_defns.h
    audio_fns.h
    avs.c
    avs_defns.h
    avs_fns.h
    bitdata.c
    bitdata_defns.h
    bitdata_fns.h
    compat.h
    es.c
    es_defns.h
    es_fns.h
    h222.c
    h222_defns.h
    h222_fns.h
    h262.c
    h262_defns.h
    h262_fns.h
    l2audio.c
    l2audio_fns.h
    misc.c
    misc_defns.h
    misc_fns.h
    nalunit.c
    nalunit_defns.h
    nalunit_fns.h
    pes.c
    pes_defns.h
    pes_fns.h
    pidint.c
    pidint_defns.h
    pidint_fns.h
    printing.c
    printing_defns.h
    printing_fns.h
    ps.c
    ps_defns.h
    ps_fns.h
    reverse.c
    reverse_defns.h
    reverse_fns.h
    ts.c
    ts_defns.h
    ts_fns.h
    tswrite.c
    video_defns.h
    tsdemux.c
    tsdemux.h
    tsdemux_test.c)

add_executable(ts2es ${SOURCE_FILES})