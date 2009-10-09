/*
 * strrpl.c -- replace sub-strings
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
#include <stdlib.h>

char *
strrpl(const char *s, const char *sep, const char *exp)
{
    int c = 0;
    char *p, *q, *r;
    
    p = (char *) s;
    
    while ((p = strstr(p, sep))) {
	c++;
	p += strlen(sep);
    }

    q = r = malloc(strlen(s) - (c * strlen(sep)) + (c * strlen(exp)));
    
    for (p = (char *) s; *p;) {
	if (*p == *sep && strstr(p, sep)) {
	    strcat(q, exp);
	    p += strlen(sep);
	    q += strlen(exp);
	} else {
	    *q++ = *p++;
	}
    }
    
    return r;
}

int 
main(int argc, char **argv)
{
    if (argc != 4) {
	printf("usage: strrpl input from to\n");
	exit(1);
    }
    
    puts(strrpl(argv[1], argv[2], argv[3]));
    return 0;
}
