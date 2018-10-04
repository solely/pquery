# pquery
一个获取HTML dom信息的PHP扩展

### 安装

`phpize`

`./configure`

`make && make install`

### 使用
`目前，测试了 PHP7.1  PHP7.2，PHP5还未作测试，请谨慎使用。`

#### 相关API
1、 void __construct( string $html);

`在实例化 pquery 时自动调用， $html 可为空;`

2、 array getTitle( void);

`获取 HTML 的 title标签 及其标签内文本内容;`

`参数为空;`

`返回数组，有两个键，一个键名为 count ： 匹配的次数, int 类型，这个数字不一定等于 data 数组元素和;`

`另一个键名为 data : 其值类型是数组，为匹配的结果;`

3、 array matchByTag( string $tag[, int $flag [, string $other_str]]);

`匹配闭合 HTML 标签;`

`参数： $tag : HTML 闭合标签`

` $flag : 三个常量之一;`

`        PQUERY_FLAG_NOT_MATCH_CONTENT : 不匹配标签内容`

`        PQUERY_FLAG_MATCH_CONTENT : 匹配标签内容`

`        PQUERY_FLAG_MATCH_HREF_SRC : 匹配标签内的 href 或 src 属性值`

` $other_str : 选填，根据自己的需要可以传入标志性字符串，或者正则表达式`

` 返回数组，结构同上。`

4、 array matchByA([ int $flag [, string $other_str]]);

`匹配 HTML A 标签;`

`参数，返回结果 同上;`

5、 array getMetaByName( string $name);

`获取 HTML meta标签;`


6、 object find( string $str);

7、 void setHtml( string $html);

8、 string getHtml( void);

9、 string getTextContent( void);

10、 object getNode( void);

11、 string getAttribute( string $str);

12、 object getItem( int $index);

13、 int select( string $str);

#### demo实例
TODO ： 文档未完待续......