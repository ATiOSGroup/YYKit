//
//  NSObject+YYDatabase.h
//  RouteOC
//
//  Created by 李阳 on 2020/8/28.
//  Copyright © 2020 appscomm. All rights reserved.
//


#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN
 
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

SELECT DATE(1638158275, 'unixepoch', 'localtime', 'start of year')
2021-01-01

select datetime(1638158275, 'unixepoch', 'localtime', strftime('-%w day','now'))
2021-11-28 11:57:55

select datetime(1638158275, 'unixepoch', 'localtime', strftime('-%w day','now'), 'start of day')
2021-11-28 00:00:00


select datetime(1635717253, 'unixepoch', 'localtime', strftime('-%w day',1635717253, 'unixepoch', 'localtime'), 'start of day')
2021-10-31 00:00:00

select date(1638158275, 'unixepoch', 'localtime', 'start of month')
2021-11-01

select strftime('%Y-%m-%d %H:%M:%S', 1638158275, 'unixepoch', 'localtime')
2021-11-29 11:57:55

select strftime('%Y-%m-%d %H:%M:%S', 1638158275, 'unixepoch', 'localtime', 'start of day')
2021-11-29 00:00:00

----------------------------------------
create table if not exists t_weight(no integer primary key autoincrement, timestamp integer unique, value real, insertTimestamp text not null default (strftime('%s','now')), insertTime text not null default (datetime('now','localtime')));
// 按天分组
select avg(value) as avgValue, strftime('%s', timestamp, 'unixepoch', 'localtime', 'start of day', 'utc') as ts from t_weight group by ts order by ts;
select avg(value) as avgValue, strftime('%s', timestamp, 'unixepoch', 'localtime', 'start of day', 'utc') as ts from t_weight group by ts having ts >= '1637856000' order by ts;
// 按周分组
select avg(value) as avgValue, strftime('%s', timestamp, 'unixepoch', 'localtime', strftime('-%w day', timestamp, 'unixepoch', 'utc'), 'start of day', 'utc') as ts from t_weight group by ts order by ts;
// 按月分组
select avg(value) as avgValue, strftime('%s', timestamp, 'unixepoch', 'localtime', 'start of month', 'utc') as ts from t_weight group by ts order by ts;
*/


/**
内置了 unixepoch(parm1, parm2, parm3) 时间戳处理函数
param1为时间戳字段或者 整型或浮点型 数值
param2为修饰符，字符串类型，不区分大小写，支持
['start of hour', 'end of hour',
'start of day', 'end of day',
'start of week', 'end of week',
'start of month', 'end of month',
'start of year', 'end of year']

param3参数
当parm2为'start of week'或者'end of week'时，此参数用来指定周首日，不传/0表示周日首日，1表示周一首日，2表示周二首日，-1表示上周五首日，依次类推
当parm2为'start of hour'或者'end of hour'时，此参数用来指定倍数，比如
当前时间09:32

        param3:    3      2
param2：
'start of hour'    09:00  08:00
'end of hour'      12:00  10:00

返回的是修饰符对应的unix时间戳字符串

用于分组查询
select avg(value) as avgValue, unixepoch(timestamp, 'start of day') as ts from t_heart_rate group by ts order by ts

select avg(value) as avgValue, unixepoch(timestamp, 'start of week') as ts from t_weight where timestamp >= 1628956800 group by ts having ts >= '1628956800' order by ts;
*/
/// 执行sqlite函数，sqlite有些日期时间处理函数还是很好用的
 
typedef NSString * const ColumnName;
typedef NSString * const TableName;
typedef ColumnName NewColumnName;
typedef ColumnName OldColumnName;

///
typedef NS_ENUM(NSInteger, ForeignKeyAction) {
    //:默认的,表示没有什么行为.
    nothing = 0,
    //:当有一个child关联到parent时,禁止delete或update parent
    prohibit,
    /// :当parent被delete或update时,child的的关联字段被置为null(如果字段有not null,就出错)
    setNULL,
    /// :类似于SET NULL (是不是设置默认值?没有试过)
    setDefault,
    /**
     :将实施在parent上的删除或更新操作,传播给你吧与之关联的child上.
     对于 ON DELETE CASCADE, 同被删除的父表中的行 相关联的子表中的每1行,也会被删除.
     对于ON UPDATE CASCADE, 存储在子表中的每1行, 对应的字段的值会被自动修改成同新的父键匹配
     */
    cascade
};

@interface ColumnConstraintWorker : NSObject

/// 再约束的最后调用这个可以消除在swift中返回结果未使用的警告
@property (nonatomic, copy, readonly) void (^end)(void);

@property (nonatomic, copy, readonly) ColumnConstraintWorker * (^notnull)(void);

