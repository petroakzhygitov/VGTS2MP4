//
//  VGTS2MP4.h
//  VGTS2MP4
//
//  Created by Petro Akzhygitov on 23/10/14.
//  Copyright (c) 2014 VG. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface VGTS2MP4 : NSObject

- (void)exportTSFile:(NSString *)tsFile toMP4File:(NSString *)mp4File withCompletionBlock:(void(^)(NSError *error))completionBlock;

@end
