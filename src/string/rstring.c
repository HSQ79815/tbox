/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		rstring.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "rstring.h"
#include "../libc/libc.h"
#include "../utils/utils.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_RSTRING_FMTD_SIZE 		(4096)
#else
# 	define TB_RSTRING_FMTD_SIZE 		(8192)
#endif

/* ////////////////////////////////////////////////////////////////////////
 * atomic
 */
static __tb_inline__ tb_rstring_data_t* tb_rstring_atomic_data_get(tb_rstring_t const* string)
{
	tb_check_return_val(string && string->data, TB_NULL);

	// FIXME: atomic
	return *(string->data);
}
static __tb_inline__ tb_void_t tb_rstring_atomic_data_set(tb_rstring_t* string, tb_rstring_data_t* data)
{
	tb_check_return(string && string->data);

	// FIXME: atomic
	string->data = data;
}
static __tb_inline__ tb_handle_t tb_rstring_atomic_mutx_get(tb_rstring_data_t const* data)
{
	tb_check_return_val(data, TB_NULL);

	// FIXME: atomic
	return data->mutx;
}
static __tb_inline__ tb_void_t tb_rstring_atomic_mutx_set(tb_rstring_data_t* data, tb_handle_t mutx)
{
	tb_check_return(data);

	// FIXME: atomic
	data->mutx = mutx;
}
static __tb_inline__ tb_size_t tb_rstring_atomic_refn_get(tb_rstring_data_t const* data)
{
	tb_check_return_val(data, TB_NULL);

	// FIXME: atomic
	return data->refn;
}
static __tb_inline__ tb_void_t tb_rstring_atomic_refn_set(tb_rstring_data_t* data, tb_size_t refn)
{
	tb_check_return(data);

	// FIXME: atomic
	data->refn = refn;
}
static __tb_inline__ tb_size_t tb_rstring_atomic_refn_inc(tb_rstring_data_t* data)
{
	tb_check_return_val(data, TB_NULL);

	// FIXME: atomic
	return ++data->refn;
}
static __tb_inline__ tb_size_t tb_rstring_atomic_refn_dec(tb_rstring_data_t* data)
{
	tb_check_return_val(data, TB_NULL);

	// FIXME: atomic
	return --data->refn;
}
/* ////////////////////////////////////////////////////////////////////////
 * init & exit
 */
tb_bool_t tb_rstring_init(tb_rstring_t* string)
{
	tb_assert_and_check_return_val(string, TB_FALSE);
	tb_memset(string, 0, sizeof(tb_rstring_t));
	return TB_TRUE;
}
tb_void_t tb_rstring_exit(tb_rstring_t* string)
{
	if (string)
	{
		// refn--
		tb_rstring_decr(string);

		// clear
		tb_memset(string, 0, sizeof(tb_rstring_t));
	}
}

/* ////////////////////////////////////////////////////////////////////////
 * accessors
 */
tb_char_t const* tb_rstring_cstr(tb_rstring_t const* string)
{
	tb_assert_and_check_return_val(string, TB_NULL);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, TB_NULL);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, TB_NULL);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// cstr
	tb_char_t const* s = data->pstr? tb_pstring_cstr(data->pstr) : TB_NULL;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return s;
}
tb_size_t tb_rstring_size(tb_rstring_t const* string)
{
	tb_assert_and_check_return_val(string, 0);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, 0);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), 0);

	// cstr
	tb_size_t n = data->pstr? tb_pstring_size(data->pstr) : 0;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), 0);

	return n;
}
tb_size_t tb_rstring_refn(tb_rstring_t const* string)
{	
	tb_assert_and_check_return_val(string, 0);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, 0);

	// get refn
	return tb_rstring_atomic_refn_get(data);
}

/* ////////////////////////////////////////////////////////////////////////
 * modifiors
 */
tb_void_t tb_rstring_clear(tb_rstring_t* string)
{
	tb_assert_and_check_return(string);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return(data);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return(mutx);

	// enter
	tb_check_return(tb_mutex_enter(mutx));

	// clear
	if (data->pstr) tb_pstring_clear(data->pstr);

	// leave
	tb_check_return(tb_mutex_leave(mutx));
}
tb_char_t const* tb_rstring_strip(tb_rstring_t* string, tb_size_t n)
{
	tb_assert_and_check_return_val(string, TB_NULL);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, TB_NULL);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, TB_NULL);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// strip
	tb_char_t const* s = data->pstr? tb_pstring_strip(data->pstr, n) : TB_NULL;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return s;
}
tb_size_t tb_rstring_incr(tb_rstring_t* string)
{	
	tb_assert_and_check_return_val(string, 0);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, 0);

	// refn++
	return tb_rstring_atomic_refn_inc(data);
}
tb_size_t tb_rstring_decr(tb_rstring_t* string)
{	
	tb_assert_and_check_return_val(string, 0);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, 0);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), 0);

	// not freed?
	tb_bool_t frmx = TB_FALSE;
	tb_size_t refn = data->refn;
	if (refn >= 1) 
	{
		// refn--, avoid double free
		data->refn = --refn;

		// free it
		if (!refn)
		{
			// free pstring
			tb_pstring_exit(data->pstr);

			// free shared data
			tb_free(data);

			// free mutex
			frmx = TB_TRUE;
		}
	}
	// \note: mutex maybe already been freed
	// else ;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), 0);

	// exit mutex
	if (frmx) tb_mutex_exit(mutx);

	// clear string
	if (!refn) string->data = TB_NULL;

	return refn;
}
/* ////////////////////////////////////////////////////////////////////////
 * enter & leave
 */
