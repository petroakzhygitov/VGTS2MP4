//
//  VGTS2MP4ExampleTests.m
//  VGTS2MP4ExampleTests
//
//  Created by Petro Akzhygitov on 23/10/14.
//  Copyright (c) 2014 VG. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <XCTest/XCTest.h>
#import "VGTS2MP4.h"#include "setup.h"

#define VERBOSE 1

@interface VGTS2MP4ExampleTests : XCTestCase

@end

@implementation VGTS2MP4ExampleTests {
    dispatch_semaphore_t _semaphore;
}

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.

    _semaphore = dispatch_semaphore_create(0);
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testVGTS2MP4 {
    // This is an example of a functional test case.
#if DEBUG
    NSLog(@"debug");
#endif

    NSString *inputFile = [[[NSBundle bundleForClass:[self class]] resourcePath] stringByAppendingPathComponent:@"testdata/all12.ts"];
    NSString *outputFile = [[[NSBundle bundleForClass:[self class]] resourcePath] stringByAppendingPathComponent:@"testdata/all12.mp4"];

    VGTS2MP4 *ts2mp4 = [[VGTS2MP4 alloc] init];
    [ts2mp4 exportTSFile:inputFile toMP4File:outputFile withCompletionBlock:^(NSError *error) {
        NSLog(@"Exported file: %@", outputFile);
        dispatch_semaphore_signal(_semaphore);
    }];

    while (dispatch_semaphore_wait(_semaphore, DISPATCH_TIME_NOW)) {
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode
                                 beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.1]];
    }
}

//- (void)testTS2MP4 {
//    NSURL *tsFileURL = [NSURL URLWithString:[[[NSBundle bundleForClass:[self class]] resourcePath] stringByAppendingPathComponent:@"testdata/8004.ts"]];
//    KMMediaAsset *tsAsset = [KMMediaAsset assetWithURL:tsFileURL withFormat:KMMediaFormatTS];
//
//    NSURL *mp4FileURL = [NSURL URLWithString:[[[NSBundle bundleForClass:[self class]] resourcePath] stringByAppendingPathComponent:@"testdata/8004.mp4"]];
//    KMMediaAsset *mp4Asset = [KMMediaAsset assetWithURL:mp4FileURL withFormat:KMMediaFormatMP4];
//
//    KMMediaAssetExportSession *tsToMP4ExportSession = [[KMMediaAssetExportSession alloc] initWithInputAssets:@[tsAsset]];
//    tsToMP4ExportSession.outputAssets = @[mp4Asset];
//
//    [tsToMP4ExportSession exportAsynchronouslyWithCompletionHandler:^{
//        if (tsToMP4ExportSession.status == KMMediaAssetExportSessionStatusCompleted) {
//            NSLog(@"Export completed");
//
//        } else {
//            NSLog(@"Export failed");
//        }
//    }];
//}


- (void)testPerformanceExample {
    // This is an example of a performance test case.
    [self measureBlock:^{
        // Put the code you want to measure the time of here.
    }];
}

@end
