/***************************************
  $Revision: 1.15 $

  Utilities (ut). memwrap.h - header file for memory allocation wrappers. 

  Status: NOT REVUED, TESTED, 

  Design and implementation by: Marek Bukowy

  ******************/ /******************
  Copyright (c) 1999                              RIPE NCC
 
  All Rights Reserved
  
  Permission to use, copy, modify, and distribute this software and its
  documentation for any purpose and without fee is hereby granted,
  provided that the above copyright notice appear in all copies and that
  both that copyright notice and this permission notice appear in
  supporting documentation, and that the name of the author not be
  used in advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.
  
  THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS; IN NO EVENT SHALL
  AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
  DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
  ***************************************/

#ifndef _MEMWRAP_H
#define _MEMWRAP_H

/*#include <erroutines.h>*/
#include <glib.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void UT_memory_log(int active);

void *UT_malloc_real(size_t size, const char *file, int line);
void *UT_calloc_real(size_t num, size_t size, const char *file, int line);
void *UT_realloc_real(void *ptr, size_t size, const char *file, int line);
void UT_free_real(void *ptr, const char *file, int line);
char *UT_strdup_real(const char *str, const char *file, int line);

#define UT_malloc(size)       UT_malloc_real((size),        __FILE__, __LINE__)
#define UT_calloc(num,size)   UT_calloc_real((num),(size),  __FILE__, __LINE__)
#define UT_realloc(ptr,size)  UT_realloc_real((ptr),(size), __FILE__, __LINE__)
#define UT_free(ptr)          UT_free_real((ptr),           __FILE__, __LINE__)
#define UT_strdup(str)        UT_strdup_real((str),         __FILE__, __LINE__)

void wr_clear_list(GList **list);

#ifdef __cplusplus
}
#endif

#endif /* _MEMWRAP_H */
