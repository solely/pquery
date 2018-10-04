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
  | Author:     solely                                                   |
  | time:		2018年10月4日16:55:23                                     |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "ext/standard/info.h"
#include "ext/pcre/php_pcre.h"
#include "php_pquery.h"

enum {
	PHP_PCRE_NO_ERROR = 0,
	PHP_PCRE_INTERNAL_ERROR,
	PHP_PCRE_BACKTRACK_LIMIT_ERROR,
	PHP_PCRE_RECURSION_LIMIT_ERROR,
	PHP_PCRE_BAD_UTF8_ERROR,
	PHP_PCRE_BAD_UTF8_OFFSET_ERROR,
	PHP_PCRE_JIT_STACKLIMIT_ERROR
};

/* If you declare any globals in php_pquery.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(pquery)
*/

/* True global resources - no need for thread safety here */
static int le_pquery;
zend_class_entry *pquery_ce;
zend_string *pquery_html = NULL;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("pquery.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_pquery_globals, pquery_globals)
    STD_PHP_INI_ENTRY("pquery.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_pquery_globals, pquery_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_pquery_compiled(string arg)
   Return a string to confirm that the module is compiled in */

PHP_PQUERY_API char *pquery_regex_not_closed_tag(char *tag, int flag, char *other_str, char *element_name)
{
	char *regex = NULL;
	switch(flag){
		case PQUERY_FLAG_NORMAL:
			if(other_str){
				regex = pquery_string_concat(10,PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_TAG_LEFT, tag,
						PQUERY_REGEX_NOT_TAG, other_str,
						PQUERY_REGEX_NOT_TAG, 
						PQUERY_REGEX_OBLIQUE_LINE, PQUERY_REGEX_TAG_RIGHT, 
						PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_MODIFIER_S);
			}else{
				regex = pquery_string_concat(8,PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_TAG_LEFT, tag,
						PQUERY_REGEX_NOT_TAG, 
						PQUERY_REGEX_OBLIQUE_LINE, PQUERY_REGEX_TAG_RIGHT, 
						PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_MODIFIER_S);
			}
			break;
		case PQUERY_FLAG_BEFORE:
			if(other_str){
				regex = pquery_string_concat(13,PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_TAG_LEFT, tag,
						PQUERY_REGEX_NOT_TAG, other_str,
						PQUERY_REGEX_NOT_TAG, element_name, PQUERY_REGEX_ELEMENT_VALUE, PQUERY_REGEX_NOT_TAG,
						PQUERY_REGEX_OBLIQUE_LINE, PQUERY_REGEX_TAG_RIGHT, 
						PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_MODIFIER_S);
			}else{
				regex = pquery_string_concat(11,PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_TAG_LEFT, tag,
						PQUERY_REGEX_NOT_TAG, element_name, PQUERY_REGEX_ELEMENT_VALUE, PQUERY_REGEX_NOT_TAG,
						PQUERY_REGEX_OBLIQUE_LINE, PQUERY_REGEX_TAG_RIGHT, 
						PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_MODIFIER_S);
			}
			break;
		case PQUERY_FLAG_AFTER:
			if(other_str){
				regex = pquery_string_concat(13,PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_TAG_LEFT, tag,
						PQUERY_REGEX_NOT_TAG, element_name, PQUERY_REGEX_ELEMENT_VALUE
						PQUERY_REGEX_NOT_TAG, other_str, PQUERY_REGEX_NOT_TAG,
						PQUERY_REGEX_OBLIQUE_LINE, PQUERY_REGEX_TAG_RIGHT, 
						PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_MODIFIER_S);
			}else{
				regex = pquery_string_concat(11,PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_TAG_LEFT, tag,
						PQUERY_REGEX_NOT_TAG, element_name, PQUERY_REGEX_ELEMENT_VALUE
						PQUERY_REGEX_NOT_TAG,
						PQUERY_REGEX_OBLIQUE_LINE, PQUERY_REGEX_TAG_RIGHT, 
						PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_MODIFIER_S);
			}
			break;
		default:
			regex = pquery_string_concat(8,PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_TAG_LEFT, tag,
					PQUERY_REGEX_NOT_TAG, 
					PQUERY_REGEX_OBLIQUE_LINE, PQUERY_REGEX_TAG_RIGHT, 
					PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_MODIFIER_S);
			break;
	}
	return regex;
}

PHP_PQUERY_API char *pquery_regex_tag(char *tag, int flag, char *other_str)
{
	char *regex = NULL;
	
	switch(flag){
		case PQUERY_FLAG_NOT_MATCH_CONTENT:
			if(other_str){
				regex = pquery_string_concat(14,PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_TAG_LEFT, tag,
						PQUERY_REGEX_NOT_LEFT_ARROWS, other_str,
						PQUERY_REGEX_NOT_TAG, PQUERY_REGEX_TAG_RIGHT, PQUERY_REGEX_NOT_USED_SUBSCHEMA, 
						PQUERY_REGEX_TAG_LEFT ,PQUERY_REGEX_OBLIQUE_LINE, tag, PQUERY_REGEX_TAG_RIGHT, 
						PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_MODIFIER_S);
			}else{
				regex = pquery_string_concat(12,PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_TAG_LEFT, tag, 
						PQUERY_REGEX_NOT_TAG, PQUERY_REGEX_TAG_RIGHT, PQUERY_REGEX_NOT_USED_SUBSCHEMA, 
						PQUERY_REGEX_TAG_LEFT ,PQUERY_REGEX_OBLIQUE_LINE, tag, PQUERY_REGEX_TAG_RIGHT, 
						PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_MODIFIER_S);
			}
			break;
		case PQUERY_FLAG_MATCH_CONTENT:
			if(other_str){
				regex = pquery_string_concat(14,PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_TAG_LEFT, tag, 
						PQUERY_REGEX_NOT_LEFT_ARROWS, other_str,
						PQUERY_REGEX_NOT_TAG,PQUERY_REGEX_TAG_RIGHT, PQUERY_REGEX_USED_SUBSCHEMA, PQUERY_REGEX_TAG_LEFT ,
						PQUERY_REGEX_OBLIQUE_LINE, tag, PQUERY_REGEX_TAG_RIGHT, PQUERY_REGEX_BOUNDARY_OPERATOR, 
						PQUERY_REGEX_MODIFIER_S);
			}else{
				regex = pquery_string_concat(12,PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_TAG_LEFT, tag, 
						PQUERY_REGEX_NOT_TAG,PQUERY_REGEX_TAG_RIGHT, PQUERY_REGEX_USED_SUBSCHEMA, PQUERY_REGEX_TAG_LEFT ,
						PQUERY_REGEX_OBLIQUE_LINE, tag, PQUERY_REGEX_TAG_RIGHT, PQUERY_REGEX_BOUNDARY_OPERATOR, 
						PQUERY_REGEX_MODIFIER_S);
			}
			break;
		case PQUERY_FLAG_MATCH_HREF_SRC:
			regex = pquery_string_concat(15,PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_TAG_LEFT, tag, 
					PQUERY_REGEX_NOT_USED_SUBSCHEMA,PQUERY_REGEX_HREF_OR_SRC, PQUERY_REGEX_FLAG_TAG_CONTENT,
					PQUERY_REGEX_NOT_TAG,PQUERY_REGEX_TAG_RIGHT, PQUERY_REGEX_USED_SUBSCHEMA, PQUERY_REGEX_TAG_LEFT ,
					PQUERY_REGEX_OBLIQUE_LINE, tag, PQUERY_REGEX_TAG_RIGHT, PQUERY_REGEX_BOUNDARY_OPERATOR, 
					PQUERY_REGEX_MODIFIER_S);
			break;
		default:
			regex = pquery_string_concat(12,PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_TAG_LEFT, tag, 
					PQUERY_REGEX_NOT_TAG, PQUERY_REGEX_TAG_RIGHT, PQUERY_REGEX_NOT_USED_SUBSCHEMA, 
					PQUERY_REGEX_TAG_LEFT ,PQUERY_REGEX_OBLIQUE_LINE, tag, PQUERY_REGEX_TAG_RIGHT, 
					PQUERY_REGEX_BOUNDARY_OPERATOR, PQUERY_REGEX_MODIFIER_S);
			break;
	}

	return regex;
}

PHP_PQUERY_API char *pquery_string_concat(int count, ...)
{
	va_list ap;
	int i;

	int len = 0;
	va_start(ap, count);
	for(i = 0; i < count; i++){
		len += (int)strlen(va_arg(ap, char*));
	}
	va_end(ap);

	char *merged = ecalloc(sizeof(char), len);
	int null_pos = 0;

	va_start(ap, count);
	for(i = 0; i < count; i++){
		char *s = va_arg(ap, char*);
		strcpy(merged + null_pos, s);
		null_pos += strlen(s);
	}
	va_end(ap);

	return merged;
}

PHP_PQUERY_API void pquery_return_value(pquery_match_entry *pme, zval *return_value)
{
	array_init(return_value);
	zval val_count;
	zval data;
	ZVAL_LONG(&val_count, pme->count);
	ZVAL_ARR(&data, Z_ARR_P(pme->data));
	zend_hash_str_add(Z_ARRVAL_P(return_value), PQUERY_RETURN_VALUE_COUNT, sizeof(PQUERY_RETURN_VALUE_COUNT) - 1, &val_count);
	zend_hash_str_add(Z_ARRVAL_P(return_value), PQUERY_RETURN_VALUE_DATA, sizeof(PQUERY_RETURN_VALUE_DATA) - 1, &data);// 这个后面不能直接使用 Z_ARR_P(pme->data),因为这个是返回指针地址的，也就是int值了
	ZVAL_NULL(&val_count);
	pme = NULL;
}

PHP_PQUERY_API void pquery_do_match(pcre_cache_entry *pce,pquery_match_entry *pme, char *subject, int subject_len, int global)
{
	zval             subpats;
	zval			 result_set,		/* Holds a set of subpatterns after
										   a global match */
				    *match_sets = NULL;	/* An array of sets of matches for each
										   subpattern after a global match */
	pcre_extra		*extra = pce->extra;/* Holds results of studying */
	pcre_extra		 extra_data;		/* Used locally for exec options */
	int				 exoptions = 0;		/* Execution options */
	int				 count = 0;			/* Count of matched subpatterns */
	int				*offsets;			/* Array of subpattern offsets */
	int				 num_subpats;		/* Number of captured subpatterns */
	int				 size_offsets;		/* Size of the offsets array */
	int				 matched;			/* Has anything matched */
	int				 g_notempty = 0;	/* If the match should not be empty */
	const char	   **stringlist;		/* Holds list of subpatterns */
	char 		   **subpat_names;		/* Array for named subpatterns */
	int				 i;
	int				 subpats_order;		/* Order of subpattern matches */
	int				 offset_capture = 0;    /* Capture match offsets: yes/no */
	unsigned char   *mark = NULL;       /* Target for MARK name */
	zval            marks;      		/* Array of marks for PREG_PATTERN_ORDER */
	zend_long             start_offset = 0;

	ALLOCA_FLAG(use_heap);

	ZVAL_UNDEF(&marks);
	
	array_init(&subpats);

	subpats_order = global ? PREG_PATTERN_ORDER : 0;

	if (extra == NULL) {
		extra_data.flags = PCRE_EXTRA_MATCH_LIMIT | PCRE_EXTRA_MATCH_LIMIT_RECURSION;
		extra = &extra_data;
	}
	extra->match_limit = (unsigned long)PCRE_G(backtrack_limit);
	extra->match_limit_recursion = (unsigned long)PCRE_G(recursion_limit);
#ifdef PCRE_EXTRA_MARK
	extra->mark = &mark;
	extra->flags |= PCRE_EXTRA_MARK;
#endif

	num_subpats = pce->capture_count + 1;
	size_offsets = num_subpats * 3;

	subpat_names = NULL;
	if (pce->name_count > 0) {
		subpat_names = make_subpats_table(num_subpats, pce);
		if (!subpat_names) {
			pme->count = -1;
			return;
		}
	}

	if (size_offsets <= 32) {
		offsets = (int *)do_alloca(size_offsets * sizeof(int), use_heap);
	} else {
		offsets = (int *)safe_emalloc(size_offsets, sizeof(int), 0);
	}
	memset(offsets, 0, size_offsets*sizeof(int));
	
	if (global && (&subpats) && subpats_order == PREG_PATTERN_ORDER) {
		match_sets = (zval *)safe_emalloc(num_subpats, sizeof(zval), 0);
		for (i=0; i<num_subpats; i++) {
			array_init(&match_sets[i]);
		}
	}

	matched = 0;
	PCRE_G(error_code) = PHP_PCRE_NO_ERROR;


	do {
		count = pcre_exec(pce->re, extra, subject, (int)subject_len, (int)start_offset,
						  exoptions|g_notempty, offsets, size_offsets);

		exoptions |= PCRE_NO_UTF8_CHECK;

		if (count == 0) {
			php_error_docref(NULL, E_NOTICE, "Matched, but too many substrings");
			count = size_offsets/3;
		}

		if (count > 0) {
			matched++;

			if (&subpats != NULL) {
				if ((offsets[1] - offsets[0] < 0) || pcre_get_substring_list(subject, offsets, count, &stringlist) < 0) {
					if (subpat_names) {
						efree(subpat_names);
					}
					if (size_offsets <= 32) {
						free_alloca(offsets, use_heap);
					} else {
						efree(offsets);
					}
					if (match_sets) efree(match_sets);
					pme->count = -1;
					return;
				}

				if (global) {
					if (&subpats && subpats_order == PREG_PATTERN_ORDER) {
						if (offset_capture) {
							for (i = 0; i < count; i++) {
								add_offset_pair(&match_sets[i], (char *)stringlist[i],
												offsets[(i<<1)+1] - offsets[i<<1], offsets[i<<1], NULL);
							}
						} else {
							for (i = 0; i < count; i++) {
								add_next_index_stringl(&match_sets[i], (char *)stringlist[i],
													   offsets[(i<<1)+1] - offsets[i<<1]);
							}
						}
						if (mark) {
							if (Z_TYPE(marks) == IS_UNDEF) {
								array_init(&marks);
							}
							add_index_string(&marks, matched - 1, (char *) mark);
						}
						if (count < num_subpats) {
							for (; i < num_subpats; i++) {
								add_next_index_string(&match_sets[i], "");
							}
						}
					} else {
						array_init_size(&result_set, count + (mark ? 1 : 0));

						if (subpat_names) {
							if (offset_capture) {
								for (i = 0; i < count; i++) {
									add_offset_pair(&result_set, (char *)stringlist[i],
													offsets[(i<<1)+1] - offsets[i<<1], offsets[i<<1], subpat_names[i]);
								}
							} else {
								for (i = 0; i < count; i++) {
									if (subpat_names[i]) {
										add_assoc_stringl(&result_set, subpat_names[i], (char *)stringlist[i],
															   offsets[(i<<1)+1] - offsets[i<<1]);
									}
									add_next_index_stringl(&result_set, (char *)stringlist[i],
														   offsets[(i<<1)+1] - offsets[i<<1]);
								}
							}
						} else {
							if (offset_capture) {
								for (i = 0; i < count; i++) {
									add_offset_pair(&result_set, (char *)stringlist[i],
													offsets[(i<<1)+1] - offsets[i<<1], offsets[i<<1], NULL);
								}
							} else {
								for (i = 0; i < count; i++) {
									add_next_index_stringl(&result_set, (char *)stringlist[i],
														   offsets[(i<<1)+1] - offsets[i<<1]);
								}
							}
						}
						if (mark) {
							add_assoc_string_ex(&result_set, "MARK", sizeof("MARK") - 1, (char *)mark);
						}
						zend_hash_next_index_insert(Z_ARRVAL(subpats), &result_set);
					}
				} else {
					if (subpat_names) {
						if (offset_capture) {
							for (i = 0; i < count; i++) {
								add_offset_pair(&subpats, (char *)stringlist[i],
												offsets[(i<<1)+1] - offsets[i<<1],
												offsets[i<<1], subpat_names[i]);
							}
						} else {
							for (i = 0; i < count; i++) {
								if (subpat_names[i]) {
									add_assoc_stringl(&subpats, subpat_names[i], (char *)stringlist[i],
													  offsets[(i<<1)+1] - offsets[i<<1]);
								}
								add_next_index_stringl(&subpats, (char *)stringlist[i],
													   offsets[(i<<1)+1] - offsets[i<<1]);
							}
						}
					} else {
						if (offset_capture) {
							for (i = 0; i < count; i++) {
								add_offset_pair(&subpats, (char *)stringlist[i],
												offsets[(i<<1)+1] - offsets[i<<1],
												offsets[i<<1], NULL);
							}
						} else {
							for (i = 0; i < count; i++) {
								add_next_index_stringl(&subpats, (char *)stringlist[i],
													   offsets[(i<<1)+1] - offsets[i<<1]);
							}
						}
					}

					if (mark) {
						add_assoc_string_ex(&subpats, "MARK", sizeof("MARK") - 1, (char *)mark);
					}
				}

				pcre_free((void *) stringlist);
			}
		} else if (count == PCRE_ERROR_NOMATCH) {
			if (g_notempty != 0 && start_offset < subject_len) {
				int unit_len = calculate_unit_length(pce, subject + start_offset);

				offsets[0] = (int)start_offset;
				offsets[1] = (int)(start_offset + unit_len);
			} else
				break;
		} else {
			pcre_handle_exec_error(count);
			break;
		}

		g_notempty = (offsets[1] == offsets[0])? PCRE_NOTEMPTY_ATSTART | PCRE_ANCHORED : 0;

		start_offset = offsets[1];
	} while (global);

	if (global && (&subpats) && subpats_order == PREG_PATTERN_ORDER) {
		if (subpat_names) {
			for (i = 0; i < num_subpats; i++) {
				if (subpat_names[i]) {
					zend_hash_str_update(Z_ARRVAL(subpats), subpat_names[i],
									 strlen(subpat_names[i]), &match_sets[i]);
					Z_ADDREF(match_sets[i]);
				}
				zend_hash_next_index_insert(Z_ARRVAL(subpats), &match_sets[i]);
			}
		} else {
			for (i = 0; i < num_subpats; i++) {
				zend_hash_next_index_insert(Z_ARRVAL(subpats), &match_sets[i]);
			}
		}
		efree(match_sets);

		if (Z_TYPE(marks) != IS_UNDEF) {
			add_assoc_zval(&subpats, "MARK", &marks);
		}
	}

	if (size_offsets <= 32) {
		free_alloca(offsets, use_heap);
	} else {
		efree(offsets);
	}
	if (subpat_names) {
		efree(subpat_names);
	}

	if (PCRE_G(error_code) == PHP_PCRE_NO_ERROR) {
		pme->count = matched;
	} else {
		pme->count = -1;
	}

	ZVAL_ARR(pme->data, Z_ARR_P(&subpats));
}

PHP_PQUERY_API void pquery_match(char *regex, zend_string *subject, pquery_match_entry *pme)
{
	const char *pattern = regex;
	zend_string *reg = NULL;
	pcre_cache_entry *pce;

	reg = zend_string_init(pattern, strlen(pattern), 0);

	if ((pce = pcre_get_compiled_regex_cache(reg)) == NULL) {
		pme->count = -1;
	}
	zend_string_release(reg);

	pce->refcount++;
	pquery_do_match(pce, pme,ZSTR_VAL(subject), (int)ZSTR_LEN(subject), 0);
	pce->refcount--;
	efree(regex);
	regex = NULL;
}

PHP_PQUERY_API void pquery_match_all(char *regex, zend_string *subject, pquery_match_entry *pme)
{
	const char *pattern = regex;
	zend_string *reg = NULL;
	pcre_cache_entry *pce;

	reg = zend_string_init(pattern, strlen(pattern), 0);

	if ((pce = pcre_get_compiled_regex_cache(reg)) == NULL) {
		pme->count = -1;
	}
	zend_string_release(reg);

	pce->refcount++;
	pquery_do_match(pce, pme,ZSTR_VAL(subject), (int)ZSTR_LEN(subject), 1);
	pce->refcount--;
	efree(regex);
	regex = NULL;
}

PHP_PQUERY_API void pquery_check_pquery_html(zval *html)
{
	if(ZVAL_IS_NULL(html)){
		php_error_docref(NULL,E_ERROR, "html can not be NULL");
	}
}

PHP_PQUERY_API zend_long pquery_dom_element(char *query_str, zval *html, zval *elements_value, zval *node)
{
	zval call_dom_name, call_dom_ret, call_dom_params;
	zval call_dom_method_load_html_name;
	uint32_t call_dom_param_cnt = 1;
	zend_string *call_dom_name_str;
	char *dom_name = "DOMDocument";
	char *dom_method_load_html = "loadHTML";

	zval luie_name,luie_ret, luie_params;
	zend_string *libxml_use_internal_errors = zend_string_init("libxml_use_internal_errors", strlen("libxml_use_internal_errors"), 0);
	ZVAL_STR(&luie_name, libxml_use_internal_errors);
	ZVAL_BOOL(&luie_params, 1); 
	if(SUCCESS != call_user_function(EG(function_table), NULL, &luie_name, &luie_ret, 1, &luie_params)){
		zend_string_release(libxml_use_internal_errors);
		php_error_docref(NULL,E_ERROR, "libxml_user_internal_errors call fails");
	}

	zval dom_xpath_name,dom_xpath_ret,dom_xpath__construct_method,dom_xpath_query_method;
	char *dom_xpath = "DOMXpath";
	char *dom_xpath__construct = "__construct";
	char *dom_xpath_query = "query";
	zend_string *dom_xpath_name_str = zend_string_init(dom_xpath, strlen(dom_xpath), 0);
	zend_class_entry *dom_xpath_class = zend_lookup_class(dom_xpath_name_str);
	object_init_ex(&dom_xpath_name, dom_xpath_class);
	zend_string *dom_xpath__construct_method_str = zend_string_init(dom_xpath__construct, strlen(dom_xpath__construct), 0);
	ZVAL_STR(&dom_xpath__construct_method, dom_xpath__construct_method_str);

	if(ZVAL_IS_NULL(node)){
		call_dom_name_str = zend_string_init(dom_name, strlen(dom_name), 0);
		ZVAL_STR(&call_dom_name, call_dom_name_str);
		zend_string *call_dom_method_load_html = zend_string_init(dom_method_load_html, strlen(dom_method_load_html), 0);
		ZVAL_STR(&call_dom_method_load_html_name, call_dom_method_load_html);
		zend_class_entry *dom_class = zend_lookup_class(call_dom_name_str);

		object_init_ex(&call_dom_name, dom_class);
		if(SUCCESS != call_user_function(zend_class_entry.function_table, &call_dom_name, &call_dom_method_load_html_name, &call_dom_ret, call_dom_param_cnt, html)){
			zend_string_release(call_dom_name_str);
			zend_string_release(call_dom_method_load_html);
			zval_ptr_dtor(&call_dom_name);
			php_error_docref(NULL, E_ERROR, "loadHTML call fails");
		}

		zend_string_release(call_dom_name_str);
		zend_string_release(call_dom_method_load_html);
		if(SUCCESS != call_user_function(zend_class_entry.function_table, &dom_xpath_name, &dom_xpath__construct_method, &dom_xpath_ret,1,&call_dom_name)){
			zend_string_release(libxml_use_internal_errors);
			zval_ptr_dtor(&call_dom_name);
			zend_string_release(dom_xpath_name_str);
			zend_string_release(dom_xpath__construct_method_str);
			zval_ptr_dtor(&dom_xpath_name);
			php_error_docref(NULL, E_ERROR, "DOMXpath Initialization failed");
		}
		zval_ptr_dtor(&call_dom_name);
	} else {
		zval dom_document_rv;
		zval *dom_document = zend_read_property(Z_OBJCE_P(node), node, "ownerDocument", sizeof("ownerDocument") - 1, 0, &dom_document_rv);
		if(SUCCESS != call_user_function(zend_class_entry.function_table, &dom_xpath_name, &dom_xpath__construct_method, &dom_xpath_ret,1,dom_document)){
			zend_string_release(libxml_use_internal_errors);
			zend_string_release(dom_xpath_name_str);
			zend_string_release(dom_xpath__construct_method_str);
			zval_ptr_dtor(&dom_xpath_name);
			php_error_docref(NULL, E_ERROR, "DOMXpath Initialization failed");
		}
	}
	zend_string_release(dom_xpath_name_str);
	zend_string_release(dom_xpath__construct_method_str);

	zval dom_xpath_query_ret, query_params;

	zend_string *dom_xpath_query_method_str = zend_string_init(dom_xpath_query,strlen(dom_xpath_query),0);
	ZVAL_STR(&dom_xpath_query_method, dom_xpath_query_method_str);
	zend_string *query_params_str = zend_string_init(query_str, strlen(query_str),0);

	ZVAL_STR(&query_params, query_params_str);
	if(SUCCESS != call_user_function(zend_class_entry.function_table, &dom_xpath_name, &dom_xpath_query_method,elements_value,1,&query_params)){
		zend_string_release(libxml_use_internal_errors);
		zval_ptr_dtor(&dom_xpath_name);
		zend_string_release(dom_xpath_query_method_str);
		zend_string_release(query_params_str);
		php_error_docref(NULL, E_ERROR, "query call fails");
	}
	zend_string_release(dom_xpath_query_method_str);
	zend_string_release(query_params_str);
	
	if(!zval_is_true(elements_value)){
		zval_ptr_dtor(&dom_xpath_name);
		zend_string_release(libxml_use_internal_errors);
		return -1;
	}

	zend_string *nodeList_length = zend_string_init("length", strlen("length"), 0);
	zval length_ret;
	zval *dom_nodeList_length = zend_read_property_ex(Z_OBJCE_P(elements_value),elements_value, nodeList_length, 0, &length_ret);
	zval_ptr_dtor(&dom_xpath_name);
	zend_string_release(nodeList_length);

	zval luierrors_ret;
	if(SUCCESS != call_user_function(EG(function_table), NULL, &luie_name, &luierrors_ret, 1, &luie_ret)){
		zend_string_release(libxml_use_internal_errors);
		php_error_docref(NULL, E_ERROR, "libxml_use_internal_errors call fails");
	}
	zend_string_release(libxml_use_internal_errors);
	return zval_get_long(dom_nodeList_length);
}

PHP_FUNCTION(confirm_pquery_compiled)
{
	char *arg = NULL;
	size_t arg_len, len;
	zend_string *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	strg = strpprintf(0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "pquery", arg);

	RETURN_STR(strg);
}

/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_pquery_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_pquery_init_globals(zend_pquery_globals *pquery_globals)
{
	pquery_globals->global_value = 0;
	pquery_globals->global_string = NULL;
}
*/
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_pquery_html, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, html, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pquery_matchByTag, 0, 0, 3)
	ZEND_ARG_INFO(0, tag)
	ZEND_ARG_INFO(0, flag)
	ZEND_ARG_INFO(0, other_str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pquery_matchByA, 0, 0, 2)
	ZEND_ARG_INFO(0, flag)
	ZEND_ARG_INFO(0, other_str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pquery_getMetaByName, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pquery_setHtml, 0, 0, 1)
	ZEND_ARG_INFO(0, html)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pquery_find, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pquery_getAttribute, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pquery_getItem, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pquery_select, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

PHP_METHOD(pquery, __construct)
{
	zend_string *html = NULL;

	#ifndef FAST_ZPP
		if(zend_parse_parameters(ZEND_NUM_ARGS(),"|S", &html) == FAILURE){
			return;
		}
	#else
		ZEND_PARSE_PARAMETERS_START(0,1)
			Z_PARAM_OPTIONAL
			Z_PARAM_STR(html)
		ZEND_PARSE_PARAMETERS_END();
	#endif

	if(html != NULL){
		zend_update_property_str(pquery_ce,getThis(),PQUERY_PROPERTY_HTML, sizeof(PQUERY_PROPERTY_HTML) - 1, html);
	}
}

PHP_METHOD(pquery, getTitle)
{
	zval *html = zend_read_property(pquery_ce,getThis(),PQUERY_PROPERTY_HTML, sizeof(PQUERY_PROPERTY_HTML) - 1, 0, NULL);
	pquery_check_pquery_html(html);
	
	pquery_match_entry pme;
	PQUERY_PME_INIT(pme);
	char *regex = pquery_regex_tag("title", 1, NULL);
	pquery_match(regex,zval_get_string(html), &pme);
	zval_ptr_dtor(html);
	pquery_return_value(&pme, return_value);
}

PHP_METHOD(pquery, matchByTag)
{
	zval *html = zend_read_property(pquery_ce,getThis(),PQUERY_PROPERTY_HTML, sizeof(PQUERY_PROPERTY_HTML) - 1, 0, NULL);
	pquery_check_pquery_html(html);
	char *tag = NULL;
	char *other_str = NULL;
	size_t tag_len;
	size_t other_str_len;
	zend_long flag = 0; //是否开启子模式，0：不开启，1：开启， 默认 不开启。 PHP层面传 false也可以不开启，传 true 开启

	#ifndef FAST_ZPP
		if(zend_parse_parameters(ZEND_NUM_ARGS(), "s|ls", &tag, &tag_len, &flag, &other_str, &other_str_len) == FAILURE){
			return;
		}
	#else
		ZEND_PARSE_PARAMETERS_START(1,3)
			Z_PARAM_STRING(tag, tag_len)
			Z_PARAM_OPTIONAL
			Z_PARAM_LONG(flag)
			Z_PARAM_STRING(other_str, other_str_len)
		ZEND_PARSE_PARAMETERS_END();
	#endif
	
	char *regex = pquery_regex_tag(tag, flag, other_str);
	pquery_match_entry pme;
	PQUERY_PME_INIT(pme);
	pquery_match_all(regex,zval_get_string(html), &pme);
	zval_ptr_dtor(html);
	pquery_return_value(&pme, return_value);
}

PHP_METHOD(pquery, matchByA)
{
	zval *html = zend_read_property(pquery_ce,getThis(),PQUERY_PROPERTY_HTML, sizeof(PQUERY_PROPERTY_HTML) - 1, 0, NULL);
	pquery_check_pquery_html(html);
	zend_long flag = 0;
	char *other_str = NULL; // 其它字符串，有可能是 id 的字符串，有可能是 class 的字符串，还有可能是其它标志性 字符串
	size_t other_str_len;
	pquery_match_entry pme;

	#ifndef FAST_ZPP
		if(zend_parse_parameters(ZEND_NUM_ARGS(), "|ls",$flag, &other_str, &other_str_len) == FAILURE){
			return;
		}
	#else
		ZEND_PARSE_PARAMETERS_START(0,2)
			Z_PARAM_OPTIONAL
			Z_PARAM_LONG(flag)
			Z_PARAM_STRING(other_str, other_str_len)
		ZEND_PARSE_PARAMETERS_END();
	#endif

	char *regex = pquery_regex_tag("a", flag, other_str);

	PQUERY_PME_INIT(pme);
	pquery_match_all(regex,zval_get_string(html), &pme);
	zval_ptr_dtor(html);
	pquery_return_value(&pme, return_value);
}

PHP_METHOD(pquery, getMetaByName)
{
	zval *html = zend_read_property(pquery_ce,getThis(),PQUERY_PROPERTY_HTML, sizeof(PQUERY_PROPERTY_HTML) - 1, 0, NULL);
	pquery_check_pquery_html(html);
	char *name = NULL;
	size_t name_len;
	pquery_match_entry pme;

	#ifndef FAST_ZPP
		if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE){
			return;
		}
	#else
		ZEND_PARSE_PARAMETERS_START(1,1)
			Z_PARAM_STRING(name, name_len)
		ZEND_PARSE_PARAMETERS_END();
	#endif

	char *regex = pquery_regex_not_closed_tag("meta", 1, name, "content");
	
	PQUERY_PME_INIT(pme);
	pquery_match(regex,zval_get_string(html), &pme);
	zval_ptr_dtor(html);
	pquery_return_value(&pme, return_value);
}

