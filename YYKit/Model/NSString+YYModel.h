//
//  NSString+YYModel.h
//  RouteOC
//
//  Created by 李阳 on 2020/8/24.
//  Copyright © 2020 appscomm. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, YYStringMapperType) {
    YYStringMapperDefault = 0,// 不做转换
    YYStringMapperFirstCharLower = 1,// 首字母变小写
    YYStringMapperFirstCharUpper, // 首字母变大写
    YYStringMapperUnderLineFromCamel, // 驼峰转下划线（loveYou -> love_you）
    YYStringMapperCamelFromUnderLine, // 下划线转驼峰（love_you -> loveYou）
};

@interface NSString (YYModel)

- (NSString *)mapperWithType:(YYStringMapperType)type;
- (NSComparisonResult)compareVersion:(NSString *)version;

@end

NS_ASSUME_NONNULL_END