// 主键字段默认包含 not null 和 unique 两个约束
@property (nonatomic, copy, readonly) ColumnConstraintWorker * (^primaryKey)(void);
/// 只能用在 integer 字段上
@property (nonatomic, copy, readonly) ColumnConstraintWorker * (^autoincrement)(void);
@property (nonatomic, copy, readonly) ColumnConstraintWorker * (^unique)(void);
/*
 https://www.runoob.com/sqlite/sqlite-index.html
 如果是检索有大量重复数据的字段，不适合建立索引，反而会导致检索速度变慢，因为扫描索引节点的速度比全表扫描要慢
 */
/// 创建索引
@property (nonatomic, copy, readonly) ColumnConstraintWorker * (^uniqueIndex)(void);
@property (nonatomic, copy, readonly) ColumnConstraintWorker * (^index)(void);

@property (nonatomic, copy, readonly) ColumnConstraintWorker *(^defaulte)(id value);
/// 被参照的键 column 必须有唯一约束或是主键
@property (nonatomic, copy, readonly) ColumnConstraintWorker * (^foreignRef)(TableName tableName, ColumnName column, ForeignKeyAction onDelete, ForeignKeyAction onUpdate);


@end

@interface ColumnConstraintMaker : NSObject
 
@property (nonatomic, copy, readonly) ColumnConstraintWorker *(^column)(ColumnName name);

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
+ (NSString *)db_filePathWithSuggestDirectory:(NSString *)directory;

/// 默认从0.0.1开始，想要升级数据库，需增加版本号
+ (NSString *)db_NewVersion;

/// 若想更改字段名，需实现此方法且增加版本号, 可通过参数实现跨版本迁移，不同versoin返回不同的结果
+ (NSDictionary<NewColumnName, OldColumnName> *)db_newColumnNameFromOldColumnNamesVersioned:(NSString *)dbVersion;
@end
 

/// 以下方法可以用在 where 语句中，快速判断日期
typedef NSString *const DBCondition;
FOUNDATION_EXTERN DBCondition db_year_is(const char *column, int year);
FOUNDATION_EXTERN DBCondition db_month_is(const char *column, int month);
FOUNDATION_EXTERN DBCondition db_day_is(const char *column, int day);

FOUNDATION_EXTERN ColumnName DBColumnInsertTime;
FOUNDATION_EXTERN ColumnName DBColumnInsertTimestamp;
/// 默认主键字段
FOUNDATION_EXTERN ColumnName DBColumnDefaultPK;
FOUNDATION_EXTERN TableName  DBDefaultTableName;

@class YYDatabase;
/*
 数据库设计说明，
 内部会为每个表增加 c_insertTimestamp(插入时间戳) 和 c_insertTime(插入时间) 字段，用以调试
 c_insertTimestamp 和 c_insertTime 作为保留字段，
 表名默认为 t_类名
 比如Person类，表名为 t_Person
 不支持联合主键，联合主键在升级迁移数据库表会很麻烦，
 */
@interface NSObject (YYDataBase)
/// 可自定义表名
+ (NSString *)db_tableName;
/// 数据库路径
+ (NSString *)db_filePath;
+ (NSString *)db_version;

+ (BOOL)db_close;

+ (BOOL)db_updateTableIfNecessary;

- (BOOL)db_insert;
- (BOOL)db_insertOrReplace;
 
// 使用下面四个方法，必须指定主键，应为内部是根据主键更新的
- (BOOL)db_update;
/// 只更新 columns字段 多个字段用,隔开
- (BOOL)db_updateColumnsInString:(NSString *)columns;
/// 更新 除exclude外的字段 多个字段用,隔开
- (BOOL)db_updateExcludeColumnInString:(NSString *)exclude;

- (BOOL)db_delete;
+ (BOOL)db_deleteWithPrimaryValue:(id)primaryValue;
+ (BOOL)db_deleteWithPrimaryValueInArray:(NSArray *)array;

// MARK: Select
/// 前提主键有值才能查询，并更新属性
- (instancetype)db_select;
/// 从数据库中查询主键值为 value 的记录并以模型返回
+ (instancetype)db_modelWithPrimaryValue:(id)value;
 
/// 删除表
+ (BOOL)db_dropTable;
+ (BOOL)db_dropIndexTable;
 

+ (BOOL)db_execute:(NSString *)sql;
+ (nullable NSArray<NSDictionary<NSString *, id> *> *)db_query:(NSString *)sql;
 

/// 创表语句, 
+ (NSString *)db_createTableSql;

+ (NSString *)db_lastErrorMessage;

@property (nonatomic, strong, class, readonly) YYDatabase *db_handle;
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