PHP_METHOD(pquery, find)
{
	char *name;
	size_t name_len;
	#ifndef FAST_ZPP
		if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE){
			return;
		}
	#else
		ZEND_PARSE_PARAMETERS_START(1,1)
			Z_PARAM_STRING(name, name_len)
		ZEND_PARSE_PARAMETERS_END();
	#endif

	char *_name = pquery_string_concat(3, "//*[@", name , "]");
	zval *html = zend_read_property(pquery_ce,getThis(),PQUERY_PROPERTY_HTML, sizeof(PQUERY_PROPERTY_HTML) - 1, 0, NULL);
	zval dom_ret;
	zval *node = zend_read_property(pquery_ce,getThis(),PQUERY_PROPERTY_NODE, sizeof(PQUERY_PROPERTY_NODE) - 1, 0, NULL);

	if(ZVAL_IS_NULL(node)){
		pquery_check_pquery_html(html);
	}
	
	zend_long length = pquery_dom_element(_name, html, &dom_ret, node);		
	efree(_name);

	if(length == -1){
		zval_ptr_dtor(&dom_ret);
		RETURN_NULL();
	}

	zval self;
	object_init_ex(&self,pquery_ce);
	if(length == 0){
		pquery_html = zend_string_init("",0,0);
		zend_update_property_str(Z_OBJCE_P(&self),&self,PQUERY_PROPERTY_HTML, sizeof(PQUERY_PROPERTY_HTML) - 1, pquery_html);
		zend_update_property_null(Z_OBJCE_P(&self), &self, PQUERY_PROPERTY_NODE, sizeof(PQUERY_PROPERTY_NODE) - 1);
		zend_string_release(pquery_html);
		zval_ptr_dtor(&dom_ret);
		RETURN_ZVAL(&self, 1, 1);
	}

	zval dom_node_item_ret, dom_node_item_name, dom_node_item_params;
	zend_string *dom_node_item_name_str = zend_string_init("item", strlen("item"), 0);
	ZVAL_STR(&dom_node_item_name, dom_node_item_name_str);
	ZVAL_LONG(&dom_node_item_params, 0);
	if(SUCCESS != call_user_function(zend_class_entry.function_table, &dom_ret, &dom_node_item_name, &dom_node_item_ret, 1, &dom_node_item_params)){
		zend_string_release(dom_node_item_name_str);
		zval_ptr_dtor(&dom_ret);
		zval_ptr_dtor(&dom_node_item_ret);
		php_error_docref(NULL, E_ERROR, "item call fails");
	}
	zend_string_release(dom_node_item_name_str);
	zval_ptr_dtor(&dom_ret);

	zval ownerDocument_ret;
	zval *dom_ownerDocument = zend_read_property(Z_OBJCE(dom_node_item_ret), &dom_node_item_ret, "ownerDocument", sizeof("ownerDocument") - 1, 0, &ownerDocument_ret);
	zval dom_element_saveHTML_ret, dom_element_saveHTML_name;
	zend_string *dom_element_saveHTML_name_str = zend_string_init("saveHTML", strlen("saveHTML"), 0);
	ZVAL_STR(&dom_element_saveHTML_name, dom_element_saveHTML_name_str);
	if(SUCCESS != call_user_function(zend_class_entry.function_table, dom_ownerDocument,&dom_element_saveHTML_name, &dom_element_saveHTML_ret, 1, &dom_node_item_ret)){
		zend_string_release(dom_element_saveHTML_name_str);
		zval_ptr_dtor(&dom_node_item_ret);
		zval_ptr_dtor(dom_ownerDocument);
		zval_ptr_dtor(&dom_element_saveHTML_ret);
		php_error_docref(NULL, E_ERROR, "saveHTML call fails");
	}
	zend_string_release(dom_element_saveHTML_name_str);
	zval_ptr_dtor(dom_ownerDocument);

	zend_update_property_str(Z_OBJCE_P(&self),&self,PQUERY_PROPERTY_HTML, sizeof(PQUERY_PROPERTY_HTML) - 1, Z_STR(dom_element_saveHTML_ret));
	zend_string *_node = zend_string_init(PQUERY_PROPERTY_NODE ,sizeof(PQUERY_PROPERTY_NODE) - 1, 0);
	zend_update_property_ex(Z_OBJCE_P(&self), &self, _node, &dom_node_item_ret);
	zend_string_release(_node);
	zval_ptr_dtor(&dom_element_saveHTML_ret);
	zval_ptr_dtor(&dom_node_item_ret);
	RETURN_ZVAL(&self, 1, 1);
}