tb_bool_t tb_rstring_enter(tb_rstring_t const* string)
{
	tb_assert_and_check_return_val(string, TB_FALSE);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, TB_FALSE);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, TB_FALSE);

	// enter
	return tb_mutex_enter(mutx);
}
tb_bool_t tb_rstring_leave(tb_rstring_t const* string)
{
	tb_assert_and_check_return_val(string, TB_FALSE);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, TB_FALSE);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, TB_FALSE);

	// leave
	return tb_mutex_leave(mutx);
}

/* ////////////////////////////////////////////////////////////////////////
 * strchr
 */
tb_long_t tb_rstring_strchr(tb_rstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_assert_and_check_return_val(string, -1);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, -1);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// strchr
	tb_long_t r = data->pstr? tb_pstring_strchr(data->pstr, p, c) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}
tb_long_t tb_rstring_strichr(tb_rstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_assert_and_check_return_val(string, -1);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, -1);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// strichr
	tb_long_t r = data->pstr? tb_pstring_strichr(data->pstr, p, c) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * strrchr
 */
tb_long_t tb_rstring_strrchr(tb_rstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_assert_and_check_return_val(string, -1);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, -1);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// strrchr
	tb_long_t r = data->pstr? tb_pstring_strrchr(data->pstr, p, c) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}
tb_long_t tb_rstring_strirchr(tb_rstring_t const* string, tb_size_t p, tb_char_t c)
{
	tb_assert_and_check_return_val(string, -1);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, -1);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// strirchr
	tb_long_t r = data->pstr? tb_pstring_strirchr(data->pstr, p, c) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * strstr
 */
tb_long_t tb_rstring_strstr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string, -1);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, -1);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// strstr
	tb_long_t r = data->pstr? tb_pstring_strstr(data->pstr, p, s) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}
tb_long_t tb_rstring_stristr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string, -1);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, -1);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// stristr
	tb_long_t r = data->pstr? tb_pstring_stristr(data->pstr, p, s) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}
tb_long_t tb_rstring_cstrstr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s2)
{
	tb_assert_and_check_return_val(string, -1);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, -1);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// cstrstr
	tb_long_t r = data->pstr? tb_pstring_cstrstr(data->pstr, p, s2) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}
tb_long_t tb_rstring_cstristr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s2)
{	
	tb_assert_and_check_return_val(string, -1);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, -1);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// cstristr
	tb_long_t r = data->pstr? tb_pstring_cstristr(data->pstr, p, s2) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * strrstr
 */
tb_long_t tb_rstring_strrstr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string, -1);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, -1);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// strrstr
	tb_long_t r = data->pstr? tb_pstring_strrstr(data->pstr, p, s) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}
tb_long_t tb_rstring_strirstr(tb_rstring_t const* string, tb_size_t p, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string, -1);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, -1);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// strirstr
	tb_long_t r = data->pstr? tb_pstring_strirstr(data->pstr, p, s) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}

tb_long_t tb_rstring_cstrrstr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s2)
{	
	tb_assert_and_check_return_val(string, -1);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, -1);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// cstrrstr
	tb_long_t r = data->pstr? tb_pstring_cstrrstr(data->pstr, p, s2) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}
tb_long_t tb_rstring_cstrirstr(tb_rstring_t const* string, tb_size_t p, tb_char_t const* s2)
{
	tb_assert_and_check_return_val(string, -1);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, -1);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, -1);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), -1);

	// cstrirstr
	tb_long_t r = data->pstr? tb_pstring_cstrirstr(data->pstr, p, s2) : -1;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), -1);

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * strcpy
 */
