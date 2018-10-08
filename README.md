# pquery
一个获取HTML dom信息的PHP扩展

### 安装

```bash
phpize
./configure
make && make install
echo 'pquery.so' > /path/php.ini
service php-fpm restart
```

### 使用
``` 
目前，测试了 PHP7.1  PHP7.2，PHP5还未作测试，请谨慎使用。
```

#### 相关API
1、 void __construct( string $html);

```php
在实例化 pquery 时自动调用， $html 可为空;
```

2、 array getTitle( void);

```php
获取 HTML 的 title标签 及其标签内文本内容;
参数为空;
返回数组，有两个键，一个键名为 count ： 匹配的次数, int 类型，这个数字不一定等于 data 数组元素和;
另一个键名为 data : 其值类型是数组，为匹配的结果;
```

3、 array matchByTag( string $tag[, int $flag [, string $other_str]]);

```php
匹配闭合 HTML 标签;
参数： $tag : HTML 闭合标签
$flag : 三个常量之一;
         PQUERY_FLAG_NMC : 不匹配标签内容，默认
         PQUERY_FLAG_MC : 匹配标签内容
         PQUERY_FLAG_MHS : 匹配标签内的 href 或 src 属性值
$other_str : 选填，根据自己的需要可以传入标志性字符串，或者正则表达式
返回数组，结构同上。
```

4、 array matchByA([ int $flag [, string $other_str]]);

```php
匹配 HTML A 标签;
参数，返回结果 同上;
```

5、 array getMetaByName( string $name);

```php
获取 HTML meta标签，根据 name属性值进行匹配;
参数： $name : meta标签中 name属性值
返回结果同上;
```

6、 object find( string $str);
```php
根据 HTML 属性及属性值 进行查找;
参数： $str : HTML的属性及属性值组成的字符串;
       比如： $str = 'id="id-name"';
返回 pquery 对象;
```

7、 void setHtml( string $html);
```php
给 pquery 对象设置查找的 HTML 字符串;
参数： $html : HTML字符串
```

8、 string getHtml( void);
```php
获取 HTML 字符串;
```

9、 string getTextContent( void);
```php
获取文本内容;
```

10、 object getNode( void);
```php
获取node对象;
```

11、 string getAttribute( string $str);
```php
获取属性值;
参数： $str ： 属性字符串
```

12、 object getItem( int $index);
```php
获取某一个item，当调用了 select() 之后，此方法返回 $index 处 item 的 pquery 对象，否则返回 NULL
```

13、 int select( string $str);
```php
同 find()，只是这个返回匹配的总个数;
```

