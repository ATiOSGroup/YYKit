//
//  NSObject+YYModel.h
//  YYKit <https://github.com/ibireme/YYKit>
//
//  Created by ibireme on 15/5/10.
//  Copyright (c) 2015 ibireme.
//
//  This source code is licensed under the MIT-style license found in the
//  LICENSE file in the root directory of this source tree.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 Provide some data-model method:
 
 * Convert json to any object, or convert any object to json.
 * Set object properties with a key-value dictionary (like KVC).
 * Implementations of `NSCoding`, `NSCopying`, `-hash` and `-isEqual:`.
 
 See `YYModel` protocol for custom methods.
 
 
 Sample Code:
    
     interface YYAuthor : NSObject
     @property (nonatomic, strong) NSString *name;
     @property (nonatomic, assign) NSDate *birthday;
     @end
     @implementation YYAuthor
     @end
 
     interface YYBook : NSObject
     @property (nonatomic, copy) NSString *name;
     @property (nonatomic, assign) NSUInteger pages;
     @property (nonatomic, strong) YYAuthor *author;
     @end
     @implementation YYBook
     @end
    
     int main() {
         // create model from json
         YYBook *book = [YYBook modelWithJSON:@"{\"name\": \"Harry Potter\", \"pages\": 256, \"author\": {\"name\": \"J.K.Rowling\", \"birthday\": \"1965-07-31\" }}"];
 
         // convert model to json
         NSString *json = [book modelToJSONString];
         // {"author":{"name":"J.K.Rowling","birthday":"1965-07-31T00:00:00+0000"},"name":"Harry Potter","pages":256}
     }
 
     ********************** Coding/Copying/hash/equal *********************
     interface YYShadow :NSObject <NSCoding, NSCopying>
     @property (nonatomic, copy) NSString *name;
     @property (nonatomic, assign) CGSize size;
     @end
 
     @implementation YYShadow
     - (void)encodeWithCoder:(NSCoder *)aCoder { [self modelEncodeWithCoder:aCoder]; }
     - (id)initWithCoder:(NSCoder *)aDecoder { self = [super init]; return [self modelInitWithCoder:aDecoder]; }
     - (id)copyWithZone:(NSZone *)zone { return [self modelCopy]; }
     - (NSUInteger)hash { return [self modelHash]; }
     - (BOOL)isEqual:(id)object { return [self modelIsEqual:object]; }
     @end
 
 */

// MARK: - Model
@interface NSObject (YYModel)

/**
 Creates and returns a new instance of the receiver from a json.
 This method is thread-safe.
 
 @param json  A json object in `NSDictionary`, `NSString` or `NSData`.
 
 @return A new instance created from the json, or nil if an error occurs.
 */
+ (nullable instancetype)modelWithJSON:(id)json;

/**
 Creates and returns a new instance of the receiver from a key-value dictionary.
 This method is thread-safe.
 
 @param dictionary  A key-value dictionary mapped to the instance's properties.
 Any invalid key-value pair in dictionary will be ignored.
 
 @return A new instance created from the dictionary, or nil if an error occurs.
 
 @discussion The key in `dictionary` will mapped to the reciever's property name,
 and the value will set to the property. If the value's type does not match the
 property, this method will try to convert the value based on these rules:
 
     `NSString` or `NSNumber` -> c number, such as BOOL, int, long, float, NSUInteger...
     `NSString` -> NSDate, parsed with format "yyyy-MM-dd'T'HH:mm:ssZ", "yyyy-MM-dd HH:mm:ss" or "yyyy-MM-dd".
     `NSString` -> NSURL.
     `NSValue` -> struct or union, such as CGRect, CGSize, ...
     `NSString` -> SEL, Class.
 */
+ (nullable instancetype)modelWithDictionary:(NSDictionary *)dictionary;

/**
 Set the receiver's properties with a json object.
 
 @discussion Any invalid data in json will be ignored.
 
 @param json  A json object of `NSDictionary`, `NSString` or `NSData`, mapped to the
 receiver's properties.
 
 @return Whether succeed.
 */
