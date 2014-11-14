//
//  ViewController.m
//  VGTS2MP4Example
//
//  Created by Petro Akzhygitov on 23/10/14.
//  Copyright (c) 2014 VG. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import "ViewController.h"
#import "VGTS2MP4.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];

    UIView *newView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 320, 440)];
    newView.backgroundColor = [UIColor yellowColor];

    NSString *inputFile = [[[NSBundle bundleForClass:[self class]] resourcePath] stringByAppendingPathComponent:@"testdata/video.ts"];
    NSLog(@"%@", inputFile);
    NSString *outputFile = [[[NSBundle bundleForClass:[self class]] resourcePath] stringByAppendingPathComponent:@"testdata/video.mp4"];
//    AVPlayerItem *playerItem = [AVPlayerItem playerItemWithURL:[NSURL fileURLWithPath:inputFile]];
//    AVPlayer *avPlayer = [AVPlayer playerWithPlayerItem:playerItem];
//    AVPlayerLayer *avPlayerLayer = [AVPlayerLayer playerLayerWithPlayer:avPlayer];
//
//    avPlayerLayer.frame = self.view.frame;
//    [newView.layer addSublayer:avPlayerLayer];
//    [self.view addSubview:newView];
//    [avPlayer play];

    VGTS2MP4 *vgts2mp4 = [[VGTS2MP4 alloc] init];
    [vgts2mp4 exportTSFile:inputFile toMP4File:outputFile withCompletionBlock:^(NSError *error) {
        if (error) {
            NSLog(@"Error occured: %@", error);

        } else {
            NSLog(@"MP4 file successfully exported: %@", outputFile);
        }
    }];
}

@end
