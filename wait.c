/*
 * wait.c -- utilizzo della funzione wait
 * Copyright (C) 2004-2006, Davide Angelocola <davide.angelocola@gmail.com>
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

/*
 * La famiglia di funzioni wait
 * ----------------------------
 *
 * Nel precedente programma (fork.c) ho cercato di imitare il comportamento 
 * di wait() usando sleep(). In questo modo viene assicurato che il processo
 * padre termini dopo il figlio. Chiaramente un tale approccio risulta sia 
 * poco efficiente poiche' si spreca un secondo, un'eternita', sia poco 
 * elegante. Come puo' allora un processo stabilire la conclusione di un 
 * processo figlio? E come e' possibile determinarne il codice di uscita?
 * 
 * La famiglia di funzioni wait() permette di risolvere in modo efficiente ed
 * elegante le due questioni. In particolare la funzione wait() permette ad 
 * un processo genitore di ottenere informazioni sui propri processi figli. 
 * 
 * Processi zombie
 * ---------------
 *
 * I processi zombie sono processi, che per qualsiasi ragione, si sono  
 * conclusi, mentre l'esecuzione del processo padre procede senza saperlo. 
 */

#include <stdio.h>
#include <stdlib.h>		/* per exit() */
#include <unistd.h>		/* per fork(), getpid(), getppid() */
#include <string.h>		/* per strerror() */
#include <errno.h>		/* per errno */
#include <sys/types.h>		/* per pid_t */

#include <sys/wait.h>		/* per wait() */

int
main(void)
{
    pid_t pid;		/* PID del processo figlio */
    pid_t wpid;		/* PID ritornato da wait() */
    int status;		/* Valore di uscita del figlio wait() */
    int sts;

    /* Crea un nuovo processo figlio */
    pid = fork();		

    if (pid == -1) {
	fprintf(stderr, "%s: Failed to fork()\n", strerror(errno));
	sts = 1;
    } else if (pid == 0) {
	printf("PID %ld: Child started, parent is %ld.\n",
	       (long) getpid(), (long) getppid());
	sts = system("ps f");
    } else {
	/* Processo padre */
	printf("PID %ld: Started child PID %ld.\n", (long) getpid(),
	       (long) pid);

	/* 
	 * Chiamata a wait(). L'esecuzione del processo corrente 
	 * (processo figlio) e' interrotta: status contiene il 
	 * valore di ritorno. 
	 */
	wpid = wait(&status);

	if (wpid == -1) {
	    fprintf(stderr, "%s: wait()\n", strerror(errno));
	    exit(EXIT_FAILURE);
	} else {
	    printf("Child PID %ld exited status %d\n",
		   (long) pid, status);
	}
    }

    return sts;
}

/*
 * Local Variables:
 * ispell-local-dictionary: "italiano"
 * End
 */
