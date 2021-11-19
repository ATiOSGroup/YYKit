//
//  NSObject+YYDatabase.h
//  RouteOC
//
//  Created by 李阳 on 2020/8/28.
//  Copyright © 2020 appscomm. All rights reserved.
//


#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

FOUNDATION_EXTERN void DBSetErrorLogEnable(BOOL enable);

typedef NSString * const ColumnConstraints;
typedef NSString * const ColumnName;
typedef NSString * const TableName;
typedef ColumnName NewColumnName;
typedef ColumnName OldColumnName;


@interface ColumnConstraintWorker : NSObject

- (ColumnConstraintWorker * (^)(void))notnull;

- (ColumnConstraintWorker * (^)(void))primaryKey;
/// 只能用在 integer 字段上
- (ColumnConstraintWorker * (^)(void))autoincrement;
- (ColumnConstraintWorker * (^)(void))unique;
// default保留字
- (ColumnConstraintWorker * (^)(id value))default;
/// 被参照的键 column 必须有唯一约束或是主键
- (ColumnConstraintWorker * (^)(TableName tableName, ColumnName column))foreignRef;

/*
 如果是检索有大量重复数据的字段，不适合建立索引，反而会导致检索速度变慢，因为扫描索引节点的速度比全表扫描要慢
 */
/// 创建索引
- (ColumnConstraintWorker * (^)(void))uniqueIndex;
- (ColumnConstraintWorker * (^)(void))index;

@end

@interface ColumnConstraintMaker : NSObject

- (ColumnConstraintWorker * (^)(ColumnName name))column;

@end

@protocol YYDataBase <NSObject>
 
@optional
/// 自增长的字段在插入模型时会自动忽略该字段模型的值，由数据库自动设值
/// 经测试，sqlite autoincrement 约束只能用在主键上，并且只能有一个
/// 如果没有指定主键，默认会生成一个 no 的自动增长主键
+ (void)db_makeColumnConstraints:(ColumnConstraintMaker *)maker;
 
/// 此字段 不会插入到数据库，也不会参与模型转换
+ (NSArray<ColumnName> *)db_blackPropertyNamesList;
/// 此字段 不会插入到数据库，但可以参与模型转换，可以从其他表查询字段重命名为此字段名，模型转换时就有值了
+ (NSArray<ColumnName> *)db_ignorePropertyNames;

/// 一般可以返回userID，具有相同的identifer的表会在同一个数据库文件中
/// 没实现此方法的表都会存在 common.sqlite 文件中
+ (NSString *)db_identifier;
+ (NSString *)db_filePathWithSuggestDirectory:(NSString *)directory;

/// 默认从0.0.1开始，想要升级数据库，要递增此返回值
+ (NSString *)db_NewVersion;
/// 若想更改字段名，并实现此方法并递增版本号, 可通过参数实现跨版本迁移，不同versoin返回不同的结果
+ (NSDictionary<NewColumnName, NSString *> *)db_newColumnNameFromOldColumnNamesRepresentWithVersion:(NSString *)dbVersion;
@end

/// sql语句类型
typedef NS_ENUM(NSInteger, SqlStatementType) {
    DMLUpdate,
    DMLDelete,
    
    DQLSelect,
    DQLSelectCount,
};

/*
 https://www.w3school.com.cn/sql/sql_having.asp
 example
 orders表
 1    2008/12/29    1000    Bush
 2    2008/11/23    1600    Carter
 3    2008/10/05    700    Bush
 4    2008/09/28    300    Bush
 5    2008/08/06    2000    Adams
 6    2008/07/21    100    Carter
 
 SELECT Customer,SUM(OrderPrice) FROM Orders
 WHERE Customer='Bush' OR Customer='Adams'
 GROUP BY Customer
 HAVING SUM(OrderPrice)>1500
 
 NSArray *res = [Order db_selectWithSqlMaker:^(SqlMaker * _Nonnull maker) {
 maker.selectColumns(@"customer, date, o_id, sum(price) as price").where(@"customer = 'Bush' or customer = 'Adams'").groupBy(@"customer").having(@"sum(price) > 1500");
 NSLog(@"%@", maker.statement);
 }];
 NSLog(@"%@", res);
 
 结果
 Customer    SUM(OrderPrice)
 Bush        2000
 Adams       2000
 */

