/*
 * gegethostbyname_r.c -- gethostbyname_r() demo
 * Copyright (C) 2006, Davide Angelocola <davide.angelocola@gmail.com>
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation; either version 2 
 * of the License, or any later version. 
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, 
 * USA. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int
main(int argc, char **argv)
{
    struct hostent host, *hp;
    size_t len;
    char *buf;
    int err;
    int sts;
    int tryagain = 0;

    if (argc != 2) {
	printf("usage: %s hostname\n", argv[0]);
	return EXIT_FAILURE;
    }

    /* Allocate buffer. */
    len = 16;
    buf = malloc(len);

    if (buf == NULL) {
	fprintf(stderr, "no memory\n");
	abort();
    }

  again:
    /* Call gethostbyname_r. */
    err = gethostbyname_r(argv[1], &host, buf, len, &hp, &sts);
	
    switch (err) {
    case 0:
	if (hp == NULL) {
	    /* XXX: res is 0 on GNU libc. */
	    fprintf(stderr, "unknown host `%s'\n",
		    argv[1]);
	    goto fini;
	} else {
	    break;
	}
	
#ifdef NO_ADDRESS
    case NO_ADDRESS:
#elif defined(NO_DATA)
    case NO_DATA:
#endif
	fprintf(stderr,
		"host `%s' found but it doesn't have "
		"an address\n", argv[1]);
	break;

    case TRY_AGAIN:
	if (tryagain++ < 5) {
	    fprintf(stdout,
		    "name server could not be "
		    "contacted. Trying again...\n");
	    usleep(1000000);
	    goto again;
	} else {
	    fprintf(stderr,
		    "name server could not be "
		    "contacted.\n");
	    break;
	}

    case ERANGE:
	/* Enlarge the buffer. */
	len *= 2;
	buf = realloc(buf, len);

	if (buf == NULL) {
	    fprintf(stderr, "no memory\n");
	    abort();
	} else {
	    /* Call gethostbyname_r() again. */
	    goto again;
	}

    case NO_RECOVERY:
	fprintf(stderr,
		"a non-recoverable error occurred\n");
	/* Fall through to default. */

    default:
	break;
    }

    if (!err) {
	char **p;

	printf("Information on host `%s' (%s)\n", argv[1],
	       host.h_addrtype == PF_INET ? "IPv4" :
	       host.h_addrtype == PF_INET6 ? "IPv6" : "UNKNOWN");
	printf("official name: %s\n", hp->h_name);
	printf("aliases:\n");

	for (p = hp->h_aliases; *p != NULL; p++)
	    printf("  %s\n", *p);
		
	printf("addresses:\n");

	for (p = hp->h_addr_list; *p != NULL; p++) {
	    struct in_addr sin;

	    /* Getting IPv4 address. (no IPv6 yet) */
	    memcpy(&sin, *p, sizeof(struct in_addr));
	    printf("  %s\n", inet_ntoa(sin));
	}
    }

  fini:
    free(buf);
    return (err == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