PHP_METHOD(pquery, setHtml)
{
	zend_string *html = NULL;

	#ifndef FAST_ZPP
		if(zend_parse_parameters(ZEND_NUM_ARGS(),"S", &html) == FAILURE){
			return;
		}
	#else
		ZEND_PARSE_PARAMETERS_START(1,1)
			Z_PARAM_STR(html)
		ZEND_PARSE_PARAMETERS_END();
	#endif

	if( ZSTR_LEN(html) == 0){
		php_error_docref(NULL, E_ERROR, "An empty string is passed in\n");
	}

	zend_update_property_str(pquery_ce,getThis(),PQUERY_PROPERTY_HTML, sizeof(PQUERY_PROPERTY_HTML) - 1, html);
	RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(pquery, getHtml)
{
	zval *html = zend_read_property(pquery_ce,getThis(),PQUERY_PROPERTY_HTML, sizeof(PQUERY_PROPERTY_HTML) - 1, 0, NULL);
	RETURN_STR(zval_get_string(html));
}

PHP_METHOD(pquery, getTextContent)
{
	zval rv;
	zval *node = zend_read_property(pquery_ce,getThis(),PQUERY_PROPERTY_NODE, sizeof(PQUERY_PROPERTY_NODE) - 1, 0, NULL);
	if(ZVAL_IS_NULL(node)){
		php_error_docref(NULL, E_ERROR, "node can not be NULL");
	}
	zval *str = zend_read_property(Z_OBJCE_P(node), node, "textContent", sizeof("textContent") - 1, 0, &rv);
	zval_ptr_dtor(&rv);

	RETURN_STR(zval_get_string(str));
}

PHP_METHOD(pquery, getNode)
{
	zval *node = zend_read_property(pquery_ce,getThis(),PQUERY_PROPERTY_NODE, sizeof(PQUERY_PROPERTY_NODE) - 1, 0, NULL);
	if(ZVAL_IS_NULL(node)){
		RETURN_NULL();
	}
	Z_TRY_ADDREF_P(node);
	RETURN_OBJ(Z_OBJ_P(node));
}

PHP_METHOD(pquery, getAttribute)
{
	zval *node = zend_read_property(pquery_ce,getThis(),PQUERY_PROPERTY_NODE, sizeof(PQUERY_PROPERTY_NODE) - 1, 0, NULL);
	if(ZVAL_IS_NULL(node)){
		php_error_docref(NULL, E_ERROR, "node can not be NULL");
	}
	zend_string *name;

	#ifndef FAST_ZPP
		if(zend_parse_parameters(ZEND_NUM_ARGS(),"S", &name) == FAILURE){
			return;
		}
	#else
		ZEND_PARSE_PARAMETERS_START(1,1)
			Z_PARAM_STR(name)
		ZEND_PARSE_PARAMETERS_END();
	#endif

	if(name == NULL){
		RETURN_EMPTY_STRING();
	}
	zval getAttribute_name, getAttribute_ret, _name;
	zend_string *getAttribute = zend_string_init("getAttribute", sizeof("getAttribute") - 1, 0);
	ZVAL_STR(&getAttribute_name, getAttribute);
	ZVAL_STR(&_name, name);
	
	if(SUCCESS != call_user_function(zend_class_entry.function_table, node, &getAttribute_name, &getAttribute_ret, 1, &_name)){
		zend_string_release(getAttribute);
		php_error_docref(NULL, E_ERROR, "getAttribute call fails");
	}
	zend_string_release(getAttribute);

	RETURN_ZVAL(&getAttribute_ret,1 ,1);
}

PHP_METHOD(pquery, getItem)
{
	zend_long index = 0;
	#ifndef FAST_ZPP
		if(zend_parse_parameters(ZEND_NUM_ARGS(), "l", &index) == FAILURE){
			return;
		}
	#else
		ZEND_PARSE_PARAMETERS_START(1,1)
			Z_PARAM_LONG(index)
		ZEND_PARSE_PARAMETERS_END();
	#endif

	zval *node_list = zend_read_property(pquery_ce,getThis(),PQUERY_PROPERTY_NODELIST, sizeof(PQUERY_PROPERTY_NODELIST) - 1, 0, NULL);

	if(ZVAL_IS_NULL(node_list)){
		RETURN_NULL();
	}
	
	if(index <= 0){
		index = 0;
	}
	
	zval dom_node_item_ret, dom_node_item_name, dom_node_item_params;
	zend_string *dom_node_item_name_str = zend_string_init("item", strlen("item"), 0);
	ZVAL_STR(&dom_node_item_name, dom_node_item_name_str);
	ZVAL_LONG(&dom_node_item_params, index);
	if(SUCCESS != call_user_function(zend_class_entry.function_table, node_list, &dom_node_item_name, &dom_node_item_ret, 1, &dom_node_item_params)){
		zend_string_release(dom_node_item_name_str);
		zval_ptr_dtor(&dom_node_item_ret);
		php_error_docref(NULL, E_ERROR, "item call fails");
	}
	zend_string_release(dom_node_item_name_str);

	if(ZVAL_IS_NULL(&dom_node_item_ret)){
		zval_ptr_dtor(&dom_node_item_ret);
		RETURN_NULL();
	}
	zval ownerDocument_ret;
	zval *dom_ownerDocument = zend_read_property(Z_OBJCE(dom_node_item_ret), &dom_node_item_ret, "ownerDocument", sizeof("ownerDocument") - 1, 0, &ownerDocument_ret);
	zval dom_element_saveHTML_ret, dom_element_saveHTML_name;
	zend_string *dom_element_saveHTML_name_str = zend_string_init("saveHTML", strlen("saveHTML"), 0);
	ZVAL_STR(&dom_element_saveHTML_name, dom_element_saveHTML_name_str);
	if(SUCCESS != call_user_function(zend_class_entry.function_table, dom_ownerDocument,&dom_element_saveHTML_name, &dom_element_saveHTML_ret, 1, &dom_node_item_ret)){
		zend_string_release(dom_element_saveHTML_name_str);
		zval_ptr_dtor(&dom_node_item_ret);
		zval_ptr_dtor(dom_ownerDocument);
		zval_ptr_dtor(&dom_element_saveHTML_ret);
		php_error_docref(NULL, E_ERROR, "saveHTML call fails");
	}
	zend_string_release(dom_element_saveHTML_name_str);
	zval_ptr_dtor(dom_ownerDocument);

	zval self;
	object_init_ex(&self,pquery_ce);
	zend_update_property_str(Z_OBJCE_P(&self),&self,PQUERY_PROPERTY_HTML, sizeof(PQUERY_PROPERTY_HTML) - 1, Z_STR(dom_element_saveHTML_ret));
	zend_string *_node = zend_string_init(PQUERY_PROPERTY_NODE ,sizeof(PQUERY_PROPERTY_NODE) - 1, 0);
	zend_update_property_ex(Z_OBJCE_P(&self), &self, _node, &dom_node_item_ret);
	zend_string_release(_node);
	zval_ptr_dtor(&dom_element_saveHTML_ret);
	zval_ptr_dtor(&dom_node_item_ret);
	RETURN_ZVAL(&self, 1, 1);
}

PHP_METHOD(pquery, select)
{
	char *name;
	size_t name_len;
	#ifndef FAST_ZPP
		if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE){
			return;
		}
	#else
		ZEND_PARSE_PARAMETERS_START(1,1)
			Z_PARAM_STRING(name, name_len)
		ZEND_PARSE_PARAMETERS_END();
	#endif

	char *_name = pquery_string_concat(3, "//*[@", name , "]");
	zval *html = zend_read_property(pquery_ce,getThis(),PQUERY_PROPERTY_HTML, sizeof(PQUERY_PROPERTY_HTML) - 1, 0, NULL);
	zval dom_ret;
	zval *node = zend_read_property(pquery_ce,getThis(),PQUERY_PROPERTY_NODE, sizeof(PQUERY_PROPERTY_NODE) - 1, 0, NULL);

	if(ZVAL_IS_NULL(node)){
		pquery_check_pquery_html(html);
	}
	zend_long length = pquery_dom_element(_name, html, &dom_ret, node);		
	
	efree(_name);

	zend_update_property(pquery_ce, getThis(), PQUERY_PROPERTY_NODELIST, sizeof(PQUERY_PROPERTY_NODELIST) - 1, &dom_ret);

	zval_ptr_dtor(&dom_ret);
	if(length <= 0){
		RETURN_LONG(0);
	}
	RETURN_LONG(length);
}