/*
 insert into persons (lastname, address) values ('wilson', 'champs-elysees');
 
 update person set address = 'zhongshan 23', city = 'nanjing' where lastname = 'wilson';
 
 delete from t_student where age <= 10 or age > 30;
 
 select * from t_student order by age asc, height desc;
 select address, age, money, name, no, score, t_insertTimestamp as insertTimestamp, t_insertTime as insertTime from t_GStudent;
 
 SELECT ... FROM ... WHERE ... GROUP BY ... HAVING ... ORDER BY ...
 
 2.查询本周数据

 select * from 表名 where 字段名 between datetime(date(datetime('now',strftime('-%w day','now'))),' 1 second')
 and datetime(date(datetime('now',(6 - strftime('%w day','now'))||' day','1 day')),'-1 second')
 3.查询本月的数据

 select * from 表名 where 字段名 between datetime('now','start of month',' 1 second') and
 datetime('now','start of month',' 1 month','-1 second')
 4.查询最近7的值（从当前起向前推6天，包括今天）

 select * from 表 名 where 时间字段 + between date('now','start of day','-6day') and date('now')
 5.查询最近一年的数据（从这个月起向前推12个月）

 select * from 表 名 where 时间字段 between date('now','start of month','-12 month and date('now')
 
 SELECT SUBSTR(name, 0, instr(name, '_')) as firstName, SUBSTR(name, instr(name, '_') + 1) as lastName from t_HStudent
 
 
 同样的SQL语句，查不出数据来
 select * from table1 where t1>='2017-6-1' and t1<='2017-6-5'
 改成
 select * from table1 where t1>='2017-06-01' and t1<='2017-06-05'
 这样就可以查出数据来，注意格式
 
 
 sqlite3中的类型转换  CAST(column as int) 
 */

@class SqlMaker;

/// 内部会根据不同的sql操作自动调整所支持的子句顺序，以达到最优执行效率
@interface SqlMaker : NSObject

- (SqlMaker * (^)(NSString *keyValues, ...))set;
 
/// 要查询的字段 distinct
- (SqlMaker * (^)(ColumnName columnNames, ...))selectCount;
- (SqlMaker * (^)(ColumnName columnNames, ...))select;

- (SqlMaker * (^)(TableName tableName, ...))from;

- (SqlMaker * (^)(NSString *condition, ...))where;

- (SqlMaker * (^)(ColumnName columnName, ...))groupBy;
- (SqlMaker * (^)(NSString *condition, ...))having;
- (SqlMaker * (^)(NSString *sortedColums, ...))orderyBy;
- (SqlMaker * (^)(NSUInteger location, NSUInteger length))limit;

/// select 语句时要转换的模型，默认和查询的表一样
- (SqlMaker * (^)(Class modelCls))toModel;

- (SqlMaker * (^)(NSString *clause, ...))join;
- (SqlMaker * (^)(NSString *clause, ...))leftJoin;
- (SqlMaker * (^)(NSString *condition, ...))on;
 
- (SqlMaker * (^)(NSString *clause, ...))db_union;
- (SqlMaker * (^)(NSString *clause, ...))unionAll;

/// 调用这个方法或者 NSLog打印此对象;，可以查看最终生成的sql语句是否正确，
- (NSString *)statement;

// NS_FORMAT_FUNCTION(1, 2) 会告诉编译器，索引1处的参数是一个格式化字符串，而实际参数从索引2处开始。
/// 自定义完整的sql
- (void)execArbitrarySQL:(NSString *)format, ... NS_FORMAT_FUNCTION(1,2);;

- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)initWithClass:(Class)tableCls type:(SqlStatementType)type NS_DESIGNATED_INITIALIZER;
+ (instancetype)makerWithClass:(Class)tableCls type:(SqlStatementType)type;

@end

/// 执行sqlite函数，sqlite有些日期时间处理函数还是很好用的
FOUNDATION_EXTERN id db_exec(NSString *format, ...);

/// 一下方法可以用在 where 语句中，快速判断日期
typedef NSString *const DBCondition;
FOUNDATION_EXTERN DBCondition db_year_is(const char *column, int year);
FOUNDATION_EXTERN DBCondition db_month_is(const char *column, int month);
FOUNDATION_EXTERN DBCondition db_day_is(const char *column, int day);

FOUNDATION_EXTERN ColumnName DBColumnInsertTime;
FOUNDATION_EXTERN ColumnName DBColumnInsertTimestamp;
/// 默认主键字段
FOUNDATION_EXTERN ColumnName DBColumnDefaultPK;
FOUNDATION_EXTERN TableName  DBDefaultTableName;

