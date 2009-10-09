/*
 * split.c -- split a string into tokens
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

char **
split(const char *string, char sep, int *argc)
{
    char **argv = NULL;
    char *buf, *p, *q; 
    int c = 1, i = 0;

    if (string == NULL) 
	return NULL;
    
    if (argc == NULL)
	return NULL; 

    buf = p = q = strdup(string);

    while ((p = strchr(buf, sep)) != NULL) {
	c++;
	buf = ++p;
    }
    
    buf = q;
    argv = malloc(c * sizeof(char *));
	
    while ((p = strchr(buf, sep)) != NULL) {
	*p++ = 0;
	argv[i++] = strdup(buf);
	buf = p;
    }

    /* Store last item. */
    argv[i] = strdup(buf);

    /* Cleanup. */
    free(q);
    
    /* Return values. */
    *argc = c;
    return argv;
}

int
main(void)
{
    int argc, i;
    char **argv, input[50], *p;
    
    (void) fgets(input, 50, stdin);
    
    p = strchr(input,'\n');

    if (p) {
	*p = 0;
    }
	
    argv = split(input, ' ', &argc);
    
    for (i = 0; i < argc; i++) {
	printf("argv[%d] = '%s'\n", i, argv[i]);
    }
    
    return 0;
}

