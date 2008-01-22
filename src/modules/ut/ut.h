/***************************************
 $Revision: 1.3 $

 SQL module (sq) - MySQL implementation of SQL driver.

 Status: NOT REVUED, NOT TESTED

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
#ifndef READ_UT
#define READ_UT

#ifdef __cplusplus
extern "C" {
#endif

void UT_init(LG_context_t *ctx);
void UT_sendmail(const char *to, const char *subject, const char *body);
void UT_alarm_operator(const char *title, const char *description);

#ifdef __cplusplus
}
#endif

#endif /* READ_UT */