#### demo实例
demo.html
```html
<html>
<head>
    <title>pquery demo</title>
    <meta name="description" content="这是 pquery demo页面" />
    <meta name="keywords" content="php扩展、pquery" />
</head>
<body id="body">
    <div class="wrap">
        <ul>
            <li> <a href="demo.com/1.html" class="a">1.html</a></li>
            <li> <a href="demo.com/2.html">2.html</a></li>
            <li> <a href="demo.com/3.html">3.html</a></li>
            <li> <a href="demo.com/4.html">4.html</a></li>
            <li> <a href="demo.com/5.html">5.html</a></li>
        </ul>
    </div>
    <div class="wrap">
        <video src="demo.com/1.mp4" loop preload="auto">您的浏览器不支持播放该视频</video>
    </div>
    <div class="wrap">
        <h1 class="h1-t">H1标题</h1>
        <h2 class="h2-t">H2标题</h2>
        <h3 class="h3-t">H3标题</h3>
        <h4 class="h4-t">H4标题</h4>
        <h5 class="h5-t">H5标题</h5>
        <h6 class="h6-t">H6标题</h6>
    </div>
    <div class="one">
        <p>这是 one</p>
        <div class="two">
            <p>这是 two</p>
            <div class="three">
                <p>这是 three</p>
            </div>
        </div>
    </div>
</body>
</html>
```
php代码
```php
<?php
    $html = file_get_contents("demo.html");
    $pquery = new pquery($html);
    // 或者
    // $pquery = new pquery();
    // $pquery->setHtml($html);
    
    //获取 title
    $title = $pquery->getTitle();
    var_dump($title);
    /*
     * 输出
        array(2) {
          ["count"]=>
          int(1)
          ["data"]=>
          array(2) {
            [0]=>
            string(26) "<title>pquery demo</title>"
            [1]=>
            string(11) "pquery demo"
          }
        }
    */

    //获取 a 标签
    $tags = $pquery->matchByTag("a");
    var_dump($tags);
    
    /*
     * 输出
        array(2) {
          ["count"]=>
          int(5)
          ["data"]=>
          array(1) {
            [0]=>
            array(5) {
              [0]=>
              string(46) "<a href="demo.com/1.html" class="a">1.html</a>"
              [1]=>
              string(36) "<a href="demo.com/2.html">2.html</a>"
              [2]=>
              string(36) "<a href="demo.com/3.html">3.html</a>"
              [3]=>
              string(36) "<a href="demo.com/4.html">4.html</a>"
              [4]=>
              string(36) "<a href="demo.com/5.html">5.html</a>"
            }
          }
        }
    */
    
    //获取 a 标签及其内容
    $tags2 = $pquery->matchByTag("a", PQUERY_FLAG_MC);
    var_dump($tags2);
    
    /*
     * 输出
        array(2) {
          ["count"]=>
          int(5)
          ["data"]=>
          array(2) {
            [0]=>
            array(5) {
              [0]=>
              string(46) "<a href="demo.com/1.html" class="a">1.html</a>"
              [1]=>
              string(36) "<a href="demo.com/2.html">2.html</a>"
              [2]=>
              string(36) "<a href="demo.com/3.html">3.html</a>"
              [3]=>
              string(36) "<a href="demo.com/4.html">4.html</a>"
              [4]=>
              string(36) "<a href="demo.com/5.html">5.html</a>"
            }
            [1]=>
            array(5) {
              [0]=>
              string(6) "1.html"
              [1]=>
              string(6) "2.html"
              [2]=>
              string(6) "3.html"
              [3]=>
              string(6) "4.html"
              [4]=>
              string(6) "5.html"
            }
          }
        }
    */

    //获取特定的 a 标签及其内容
    $tags3 = $pquery->matchByTag("a", PQUERY_FLAG_MC, 'class="a"');
    var_dump($tags3);
    
    /*
     * 输出
        array(2) {
          ["count"]=>
          int(1)
          ["data"]=>
          array(2) {
            [0]=>
            array(1) {
              [0]=>
              string(46) "<a href="demo.com/1.html" class="a">1.html</a>"
            }
            [1]=>
            array(1) {
              [0]=>
              string(6) "1.html"
            }
          }
        }
    */

    //获取 a 标签及其内容与src属性值
    $tags4 = $pquery->matchByTag("a", PQUERY_FLAG_MHS);
    var_dump($tags4);
    
    /*
     * 输出
        array(2) {
          ["count"]=>
          int(5)
          ["data"]=>
          array(4) {
            [0]=>
            array(5) {
              [0]=>
              string(46) "<a href="demo.com/1.html" class="a">1.html</a>"
              [1]=>
              string(36) "<a href="demo.com/2.html">2.html</a>"
              [2]=>
              string(36) "<a href="demo.com/3.html">3.html</a>"
              [3]=>
              string(36) "<a href="demo.com/4.html">4.html</a>"
              [4]=>
              string(36) "<a href="demo.com/5.html">5.html</a>"
            }
            [1]=>
            array(5) {
              [0]=>
              string(4) "href"
              [1]=>
              string(4) "href"
              [2]=>
              string(4) "href"
              [3]=>
              string(4) "href"
              [4]=>
              string(4) "href"
            }
            [2]=>
            array(5) {
              [0]=>
              string(15) "demo.com/1.html"
              [1]=>
              string(15) "demo.com/2.html"
              [2]=>
              string(15) "demo.com/3.html"
              [3]=>
              string(15) "demo.com/4.html"
              [4]=>
              string(15) "demo.com/5.html"
            }
            [3]=>
            array(5) {
              [0]=>
              string(6) "1.html"
              [1]=>
              string(6) "2.html"
              [2]=>
              string(6) "3.html"
              [3]=>
              string(6) "4.html"
              [4]=>
              string(6) "5.html"
            }
          }
        }
    */
    
    //获取 meta 标签
    $des = $pquery->getMetaByName("description");
    var_dump($des);
    
    /*
     * 输出
        array(2) {
          ["count"]=>
          int(1)
          ["data"]=>
          array(2) {
            [0]=>
            string(62) "<meta name="description" content="这是 pquery demo页面" />"
            [1]=>
            string(24) "这是 pquery demo页面"
          }
        }
    */
    
    //使用 find() 查找元素
    $find = $pquery->find("id='body'"); // $find 是一个 pquery 对象
    var_dump($find->getHtml()); // 输出 id="body" div块的 HTML字符串
    
    $find2 = $find->find("class='wrap'");
    var_dump($find2->getHtml());//这个只返回第一个 class="wrap" div块 HTML 字符串

    //使用 select() 查找元素
    $select = $find->select("class='wrap'");
    var_dump($select); //输出 int(3)
    
    for($i = 0; $i < $select; $i++){
         $item = $find->getItem($i);
         var_dump($item->getHtml()); //输出第 $i + 1 个 class="wrap" div块 HTML 字符串
         $video = $item->find("preload='auto'");
         if(!empty($video)){
             var_dump($video->getHtml()); //输出 video标签的HTML字符串
             var_dump($video->getAttribute("src")); //获取 video 标签内的src属性值
             var_dump($video->getTextContent()); //获取 video 标签的文本内容
         }
    }

    //链式调用
    $text = $pquery->find("class='one'")->find("class='two'")->find("class='three'")->getTextContent();
    var_dump($text); //输出 "这是 three"
```