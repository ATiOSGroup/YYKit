//
//  UIApplication+YYAdd.h
//  YYKit <https://github.com/ibireme/YYKit>
//
//  Created by ibireme on 13/4/4.
//  Copyright (c) 2015 ibireme.
//
//  This source code is licensed under the MIT-style license found in the
//  LICENSE file in the root directory of this source tree.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

/**
 Provides extensions for `UIApplication`.
 */
@interface UIApplication (YYAdd)
 

/// Whether this app is pirated (not install from appstore).
@property (nonatomic, readonly) BOOL isPirated;

/// Whether this app is being debugged (debugger attached).
@property (nonatomic, readonly) BOOL isBeingDebugged;

/// Current thread real memory used in byte. (-1 when error occurs)
@property (nonatomic, readonly) int64_t memoryUsage;

/// Current thread CPU usage, 1.0 means 100%. (-1 when error occurs)
@property (nonatomic, readonly) float cpuUsage;


/**
 Increments the number of active network requests.
 If this number was zero before incrementing, this will start animating the 
 status bar network activity indicator.
 
 This method is thread safe.
 
 This method has no effect in App Extension.
 */
- (void)incrementNetworkActivityCount;

/**
 Decrements the number of active network requests. 
 If this number becomes zero after decrementing, this will stop animating the 
 status bar network activity indicator.
 
 This method is thread safe.
 
 This method has no effect in App Extension.
 */
- (void)decrementNetworkActivityCount;


/// Returns YES in App Extension.
+ (BOOL)isAppExtension;

/// Same as sharedApplication, but returns nil in App Extension.
+ (nullable UIApplication *)sharedExtensionApplication;

@end

NS_ASSUME_NONNULL_END
