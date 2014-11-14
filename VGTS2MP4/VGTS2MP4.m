//
//  VGTS2MP4.m
//  VGTS2MP4
//
//  Created by Petro Akzhygitov on 23/10/14.
//  Copyright (c) 2014 VG. All rights reserved.
//

#import "VGTS2MP4.h"
#import "tsdemux.h"
#import "mp4mux.h"
#import "VGTS2MP4Error.h"

@implementation VGTS2MP4

- (void)exportTSFile:(NSString *)tsFile toMP4File:(NSString *)mp4File withCompletionBlock:(void(^)(NSError *error))completionBlock {
    int e;
    NSError *error;
    NSUInteger errorCode;

    if (![[NSFileManager defaultManager] fileExistsAtPath:tsFile]) {
        errorCode = VGTS2MP4ErrorTSFileDoesNotExists;

        if (completionBlock) {
            completionBlock([[NSError alloc] initWithDomain:VGTS2MP4ErrorDomain code:errorCode userInfo:@{ NSLocalizedDescriptionKey : VGTS2MP4_ERROR_LOCALIZED_DESCRIPTION(errorCode) }]);
        }

        return;
    }

    NSString *fileName = [[tsFile lastPathComponent] stringByDeletingPathExtension];
    NSString *videoFilePath = [NSString stringWithFormat:@"%@/%@.264", [tsFile stringByDeletingLastPathComponent], fileName];
    NSString *audioFilePath = [NSString stringWithFormat:@"%@/%@.aac", [tsFile stringByDeletingLastPathComponent], fileName];
    
    e = demux((char *)[tsFile UTF8String], (char *)[videoFilePath UTF8String], (char *)[audioFilePath UTF8String]);
    if (e) {
        switch (e) {
            case 1:
                errorCode = VGTS2MP4ErrorDemuxerUnableToOpenInputFile;
                break;

            case 2:
            case 3:
                errorCode = VGTS2MP4ErrorDemuxerUnableToOpenOutputFile;
                break;

            case 4:
                errorCode = VGTS2MP4ErrorDemuxerUnableToExtractData;
                break;

            case 5:
                errorCode = VGTS2MP4ErrorDemuxerUnableToCloseInputFile;
                break;

            case 6:
            case 7:
                errorCode = VGTS2MP4ErrorDemuxerUnableToCloseOutputFile;
                break;

            default:
                errorCode = VGTS2MP4ErrorDemuxer;
        }

        [[NSFileManager defaultManager] removeItemAtPath:videoFilePath error:nil];
        [[NSFileManager defaultManager] removeItemAtPath:audioFilePath error:nil];

        if (completionBlock) {
            completionBlock([[NSError alloc] initWithDomain:VGTS2MP4ErrorDomain code:errorCode userInfo:@{ NSLocalizedDescriptionKey : VGTS2MP4_ERROR_LOCALIZED_DESCRIPTION(errorCode) }]);
        }

        return;
    }

    e = mux((char *)[mp4File UTF8String], (char *)[videoFilePath UTF8String], (char *)[audioFilePath UTF8String]);
    if (e) {
        switch (e) {
            case 1:
                errorCode = VGTS2MP4ErrorMuxerUnableToOpenOutputFile;
                break;

            case 2:
            case 3:
                errorCode = VGTS2MP4ErrorMuxerUnableToImportFile;
                break;

            case 4:
                errorCode = VGTS2MP4ErrorMuxerUnableToMakeInterleave;
                break;

            case 5:
                errorCode = VGTS2MP4ErrorMuxerUnableToSetStorageMode;
                break;

            case 6:
                errorCode = VGTS2MP4ErrorMuxerUnableToCloseOutputFile;
                break;

            default:
                errorCode = VGTS2MP4ErrorMuxer;
        }


        [[NSFileManager defaultManager] removeItemAtPath:videoFilePath error:nil];
        [[NSFileManager defaultManager] removeItemAtPath:audioFilePath error:nil];

        if (completionBlock) {
            completionBlock([[NSError alloc] initWithDomain:VGTS2MP4ErrorDomain code:errorCode userInfo:@{NSLocalizedDescriptionKey : VGTS2MP4_ERROR_LOCALIZED_DESCRIPTION(errorCode)}]);
        }

        return;
    }


    [[NSFileManager defaultManager] removeItemAtPath:videoFilePath error:&error];
    [[NSFileManager defaultManager] removeItemAtPath:audioFilePath error:&error];

    if (completionBlock) {
        completionBlock(error);
    }
}

@end