- (BOOL)setPropertiesValuesWithJSON:(id)json;

/**
 Set the receiver's properties with a key-value dictionary.
 
 @param dic  A key-value dictionary mapped to the receiver's properties.
 Any invalid key-value pair in dictionary will be ignored.
 
 @discussion The key in `dictionary` will mapped to the reciever's property name,
 and the value will set to the property. If the value's type doesn't match the
 property, this method will try to convert the value based on these rules:
 
     `NSString`, `NSNumber` -> c number, such as BOOL, int, long, float, NSUInteger...
     `NSString` -> NSDate, parsed with format "yyyy-MM-dd'T'HH:mm:ssZ", "yyyy-MM-dd HH:mm:ss" or "yyyy-MM-dd".
     `NSString` -> NSURL.
     `NSValue` -> struct or union, such as CGRect, CGSize, ...
     `NSString` -> SEL, Class.
 
 @return Whether succeed.
 */
- (BOOL)setPropertiesWithDictionary:(NSDictionary *)dic;

/**
 Generate a json object from the receiver's properties.
 
 @return A json object in `NSDictionary` or `NSArray`, or nil if an error occurs.
 See [NSJSONSerialization isValidJSONObject] for more information.
 
 @discussion Any of the invalid property is ignored.
 If the reciver is `NSArray`, `NSDictionary` or `NSSet`, it just convert
 the inner object to json object.
 */
- (nullable id)modelToJSONObject;

/**
 Generate a json string's data from the receiver's properties.
 
 @return A json string's data, or nil if an error occurs.
 
 @discussion Any of the invalid property is ignored.
 If the reciver is `NSArray`, `NSDictionary` or `NSSet`, it will also convert the
 inner object to json string.
 */
- (nullable NSData *)modelToJSONData;

/**
 Generate a json string from the receiver's properties.
 
 @return A json string, or nil if an error occurs.
 
 @discussion Any of the invalid property is ignored.
 If the reciver is `NSArray`, `NSDictionary` or `NSSet`, it will also convert the
 inner object to json string.
 */
- (nullable NSString *)modelToJSONString;

/**
 Copy a instance with the receiver's properties.
 
 @return A copied instance, or nil if an error occurs.
 */
- (nullable id)modelCopy;



/**
 Get a hash code with the receiver's properties.
 
 @return Hash code.
 */
- (NSUInteger)modelHash;

/**
 Compares the receiver with another object for equality, based on properties.
 
 @param model  Another object.
 
 @return `YES` if the reciever is equal to the object, otherwise `NO`.
 */
- (BOOL)modelIsEqual:(id)model;

/**
 Description method for debugging purposes based on properties.
 
 @return A string that describes the contents of the receiver.
 */
- (NSString *)modelDescription;


+ (nullable NSArray *)modelArrayWithKeyValues:(id)json;

/**
 *  通过文件(plist or json)来创建一个模型
 *  @param filename 文件名(仅限于mainBundle中的文件)
 *  @return 新建的对象
 */
+ (instancetype)modelWithFilename:(NSString *)filename;

/**
 *  通过文件(plist or json)来创建一个模型
 *  @param path 文件全路径
 *  @return 新建的对象
 */
+ (instancetype)modelWithFilePath:(NSString *)path;

/**
 通过文件(plist or json)来创建一个模型数组
 @param filename 文件名(仅限于mainBundle中的文件)
 @return 模型数组
 */
+ (NSArray *)modelArrayWithFilename:(NSString *)filename;

/**
 *  通过文件(plist or json)来创建一个模型数组
 *  @param path 文件全路径
 *  @return 模型数组
 */
+ (NSArray *)modelArrayWithFilePath:(NSString *)path;


- (BOOL)outputToFilePath:(NSString *)path;
@end 


FOUNDATION_EXTERN NSDictionary *JSONFromFile(NSString *filename);
FOUNDATION_EXTERN NSArray *JSONArrayFromFile(NSString *filename);

