/*
 * perm.c -- print file permission 
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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PERM(p,c)    (s.st_mode & (p)? (c) : '-')
#define CAN_READ(p)  PERM((p), 'r')
#define CAN_WRITE(p) PERM((p), 'w')
#define CAN_EXEC(p)  PERM((p), 'x')

#define FILE "/etc/profile"

int
main(void)
{
    struct stat s;
   
    if (stat(FILE, &s) != 0) {
	perror("stat");
	return 1;
    }

    /* as ls -l | awk '{ print $1; }' */
    printf("%c%c%c%c%c%c%c%c%c\n",
      CAN_READ(S_IRUSR), CAN_WRITE(S_IWUSR), CAN_EXEC(S_IXUSR),  /* owner */
      CAN_READ(S_IRGRP), CAN_WRITE(S_IWGRP), CAN_EXEC(S_IXGRP),  /* group */
      CAN_READ(S_IROTH), CAN_WRITE(S_IWOTH), CAN_EXEC(S_IXOTH)); /* other */

    return 0;
}

