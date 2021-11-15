//
//  NSObject+YYCodable.h
//  RouteOC
//
//  Created by 李阳 on 2020/8/28.
//  Copyright © 2020 appscomm. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

// cd 为 coding 简称
@protocol YYCodeable <NSCoding>

@optional
/// 这个数组中的属性名将会被忽略：不进行解档归档
+ (NSArray<NSString *> *)cd_ignoredPropertyNames;
/// 只有这个数组中的属性名将才会进行解档归档
+ (NSArray<NSString *> *)cd_allowedPropertyNames;
+ (NSString *)cd_filePathWithSuggestDirectory:(NSString *)directory;

@end

@interface NSObject (YYCodable)

/**
 Encode the receiver's properties to a coder.
 
 @param aCoder  An archiver object.
 */
- (void)modelEncodeWithCoder:(NSCoder *)aCoder;

/**
 Decode the receiver's properties from a decoder.
 
 @param aDecoder  An archiver object.
 
 @return self
 */
- (id)modelInitWithCoder:(NSCoder *)aDecoder;

- (BOOL)cd_archive;
+ (instancetype)cd_unarchive;

+ (NSString *)cd_filePath;

+ (BOOL)cd_remove;
@end


#define YYCodingImplementation \
- (id)initWithCoder:(NSCoder *)decoder { \
    self = [self init]; \
    return [self modelInitWithCoder:decoder]; \
} \
- (void)encodeWithCoder:(NSCoder *)encoder { \
    [self modelEncodeWithCoder:encoder]; \
}

/*
 - (id)initWithCoder:(NSCoder *)decoder {
     return [self modelInitWithCoder:decoder];
 }
 - (void)encodeWithCoder:(NSCoder *)encoder {
     [self modelEncodeWithCoder:encoder];
 }
 */

NS_ASSUME_NONNULL_END