FOUNDATION_EXTERN NSDictionary *JSONFromFilePath(NSString *path);
FOUNDATION_EXTERN NSArray *JSONArrayFromFilePath(NSString *path);

/**
 If the default model transform does not fit to your model class, implement one or
 more method in this protocol to change the default key-value transform process.
 There's no need to add '<YYModel>' to your class header.
 */
@protocol YYModel <NSObject>
@optional

/**
 这个数组中的属性名将会被忽略：不进行字典和模型的转换
 */
+ (nullable NSArray<NSString *> *)ignoredModelPropertyNames;
/**
 Custom property mapper.
 
 @discussion If the key in JSON/Dictionary does not match to the model's property name,
 implements this method and returns the additional mapper.
 
 Example:
    
    json:
        {
            "n":"Harry Pottery",
            "p": 256,
            "ext" : {
                "desc" : "A book written by J.K.Rowling."
            },
            "ID" : 100010
        }
 
    model:
        interface YYBook : NSObject
        @property NSString *name;
        @property NSInteger page;
        @property NSString *desc;
        @property NSString *bookID;
        @end
        
        implementation YYBook
        + (NSDictionary *)replaceKeyFromPropertyName {
            return @{@"name"  : @"n",
                     @"page"  : @"p",
                     @"desc"  : @"ext.desc",
                     @"bookID": @[@"id", @"ID", @"book_id"]};
        }
        @end
 
 @return A custom mapper for properties.
 */
+ (nullable NSDictionary<NSString *, id> *)replaceKeyFromPropertyName;

/// 对模型属性做统一转换 一对一
+ (NSString *)replaceKeyFromPropertyName121:(NSString *)name;

/**
 The generic class mapper for container properties.
 
 @discussion If the property is a container object, such as NSArray/NSSet/NSDictionary,
 implements this method and returns a property->class mapper, tells which kind of
 object will be add to the array/set/dictionary.
 
  Example:
        class YYShadow, YYBorder, YYAttachment;
 
        interface YYAttributes
        @property NSString *name;
        @property NSArray *shadows;
        @property NSSet *borders;
        @property NSDictionary *attachments;
        @end
 
        @implementation YYAttributes
        + (NSDictionary *)modelCustomClassInArray {
            return @{@"shadows" : [YYShadow class],
                     @"borders" : YYBorder.class,
                     @"attachments" : @"YYAttachment" };
        }
        @end
 
 @return A class mapper.
 */
+ (nullable NSDictionary<NSString *, id> *)modelCustomClassInArray;

/**
 If you need to create instances of different classes during json->object transform,
 use the method to choose custom class based on dictionary data.
 
 @discussion If the model implements this method, it will be called to determine resulting class
 during `+modelWithJSON:`, `+modelWithDictionary:`, conveting object of properties of parent objects
 (both singular and containers via `+modelCustomClassInArray`).
 
 Example:
        class YYCircle, YYRectangle, YYLine;
 
        @implementation YYShape

        + (Class)modelCustomClassForDictionary:(NSDictionary*)dictionary {
            if (dictionary[@"radius"] != nil) {
                return [YYCircle class];
            } else if (dictionary[@"width"] != nil) {
                return [YYRectangle class];
            } else if (dictionary[@"y2"] != nil) {
                return [YYLine class];
            } else {
                return [self class];
            }
        }

        @end

 @param dictionary The json/kv dictionary.
 
 @return Class to create from this dictionary, `nil` to use current class.

 */
+ (nullable Class)modelCustomClassForDictionary:(NSDictionary *)dictionary;

@end 





#define YYCopyImplementation \
- (id)copyWithZone:(NSZone *)zone { return [self modelCopy]; }

#define YYHashImplementation \
- (NSUInteger)hash { return [self modelHash]; }

#define YYEqualImplementation \
- (BOOL)isEqual:(id)object { return [self modelIsEqual:object]; }

NS_ASSUME_NONNULL_END
