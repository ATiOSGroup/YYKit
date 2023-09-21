//
//  YYDatabase.h
//  YYKit <https://github.com/ibireme/YYKit>
//
//  Created by ibireme on 15/5/9.
//  Copyright (c) 2015 ibireme.
//
//  This source code is licensed under the MIT-style license found in the
//  LICENSE file in the root directory of this source tree.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

#if DEBUG
static BOOL DBErrorLogsEnabled = YES;
#else
static BOOL DBErrorLogsEnabled = NO;
#endif

typedef NS_ENUM(int, SqliteValueType) {
    SqliteValueTypeInteger = 1,
    SqliteValueTypeFloat   = 2,
    SqliteValueTypeText    = 3,
    SqliteValueTypeBlob    = 4,
    SqliteValueTypeNull    = 5
};

@interface SqlFuncParam : NSObject

@property (nonatomic, assign, readonly) SqliteValueType type;
@property (nonatomic, strong, readonly) id value;

- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;
@end

@interface YYDatabase : NSObject

- (nullable NSArray<NSDictionary<NSString *, id> *>*)query:(NSString *)sql;

- (BOOL)executesNoRollback:(NSArray<NSString *> *)sqls;

- (BOOL)executes:(NSArray<NSString *> *)sqls;

- (BOOL)execute:(NSString *)sql;

// [self query:[NSString stringWithFormat:@"select %@ as result;", sql]]
// return result
- (id)select:(NSString *)sql;

- (BOOL)close;
- (BOOL)open;

- (BOOL)makeCollationNamed:(const char *)name
                      work:(NSComparisonResult (^)(NSString *lhs, NSString *rhs))work;
- (BOOL)makeFunctionNamed:(const char *)name
                 argument:(int)count
                     work:(id _Nullable (^)(YYDatabase *db, NSArray<SqlFuncParam *> *params, NSString *_Nullable __autoreleasing* _Nonnull error))work;

- (NSString *)lastErrorMessage;

- (int64_t)lastInsertRowId;

- (instancetype)initWithPath:(NSString *)path NS_DESIGNATED_INITIALIZER;

@property (nonatomic, strong, class, readonly) YYDatabase *memory;
@property (nonatomic, strong, class, readonly) YYDatabase *temporary; 

- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;
@end

NS_ASSUME_NONNULL_END