/*
 数据库设计说明，
 内部会为每个表增加 c_insertTimestamp(插入时间戳) 和 c_insertTime(插入时间) 字段，用以调试
 c_insertTimestamp 和 c_insertTime 作为保留字段，
 表名为 t_类名
 比如Person类，表名为 t_Person
 不支持联合主键，联合主键在升级迁移数据库表会很麻烦，
 */
@interface NSObject (YYDataBase)

+ (NSString *)db_tableName;
+ (NSString *)db_filePath;
+ (NSString *)db_version;
+ (BOOL)db_close;
 

+ (BOOL)db_updateTableIfNecessary;

- (BOOL)db_insert;
- (BOOL)db_insertOrReplace;
 
- (BOOL)db_update;
/// 只更新 columns字段 多个字段用,隔开
- (BOOL)db_updateColumnsInString:(NSString *)columns;
/// 更新 除exclude外的字段 多个字段用,隔开
- (BOOL)db_updateExcludeColumnInString:(NSString *)exclude;
- (BOOL)db_updateWithSqlMaker:(void (^)(SqlMaker *maker))maker;
/// nil 不会执行任何语句
+ (BOOL)db_updateWithSqlMaker:(void (^)(SqlMaker *maker))maker;
/// update sqlite_sequence set seq = 0 where name = 't_WOrder';
+ (BOOL)db_updateSeqFromSequenceTable:(NSInteger)value;

- (BOOL)db_delete;
+ (BOOL)db_deleteWithPrimaryValue:(id)primaryValue;
+ (BOOL)db_deleteWithPrimaryValueInArray:(NSArray *)array;
/// nil 表示删除表中所有记录
+ (BOOL)db_deleteWithSqlMaker:(nullable void (^)(SqlMaker *maker))maker;

// MARK: Select
/// 前提主键有值才能查询，并更新属性
- (instancetype)db_select;
/// 从数据库中查询主键值为 value 的记录并以模型返回
+ (instancetype)db_modelWithPrimaryValue:(id)value;

/// 空代表查询所有字段
+ (NSArray *)db_selectWithSqlMaker:(nullable void (^)(SqlMaker *maker))maker;

/// 以json字符串的方式返回查询结果(包含插入时间信息)，一般可用于调试
+ (NSString *)db_selectJSONWithSqlMaker:(nullable void (^)(SqlMaker *maker))maker;
 

+ (NSUInteger)db_selectCountWithSqlMaker:(nullable void (^)(SqlMaker *maker))maker;

/// 删除表
+ (BOOL)db_dropTable;
+ (BOOL)db_dropIndexTable;

// 添加自定函数
+ (BOOL)db_makeFunctionNamed:(const char *)name
                    argument:(int)count
                        work:(id _Nullable (^)(NSArray *params, NSString *_Nullable __autoreleasing * _Nullable querySQLAsResult, NSString *_Nullable __autoreleasing* _Nonnull error))work;
+ (BOOL)db_makeCollationNamed:(const char *)name
                         work:(NSComparisonResult (^)(NSString *lhs, NSString *rhs))work;

+ (BOOL)db_execute:(NSString *)sql;
+ (NSArray<NSDictionary *> *)db_query:(NSString *)sql;

/// 在当前线程同步执行block，线程安全
+ (void)db_threadSafe:(void(^)(void))block;
+ (nullable id)db_threadSafeReturned:(id _Nullable (^)(void))block;

/// 在一条子线程异步执行block，主线程执行finish
+ (void)db_work:(nullable id _Nullable (^)(void))work finish:(nullable void(^)(id _Nullable obj))finish; 

/// 检验创建表的语句是否正确
+ (NSString *)db_createTableSql;

+ (NSString *)db_lastErrorMessage;
@end

@interface NSArray (YYDataBase)

/// 为了效率，使用以下方法前请确保数组中实例都是同一种类型
- (BOOL)db_deletes;
- (BOOL)db_updates;
- (BOOL)db_inserts;
- (BOOL)db_insertOrReplaces;

@end


@interface NSObject (DBColumns)

+ (NSString *)dbColumns;
+ (NSString *)dbColumnsExcludeInString:(nullable NSString *)exclude;
+ (NSString *)dbColumnsExcludeInString:(nullable NSString *)exclude
                            tableAlias:(nullable NSString *)aliasName;

+ (NSString *)dbColumnsInString:(nullable NSString *)exclude
                     tableAlias:(nullable NSString *)aliasName;

@end

NS_ASSUME_NONNULL_END