tb_char_t const* tb_rstring_strcpy(tb_rstring_t* string, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string && string != s, TB_NULL);

	// refn--
	tb_rstring_decr(string);

	// copy
	tb_memcpy(string, s, sizeof(tb_rstring_t));

	// refn++
	tb_rstring_incr(string);

	// ok
	return tb_rstring_cstr(string);
}
tb_char_t const* tb_rstring_cstrcpy(tb_rstring_t* string, tb_char_t const* s)
{
	tb_assert_and_check_return_val(s, TB_NULL);
	return tb_rstring_cstrncpy(string, s, tb_strlen(s));
}
tb_char_t const* tb_rstring_cstrncpy(tb_rstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string && s && n, TB_NULL);

	tb_trace_noimpl();
	return TB_NULL;
}
tb_char_t const* tb_rstring_cstrfcpy(tb_rstring_t* string, tb_char_t const* fmt, ...)
{
	tb_assert_and_check_return_val(string && fmt, TB_NULL);

	// format data
	tb_char_t p[TB_RSTRING_FMTD_SIZE] = {0};
	tb_size_t n = 0;
	tb_va_format(p, TB_RSTRING_FMTD_SIZE, fmt, &n);
	tb_assert_and_check_return_val(n, TB_NULL);
	
	return tb_rstring_cstrncpy(string, p, n);
}
/* ////////////////////////////////////////////////////////////////////////
 * chrcat
 */
tb_char_t const* tb_rstring_chrcat(tb_rstring_t* string, tb_char_t c)
{
	tb_assert_and_check_return_val(string, TB_NULL);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, TB_NULL);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, TB_NULL);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// cstrcat
	tb_char_t const* r = data->pstr? tb_pstring_chrcat(data->pstr, c) : TB_NULL;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return r;
}
tb_char_t const* tb_rstring_chrncat(tb_rstring_t* string, tb_char_t c, tb_size_t n)
{
	tb_assert_and_check_return_val(string && n, TB_NULL);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, TB_NULL);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, TB_NULL);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// cstrcat
	tb_char_t const* r = data->pstr? tb_pstring_chrncat(data->pstr, c, n) : TB_NULL;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return r;
}
/* ////////////////////////////////////////////////////////////////////////
 * strcat
 */
tb_char_t const* tb_rstring_strcat(tb_rstring_t* string, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(s, TB_NULL);
	return tb_rstring_cstrncat(string, s, tb_rstring_size(s));
}
tb_char_t const* tb_rstring_cstrcat(tb_rstring_t* string, tb_char_t const* s)
{
	tb_assert_and_check_return_val(s, TB_NULL);
	return tb_rstring_cstrncat(string, s, tb_strlen(s));
}
tb_char_t const* tb_rstring_cstrncat(tb_rstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string && s && n, TB_NULL);

	// copy it if null
	if (!tb_rstring_size(string)) return tb_rstring_cstrncpy(string, s, n);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, TB_NULL);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, TB_NULL);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// cstrcat
	tb_char_t const* r = data->pstr? tb_pstring_cstrncat(data->pstr, s, n) : TB_NULL;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return r;
}
tb_char_t const* tb_rstring_cstrfcat(tb_rstring_t* string, tb_char_t const* fmt, ...)
{
	tb_assert_and_check_return_val(string && fmt, TB_NULL);

	// format data
	tb_char_t p[TB_RSTRING_FMTD_SIZE] = {0};
	tb_size_t n = 0;
	tb_va_format(p, TB_RSTRING_FMTD_SIZE, fmt, &n);
	tb_assert_and_check_return_val(n, TB_NULL);
	
	return tb_rstring_cstrncat(string, p, n);
}

/* ////////////////////////////////////////////////////////////////////////
 * strcmp
 */
tb_long_t tb_rstring_strcmp(tb_rstring_t* string, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string && s, 0);
	return tb_rstring_cstrncmp(string, s, tb_rstring_size(s));
}
tb_long_t tb_rstring_strimp(tb_rstring_t* string, tb_rstring_t const* s)
{
	tb_assert_and_check_return_val(string && s, 0);
	return tb_rstring_cstrnicmp(string, s, tb_rstring_size(s));
}
tb_long_t tb_rstring_cstrcmp(tb_rstring_t* string, tb_char_t const* s)
{
	tb_assert_and_check_return_val(string && s, 0);
	return tb_rstring_cstrncmp(string, s, tb_strlen(s));
}
tb_long_t tb_rstring_cstricmp(tb_rstring_t* string, tb_char_t const* s)
{
	tb_assert_and_check_return_val(string && s, 0);
	return tb_rstring_cstrnicmp(string, s, tb_strlen(s));
}
tb_long_t tb_rstring_cstrncmp(tb_rstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string, 0);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, 0);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), 0);

	// cstrncmp
	tb_long_t r = data->pstr? tb_pstring_cstrncmp(data->pstr, s, n) : 0;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), 0);

	return r;
}
tb_long_t tb_rstring_cstrnicmp(tb_rstring_t* string, tb_char_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(string, 0);

	// get data
	tb_rstring_data_t* data = tb_rstring_atomic_data_get(string);
	tb_check_return_val(data, 0);

	// get mutx
	tb_handle_t mutx = tb_rstring_atomic_mutx_get(data);
	tb_check_return_val(mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), 0);

	// cstrnicmp
	tb_long_t r = data->pstr? tb_pstring_cstrnicmp(data->pstr, s, n) : 0;

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), 0);

	return r;
}