zend_function_entry pquery_methods[] = {
	PHP_ME(pquery, __construct, arginfo_pquery_html, ZEND_ACC_PUBLIC)
	PHP_ME(pquery, getTitle, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(pquery, matchByTag, arginfo_pquery_matchByTag, ZEND_ACC_PUBLIC)
	PHP_ME(pquery, matchByA, arginfo_pquery_matchByA, ZEND_ACC_PUBLIC)
	PHP_ME(pquery, getMetaByName, arginfo_pquery_getMetaByName, ZEND_ACC_PUBLIC)
	PHP_ME(pquery, find, arginfo_pquery_find, ZEND_ACC_PUBLIC)
	PHP_ME(pquery, setHtml, arginfo_pquery_setHtml, ZEND_ACC_PUBLIC)
	PHP_ME(pquery, getHtml, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(pquery, getTextContent, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(pquery, getNode, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(pquery, getAttribute, arginfo_pquery_getAttribute, ZEND_ACC_PUBLIC)
	PHP_ME(pquery, getItem, arginfo_pquery_getItem, ZEND_ACC_PUBLIC)
	PHP_ME(pquery, select, arginfo_pquery_select, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(pquery)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "pquery", pquery_methods);

	pquery_ce = zend_register_internal_class(&ce);
	zend_declare_property_null(pquery_ce, ZEND_STRL(PQUERY_PROPERTY_HTML),ZEND_ACC_PRIVATE);
	zend_declare_property_null(pquery_ce, ZEND_STRL(PQUERY_PROPERTY_NODE),ZEND_ACC_PRIVATE);
	zend_declare_property_null(pquery_ce, ZEND_STRL(PQUERY_PROPERTY_NODELIST),ZEND_ACC_PRIVATE);

	REGISTER_LONG_CONSTANT(PQUERY_FLAG_NORMAL_NAME, PQUERY_FLAG_NORMAL, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT(PQUERY_FLAG_BEFORE_NAME, PQUERY_FLAG_BEFORE, CONST_PERSISTENT); 
	REGISTER_LONG_CONSTANT(PQUERY_FLAG_AFTER_NAME, PQUERY_FLAG_AFTER, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT(PQUERY_FLAG_NOT_MATCH_CONTENT_NAME, PQUERY_FLAG_NOT_MATCH_CONTENT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT(PQUERY_FLAG_MATCH_CONTENT_NAME, PQUERY_FLAG_MATCH_CONTENT, CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT(PQUERY_FLAG_MATCH_HREF_SRC_NAME, PQUERY_FLAG_MATCH_HREF_SRC, CONST_PERSISTENT);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(pquery)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(pquery)
{
#if defined(COMPILE_DL_PQUERY) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(pquery)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pquery)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "pquery support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ pquery_functions[]
 *
 * Every user visible function must have an entry in pquery_functions[].
 */
const zend_function_entry pquery_functions[] = {
	PHP_FE(confirm_pquery_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in pquery_functions[] */
};
/* }}} */

static zend_module_dep pquery_deps[] = {
	{NULL, NULL, NULL}
};

/* {{{ pquery_module_entry
 */
zend_module_entry pquery_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	pquery_deps,
	"pquery",
	pquery_functions,
	PHP_MINIT(pquery),
	PHP_MSHUTDOWN(pquery),
	PHP_RINIT(pquery),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(pquery),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(pquery),
	PHP_PQUERY_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/*
ZEND_DLEXPORT zend_module_entry *get_module()
{
	pquery_module_entry.deps = pquery_deps;
	return &pquery_module_entry;
}
*/
#ifdef COMPILE_DL_PQUERY
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(pquery)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
