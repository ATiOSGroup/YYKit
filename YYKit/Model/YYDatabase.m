//
//  YYDatabase.m
//  YYKit <https://github.com/ibireme/YYKit>
//
//  Created by ibireme on 15/5/9.
//  Copyright (c) 2015 ibireme.
//
//  This source code is licensed under the MIT-style license found in the
//  LICENSE file in the root directory of this source tree.
//

#import "YYDatabase.h"

#if __has_include(<sqlite3.h>)
#import <sqlite3.h>
#else
#import "sqlite3.h"
#endif

@implementation SqlFuncParam

- (instancetype)initWithTyep:(SqliteValueType)type value:(id)value {
    self = [super init];
    _type = type;
    _value = value;
    return self;
}

@end

@interface _SqlFuncBox : NSObject
@end
@implementation _SqlFuncBox {
    @package
    __unsafe_unretained YYDatabase *_db;
    int _argumentsCount;
    id _Nullable (^_block)(YYDatabase *db, NSArray<SqlFuncParam *> *params, NSString **error);
}
@end

@interface _SqlCollationBox : NSObject
@end
@implementation _SqlCollationBox {
    @package
    NSComparisonResult (^_block)(NSString *lhs, NSString *rhs);
}
@end
 

void SQLiteCallBackFunction(sqlite3_context *context, int argc, sqlite3_value **argv) {
    _SqlFuncBox *pApp = (__bridge _SqlFuncBox *)(sqlite3_user_data(context));
    int count = argc;
    NSMutableArray *params = [NSMutableArray arrayWithCapacity:count];
    for (int i = 0; i < count; i++) {
        id val = nil;
        SqliteValueType type = sqlite3_value_type(argv[i]);
        switch (type) {
            case SqliteValueTypeNull: {
                
            } break;
            case SqliteValueTypeText: {
                const char *cString = (const char *)sqlite3_value_text(argv[i]);
                if (cString) val = [NSString stringWithUTF8String:cString];
            } break;
            case SqliteValueTypeInteger: {
                val = [NSNumber numberWithLongLong:sqlite3_value_int64(argv[i])];
            } break;
            case SqliteValueTypeFloat: {
                val = [NSNumber numberWithDouble:sqlite3_value_double(argv[i])];
            } break;
            default: break;
        }
        if (val == nil) {
            type = SqliteValueTypeNull;
            val = [NSNull null];
        }
        [params addObject:[[SqlFuncParam alloc] initWithTyep:type value:val]];
    }
    
    NSString *error = nil;
    id res = pApp->_block(pApp->_db, params, &error);
    if (error) {
        sqlite3_result_error(context, [error UTF8String], -1);
        return;
    } else if (!res || res == [NSNull null]) {
        sqlite3_result_null(context);
    } else {
        if ([res isKindOfClass:[NSNumber class]]) {
            NSNumber *num = (NSNumber *)res;
            if (strcmp([num objCType], @encode(float)) == 0 ||
                strcmp([num objCType], @encode(double)) == 0) {
                sqlite3_result_double(context, [num doubleValue]);
            } else {
                sqlite3_result_int64(context, [num longLongValue]);
            }
        } else if ([res isKindOfClass:[NSString class]]) {
            sqlite3_result_text(context, [res UTF8String], -1, SQLITE_TRANSIENT);
        }
    }
}

@implementation YYDatabase {
    @package
    NSString *_dbPath;
    sqlite3 *_db;
    NSMutableSet *_functions;
    NSMutableSet *_collations;
}

+ (YYDatabase *)memory {
    static YYDatabase *_db;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _db = [[YYDatabase alloc] initWithPath:@":memory:"];
    });
    return _db;
}
+ (YYDatabase *)temporary {
    static YYDatabase *_db;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _db = [[YYDatabase alloc] initWithPath:@""];
    });
    return _db;
}

