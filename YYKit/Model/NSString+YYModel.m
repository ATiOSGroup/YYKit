//
//  NSString+YYModel.m
//  RouteOC
//
//  Created by 李阳 on 2020/8/24.
//  Copyright © 2020 appscomm. All rights reserved.
//

#import "NSString+YYModel.h"
 

@interface NSString (__YYAdd)
/**
 *  驼峰转下划线（loveYou -> love_you）
 */
- (NSString *)yy_underlineFromCamel;
/**
 *  下划线转驼峰（love_you -> loveYou）
 */
- (NSString *)yy_camelFromUnderline;
/**
 * 首字母变大写
 */
- (NSString *)yy_firstCharUpper;
/**
 * 首字母变小写
 */
- (NSString *)yy_firstCharLower;
@end
@implementation NSString (__YYAdd)
- (NSString *)yy_underlineFromCamel {
    if (self.length == 0) return self;
    NSMutableString *string = [NSMutableString string];
    for (NSUInteger i = 0, max = self.length; i < max; i++) {
        unichar c = [self characterAtIndex:i];
        NSString *cString = [NSString stringWithFormat:@"%c", c];
        NSString *cStringLower = [cString lowercaseString];
        if ([cString isEqualToString:cStringLower]) {
            [string appendString:cStringLower];
        } else {
            [string appendString:@"_"];
            [string appendString:cStringLower];
        }
    }
    return string;
}

- (NSString *)yy_camelFromUnderline {
    if (self.length == 0) return self;
    NSMutableString *string = [NSMutableString string];
    NSArray *cmps = [self componentsSeparatedByString:@"_"];
    for (NSUInteger i = 0; i<cmps.count; i++) {
        NSString *cmp = cmps[i];
        if (i && cmp.length) {
            [string appendString:[NSString stringWithFormat:@"%c", [cmp characterAtIndex:0]].uppercaseString];
            if (cmp.length >= 2) [string appendString:[cmp substringFromIndex:1]];
        } else {
            [string appendString:cmp];
        }
    }
    return string;
}

- (NSString *)yy_firstCharLower {
    if (self.length == 0) return self;
    NSMutableString *string = [NSMutableString string];
    [string appendString:[NSString stringWithFormat:@"%c", [self characterAtIndex:0]].lowercaseString];
    if (self.length >= 2) [string appendString:[self substringFromIndex:1]];
    return string;
}

- (NSString *)yy_firstCharUpper {
    if (self.length == 0) return self;
    NSMutableString *string = [NSMutableString string];
    [string appendString:[NSString stringWithFormat:@"%c", [self characterAtIndex:0]].uppercaseString];
    if (self.length >= 2) [string appendString:[self substringFromIndex:1]];
    return string;
}
@end

@implementation NSString (YYModel)

- (NSString *)mapperWithType:(YYStringMapperType)type {
    switch (type) {
        case YYStringMapperDefault:
            return self;
        case YYStringMapperFirstCharLower:
            return self.yy_firstCharLower;
        case YYStringMapperFirstCharUpper:
            return self.yy_firstCharUpper;
        case YYStringMapperUnderLineFromCamel:
            return self.yy_underlineFromCamel;
        case YYStringMapperCamelFromUnderLine:
            return self.yy_camelFromUnderline;
        default: return self;
    }
}

- (NSComparisonResult)compareVersion:(NSString *)version {
    if (!version.length) return 1;
    NSCharacterSet *set = [[NSCharacterSet decimalDigitCharacterSet] invertedSet];
    
    NSArray *nums1 = [self componentsSeparatedByCharactersInSet:set];
    NSArray *nums2 = [version componentsSeparatedByCharactersInSet:set];
    
    NSInteger relation = nums1.count - nums2.count;
    for (NSInteger i = 0, max = relation < 0 ? nums1.count : nums2.count; i < max; i++) {
        NSInteger res = [nums1[i] integerValue] - [nums2[i] integerValue];
        if (res != 0) return res > 0 ? 1 : -1;
    }
    if (relation == 0) return 0;
    return relation < 0 ? -1 : 1;
}

@end
