/***************************************
 $Revision: 1.18 $

 Utilities (ut). memwrap.c - memory allocation wrappers. 
 Facilitate easy changing a memory allocation
 library and provide uniform error codes.

 Status: NOT REVUED, TESTED, 

 Design and implementation by: Marek Bukowy

 ******************//******************
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

#include "rip.h"
#include <stdlib.h>
#include <glib.h>

/* undefine our wrappers so we can use the real functions */
#undef UT_malloc
#undef UT_calloc
#undef UT_realloc
#undef UT_free
#undef UT_strdup
#undef wr_malloc
#undef wr_calloc
#undef wr_realloc
#undef wr_free
#undef wr_string
#undef wr_clear_list

/* logging */
extern LG_context_t *ut_context;

void * UT_malloc_real(size_t size, const char *file, int line) {
	void *ptr;

	ptr = malloc(size);
	if (ptr == NULL) {
		LG_log(ut_context, LG_SEVERE, "malloc(%u) out of memory at %s:%d", size, file, line);
		die;
	}
	return ptr;
}

void * UT_calloc_real(size_t num, size_t size, const char *file, int line) {
	void *ptr;

	ptr = calloc(num, size);
	if (ptr == NULL) {
		LG_log(ut_context, LG_SEVERE, "calloc(%u, %u) out of memory at %s:%d", num, size, file, line);
		die;
	}
	return ptr;
}

void * UT_realloc_real(void *ptr, size_t size, const char *file, int line) {
	char *tmp_ptr;

	tmp_ptr = realloc(ptr, size);
	if (tmp_ptr == NULL) {
		LG_log(ut_context, LG_SEVERE, "realloc(%p, %u) out of memory at %s:%d", ptr, size, file, line);
		die;
	}
	return tmp_ptr;
}

void UT_free_real(void *ptr, const char *file, int line) {
	free(ptr);
}

char * UT_strdup_real(const char *str, const char *file, int line) {
	char *area;

	area = UT_malloc_real(strlen(str) + 1, file, line);
	strcpy(area, str);

	return area;
}

/* for GList's foreach */
static void wr_free_list_element(void *cpy, void *trash) {
	free(cpy);
}

/* free a GList, calling free() on all data elements, then freeing the GList itself, finally
 * setting its value to NULL */
void wr_clear_list(GList **list) {
	/* allow NULL argument */
	if ( *list != NULL) {
		g_list_foreach(*list, wr_free_list_element, NULL);
		g_list_free(*list);
		*list = NULL;
	}
}