- (instancetype)initWithPath:(NSString *)path {
    self = [super init];
    _dbPath = [path copy];
    
    return self;
}
- (BOOL)makeFunctionNamed:(const char *)name
                 argument:(int)count
                     work:(id _Nullable (^)(YYDatabase * _Nonnull, NSArray<SqlFuncParam *> * _Nonnull, NSString * _Nullable __autoreleasing * _Nonnull))work {
    if (!work) return NO;
    _SqlFuncBox *box = [_SqlFuncBox new];
    box->_db = self;
    box->_argumentsCount = count;
    box->_block = [work copy];
    
    if (!_functions) _functions = [NSMutableSet set];
    [_functions addObject:box];
    return sqlite3_create_function(_db, name, count, SQLITE_UTF8, (__bridge void *)(box), &SQLiteCallBackFunction, NULL, NULL) == SQLITE_OK;
}
int SQLiteCallBackCollation(void *pApp, int lLen, const void *lData, int rLen, const void *rData) {
    return (int)((__bridge NSComparisonResult (^)(NSString *__strong, NSString *__strong))(pApp))([NSString stringWithCString:lData length:lLen], [NSString stringWithCString:rData length:rLen]);
}
 
- (BOOL)makeCollationNamed:(const char *)name
                       work:(NSComparisonResult (^)(NSString *lhs, NSString *rhs))work {
    if (!work) return NO;
    
    if (!_collations) _collations = [NSMutableSet set];
    id block = [work copy];
    [_collations addObject:block];
    
    return sqlite3_create_collation_v2(_db, name, SQLITE_UTF8, (__bridge void *)(block), &SQLiteCallBackCollation, NULL) == SQLITE_OK;
}

- (BOOL)open {
    if (_db) return YES;
    
    int result = sqlite3_open(_dbPath.UTF8String, &_db);
    if (result == SQLITE_OK) {
        BOOL success = [self _addUnixepochFunction];
        NSLog(@"");
        return YES;
    }
    else {
        _db = NULL;
        if (DBErrorLogsEnabled) {
            NSLog(@"sqlite file '%@' open failed (%d).", _dbPath, result);
        }
        return NO;
    }
}

- (BOOL)close {
    if (!_db) return YES;
    
    int  result = 0;
    BOOL retry = NO;
    BOOL stmtFinalized = NO;
     
    do {
        retry = NO;
        result = sqlite3_close(_db);
        if (result == SQLITE_BUSY || result == SQLITE_LOCKED) {
            if (!stmtFinalized) {
                stmtFinalized = YES;
                sqlite3_stmt *stmt;
                while ((stmt = sqlite3_next_stmt(_db, nil)) != 0) {
                    sqlite3_finalize(stmt);
                    retry = YES;
                }
            }
        } else if (result != SQLITE_OK) {
            if (DBErrorLogsEnabled) {
                NSLog(@"sqlite file '%@' close failed (%d).", _dbPath, result);
            }
        }
    } while (retry);
    _db = NULL;
    return YES;
}
- (id)select:(NSString *)sql {
    NSArray<NSDictionary *> *rows = [self query:[NSString stringWithFormat:@"select %@ as result;", sql]];
    if (!rows.count) {
        if (DBErrorLogsEnabled) NSLog(@"%s", sqlite3_errmsg(_db));
        return nil;
    }
    return rows[0][@"result"];
}

- (BOOL)execute:(NSString *)sql {
    if (sql.length == 0) return YES;
    if (![self open]) return NO;
    
    return [self __executeUTF8:sql.UTF8String];
}
- (BOOL)executes:(NSArray<NSString *> *)sqls {
    if (!sqls.count) return YES;
    if (![self open]) return NO;
    
    // 开启事务
    if (![self __executeUTF8:"begin transaction"]) return NO;
    for (NSString *sql in sqls) {
        if (![self __executeUTF8:sql.UTF8String]) {
            // 回滚
            [self __executeUTF8:"rollback transaction"];
            return NO;
        }
    }
    
    return [self __executeUTF8:"commit transaction"];
}

