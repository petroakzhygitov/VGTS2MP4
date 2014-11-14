//
// Created by Petro Akzhygitov on 14/11/14.
// Copyright (c) 2014 VG. All rights reserved.
//

#import <Foundation/Foundation.h>

#pragma mark macros

#define VGTS2MP4_ERROR_KEY(code)                    [NSString stringWithFormat:@"%d", code]
#define VGTS2MP4_ERROR_LOCALIZED_DESCRIPTION(code)  NSLocalizedStringFromTable(VGTS2MP4_ERROR_KEY(code), @"VGTS2MP4Error", nil)

#pragma mark const

#pragma mark enum

FOUNDATION_EXPORT NSString * const VGTS2MP4ErrorDomain;

NS_ENUM(NSUInteger, VGTS2MP4Error) {
    VGTS2MP4ErrorTSFileDoesNotExists = 1000,
    VGTS2MP4ErrorDemuxer,
    VGTS2MP4ErrorDemuxerUnableToOpenInputFile,
    VGTS2MP4ErrorDemuxerUnableToOpenOutputFile,
    VGTS2MP4ErrorDemuxerUnableToExtractData,
    VGTS2MP4ErrorDemuxerUnableToCloseInputFile,
    VGTS2MP4ErrorDemuxerUnableToCloseOutputFile,
    VGTS2MP4ErrorMuxer,
    VGTS2MP4ErrorMuxerUnableToOpenOutputFile,
    VGTS2MP4ErrorMuxerUnableToImportFile,
    VGTS2MP4ErrorMuxerUnableToMakeInterleave,
    VGTS2MP4ErrorMuxerUnableToSetStorageMode,
    VGTS2MP4ErrorMuxerUnableToCloseOutputFile
};
