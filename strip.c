/*
 * strip.c -- strip leading and/or traling whitespaces 
 * Copyright (C) 2006, Davide Angelocola <davide.angelocola@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Strip mode. */
#define STRIP_TRAILING 0
#define STRIP_LEADING 1
#define STRIP_BOTH 2

/* Handy macros. */
#define strip(s) strip2((s), STRIP_BOTH)
#define chug(s) strip2((s), STRIP_TRAILING)
#define chomp(s) strip2((s), STRIP_LEADING)

char *
strip2(char *s, int how)
{
    char *p;

    if (!s) {
	return NULL;
    }
    
    /* Strip leading whitespaces. */
    if (how != STRIP_TRAILING) {
	for (p = s; *p && isspace(*p); p++)
	    ;			

	memmove(s, p, strlen(p) + 1);
    }

    /* Strip trailing whitespaces. */
    if (how != STRIP_LEADING) {
	for (p = s + strlen(s) - 1; p >= s && isspace(*p); p--)
	    *p = '\0';
    }

    return s;
}

int
main(void)
{
    char buf[] = "    a string      ";
    
    printf("strip = '%s'\n", strip(strdup(buf)));
    printf("chug = '%s'\n", chug(strdup(buf)));
    printf("chomp = '%s'\n", chomp(strdup(buf)));
    return 0;
}