- (BOOL)executesNoRollback:(NSArray<NSString *> *)sqls {
    if (!sqls.count) return YES;
    if (![self open]) return NO;
     
    for (NSString *sql in sqls) {
        [self __executeUTF8:sql.UTF8String];
    }
    return YES;
}
- (NSArray<NSDictionary<NSString *, id> *> *)query:(NSString *)sql {
    if (!sql.length) return nil;
    if (![self open]) return nil;
    
    sqlite3_stmt *stmt = nil;
    int result = sqlite3_prepare_v2(_db, sql.UTF8String, -1, &stmt, 0);
    if (result != SQLITE_OK) {
        if (DBErrorLogsEnabled) NSLog(@"sqlite file '%@' sqlite3_prepare_v2 error (%d): '%s'", _dbPath, result, sqlite3_errmsg(_db));
        return nil;
    }
    
    NSMutableArray *rows = [NSMutableArray array];
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_count(stmt);
        NSMutableDictionary *row = [NSMutableDictionary dictionary];
        [rows addObject:row];
        for (int i = 0; i < count; i++) {
            NSString *name = [NSString stringWithUTF8String:sqlite3_column_name(stmt, i)];
            id value = nil;
            switch (sqlite3_column_type(stmt, i)) {
                case SQLITE_INTEGER:
                    value = @(sqlite3_column_int(stmt, i));
                    break;
                case SQLITE_FLOAT:
                    value = @(sqlite3_column_double(stmt, i));
                    break;
                case SQLITE_BLOB:
                    value = [NSData dataWithBytes:sqlite3_column_blob(stmt, i)
                                           length:sqlite3_column_bytes(stmt, i)];
                    break;
                case SQLITE_NULL:
                    value = nil;
                    break;
                case SQLITE3_TEXT:
                    value = [NSString stringWithUTF8String:(const char *)sqlite3_column_text(stmt, i)];
                    break;
            }
            row[name] = value;
        }
    }
    
    sqlite3_finalize(stmt);
    return rows;
}

- (NSString *)lastErrorMessage {
    return [NSString stringWithUTF8String:sqlite3_errmsg(_db)];
}

// MARK: Private
- (BOOL)__executeUTF8:(const char *)sql {
    if (strlen(sql) == 0) return YES;
    
    char *error = NULL;
    int result = sqlite3_exec(_db, sql, NULL, NULL, &error);
    if (error) {
        if (DBErrorLogsEnabled) NSLog(@"sqlite file '%@' exec '%s' error (%d): '%s'", _dbPath, sql, result, error);
        sqlite3_free(error);
    }
    
    return result == SQLITE_OK;
}
 
