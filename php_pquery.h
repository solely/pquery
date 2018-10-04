/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:        solely                                                |
  | time:          2018年10月4日16:52:53                                  |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_PQUERY_H
#define PHP_PQUERY_H

extern zend_module_entry pquery_module_entry;
#define phpext_pquery_ptr &pquery_module_entry

#define PHP_PQUERY_VERSION "0.1.0" /* Replace with version number for your extension */

#include "ext/pcre/php_pcre.h"

#ifdef PHP_WIN32
#	define PHP_PQUERY_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_PQUERY_API __attribute__ ((visibility("default")))
#else
#	define PHP_PQUERY_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(pquery)
	zend_long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(pquery)
*/

/* Always refer to the globals in your function as PQUERY_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define PQUERY_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(pquery, v)

#if defined(ZTS) && defined(COMPILE_DL_PQUERY)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#define PREG_PATTERN_ORDER			1
#define PREG_SET_ORDER				2
#define PREG_OFFSET_CAPTURE			(1<<8)

#define PQUERY_PROPERTY_HTML "_html"
#define PQUERY_PROPERTY_NODE "_node"
#define PQUERY_PROPERTY_NODELIST "_nodeList"
#define PQUERY_RETURN_VALUE_COUNT "count"
#define PQUERY_RETURN_VALUE_DATA "data"
#define PQUERY_NORMAL_COUNT_VALUE 0

#define PQUERY_PME_INIT(pme) \
		{zval data; pquery_match_entry _pme = {PQUERY_NORMAL_COUNT_VALUE, &data};pme = _pme;zval_ptr_dtor(&data);}
#define PQUERY_REGEX_BOUNDARY_OPERATOR "#"
#define PQUERY_REGEX_TAG_LEFT "<"
#define PQUERY_REGEX_TAG_RIGHT ">"
#define PQUERY_REGEX_NOT_TAG "[^>]*"
#define PQUERY_REGEX_USED_SUBSCHEMA "(.*?)"
#define PQUERY_REGEX_NOT_USED_SUBSCHEMA ".*?"
#define PQUERY_REGEX_OBLIQUE_LINE "/"
#define PQUERY_REGEX_MODIFIER_S "s"
#define PQUERY_REGEX_HREF_OR_SRC "(href|src)="
#define PQUERY_REGEX_FLAG_TAG_CONTENT "\"(.*?)\""
#define PQUERY_REGEX_NOT_LEFT_ARROWS "[^<]*"
#define PQUERY_REGEX_SPACE_CHARACTER "\\s*"
#define PQUERY_REGEX_NOT_SLASH "[^/]*"
#define PQUERY_REGEX_ELEMENT_VALUE "=\"(.*?)\""
#define PQUERY_FLAG_NORMAL 0
#define PQUERY_FLAG_NORMAL_NAME "PQUERY_FLAG_NORMAL"
#define PQUERY_FLAG_BEFORE 1
#define PQUERY_FLAG_BEFORE_NAME "PQUERY_FLAG_BEFORE"
#define PQUERY_FLAG_AFTER 2
#define PQUERY_FLAG_AFTER_NAME "PQUERY_FLAG_AFTER"
#define PQUERY_FLAG_NOT_MATCH_CONTENT 0
#define PQUERY_FLAG_NOT_MATCH_CONTENT_NAME "PQUERY_FLAG_NMC"
#define PQUERY_FLAG_MATCH_CONTENT 1
#define PQUERY_FLAG_MATCH_CONTENT_NAME "PQUERY_FLAG_MC"
#define PQUERY_FLAG_MATCH_HREF_SRC 2
#define PQUERY_FLAG_MATCH_HREF_SRC_NAME "PQUERY_FLAG_MHS"


typedef struct {
  long count;
  zval *data;
} pquery_match_entry;

PHP_PQUERY_API void pquery_do_match(pcre_cache_entry *pce, pquery_match_entry *pme, char *subject, int subject_len, int global);
PHP_PQUERY_API void pquery_match(char *regex, zend_string *subject, pquery_match_entry *pme);
PHP_PQUERY_API void pquery_match_all(char *regex, zend_string *subject, pquery_match_entry *pme);
PHP_PQUERY_API void pquery_return_value(pquery_match_entry *pme, zval *return_value);
char **make_subpats_table(int num_subpats, pcre_cache_entry *pce);
PHP_PQUERY_API char *pquery_string_concat(int count, ...);
PHP_PQUERY_API char *pquery_regex_tag(char *tag, int flag, char *other_str);
PHP_PQUERY_API char *pquery_regex_not_closed_tag(char *tag, int flag, char *other_str, char *element_name);
PHP_PQUERY_API void pquery_check_pquery_html(zval *html);
PHP_PQUERY_API zend_long pquery_dom_element(char *query_str, zval *html, zval *elements_value, zval *node);

#endif	/* PHP_PQUERY_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