- (BOOL)_addUnixepochFunction {
    return [self makeFunctionNamed:"unixepoch"
                   argument:-1
                        work:^id (YYDatabase *db, NSArray<SqlFuncParam *> *params, NSString **error) {
        NSInteger n = params.count;
        if (n < 2) {
            *error = @"param invalid";
            return nil;
        }
        SqlFuncParam *param0 = params[0];
        SqlFuncParam *param1 = params[1];
        if ((param0.type != SqliteValueTypeInteger &&
             param0.type != SqliteValueTypeFloat) ||
            param1.type != SqliteValueTypeText) {
            *error = @"param invalid";
            return nil;
        }
        NSString *sql = nil;
        id ts = param0.value;
        NSString *modifier = [param1.value lowercaseString];
        
        if ([modifier isEqualToString:@"start of year"]) {
            sql = [NSString stringWithFormat:@"select strftime('%%s', %@, 'unixepoch', 'localtime', 'start of year', 'utc') as result", ts];
        } else if ([modifier isEqualToString:@"end of year"]) {
            sql = [NSString stringWithFormat:@"select strftime('%%s', %@, 'unixepoch', 'localtime', 'start of year', '+1 year', '-1 second', 'utc') as result", ts];
        } else if ([modifier isEqualToString:@"start of month"]) {
            sql = [NSString stringWithFormat:@"select strftime('%%s', %@, 'unixepoch', 'localtime', 'start of month', 'utc') as result", ts];
        } else if ([modifier isEqualToString:@"end of month"]) {
            sql = [NSString stringWithFormat:@"select strftime('%%s', %@, 'unixepoch', 'localtime', 'start of month', '+1 month', '-1 second', 'utc') as result", ts];
        } else if ([modifier isEqualToString:@"start of day"]) {
            sql = [NSString stringWithFormat:@"select strftime('%%s', %@, 'unixepoch', 'localtime', 'start of day', 'utc') as result", ts];
        } else if ([modifier isEqualToString:@"end of day"]) {
            sql = [NSString stringWithFormat:@"select strftime('%%s', %@, 'unixepoch', 'localtime', 'start of day', '+1 day', '-1 second', 'utc') as result", ts];
        } else if ([modifier containsString:@"week"]) {
            NSString *delta = @"";
            if (n > 2) {
                SqlFuncParam *param2 = params[2];
                if (param2.type == SqliteValueTypeInteger) {
                    int val2 = [param2.value intValue];
                    if (val2 > 0) {
                        delta = [NSString stringWithFormat:@" '+%d day',", val2];
                    } else if (val2 < 0) {
                        delta = [NSString stringWithFormat:@" '-%d day',", abs(val2)];
                    }
                }
            }
            if ([modifier isEqualToString:@"start of week"]) {
                sql = [NSString stringWithFormat:@"select strftime('%%s', %@, 'unixepoch', 'localtime', strftime('-%%w day', %@, 'unixepoch'), 'start of day',%@ 'utc') as result", ts, ts, delta];
            } else if ([modifier isEqualToString:@"end of week"]) {
                sql = [NSString stringWithFormat:@"select strftime('%%s', %@, 'unixepoch', 'localtime', strftime('-%%w day', %@, 'unixepoch'), 'start of day',%@ '+7 day', '-1 second', 'utc') as result", ts, ts, delta];
            } else {
                *error = @"param invalid";
                return nil;
            }
        } else if ([modifier containsString:@"hour"]) {
            NSArray<NSDictionary *> *rows = [db query:[NSString stringWithFormat:@"select strftime('%%H', %@, 'unixepoch', 'localtime') as result", ts]];
            if (!rows.count) return nil;
            int hour = [rows[0][@"result"] intValue];
            
            int step = 1;
            if (n > 2) {
                SqlFuncParam *param2 = params[2];
                if (param2.type == SqliteValueTypeInteger) {
                    int val2 = [param2.value intValue];
                    if (val2 <= 0 || val2 > 23) {
                        *error = @"param invalid";
                        return nil;
                    }
                    step = val2;
                }
            }
            NSString *delta = @"";
            if ([modifier isEqualToString:@"start of hour"]) {
                hour = hour / step * step;
                delta = [NSString stringWithFormat:@"'+%d hour'", hour];
            } else if ([modifier isEqualToString:@"end of hour"]) {
                hour = (hour / step + 1) * step;
                delta = [NSString stringWithFormat:@"'+%d hour'", hour];
            } else if ([modifier isEqualToString:@"middle of hour"])  {
                hour = (hour / step) * step;
                delta = [NSString stringWithFormat:@"'+%d hour', '+%d minute'", hour, step * 30];
            } else {
                *error = @"param invalid";
                return nil;
            }
            sql = [NSString stringWithFormat:@"select strftime('%%s', %@, 'unixepoch', 'localtime', 'start of day', %@, 'utc') as result", ts, delta];
        } else if ([modifier containsString:@"minute"]) {
            if (n < 3) {
                *error = @"param invalid";
                return nil;
            }
            int step = 0;
            SqlFuncParam *param2 = params[2];
            if (param2.type == SqliteValueTypeInteger) {
                int val2 = [param2.value intValue];
                if (val2 <= 0) {
                    *error = @"param invalid";
                    return nil;
                }
                step = val2;
            } else {
                *error = @"param invalid";
                return nil;
            }
            
            NSArray<NSDictionary *> *rows = [db query:[NSString stringWithFormat:@"select strftime('%%H:%%M', %@, 'unixepoch', 'localtime') as result", ts]];
            if (!rows.count) return nil;
            NSArray *cmps = [rows[0][@"result"] componentsSeparatedByString:@":"];
            int min = [cmps[0] intValue] * 60 + [cmps[1] intValue];
            
            NSString *delta = @"";
            if ([modifier isEqualToString:@"start of minute"]) {
                min = min / step * step;
                delta = [NSString stringWithFormat:@"'+%d minute'", min];
            } else if ([modifier isEqualToString:@"end of minute"]) {
                min = (min / step + 1) * step;
                delta = [NSString stringWithFormat:@"'+%d minute'", min];
            } else {
                *error = @"param invalid";
                return nil;
            }
            sql = [NSString stringWithFormat:@"select strftime('%%s', %@, 'unixepoch', 'localtime', 'start of day', %@, 'utc') as result", ts, delta];
        } else {
            *error = @"param invalid";
            return nil;
        }
        
        NSArray<NSDictionary *> *rows = [db query:sql];
        if (!rows.count) return nil;
        return rows[0][@"result"];
    }];
}

- (void)dealloc {
    [self close];
}

@end
