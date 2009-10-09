/*
 * exec.c -- utilizzo della funzione exec
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
 * La famiglia di funzioni exec()
 * ------------------------------
 *
 * Se l'unico modo disponibile di creare nuovi processi fosse fork(), 
 * sarebbe solamente possibile eseguire piu' copie dello stesso processo: 
 * l'uso di exec(), invece, consente di lanciare nuovi programmi. Fra'
 * breve vedremo perche' le funzioni fork() ed exec() si utilizzano insieme.
 * 
 * Questa famiglia di funzioni comprende:
 *
 *  int execve(const char *filename, char *const argv [], char *const envp[]);
 *  int execl(const char *path, const char *arg, ...);
 *  int execlp(const char *file, const char *arg, ...);
 *  int execle(const char *path, const char *arg  , ..., char *const envp[]); 
 *  int execv(const char *path, char *const argv[]);
 *  int execvp(const char *file, char *const argv[]);
 *  
 * La funzione base e' execve(). Le altre sono funzioni "wrapper", cioe' che 
 * hanno una diversa firma ma che, in pratica, fanno la stessa cosa. 
 *
 * Cosa fa' la funzione exec()?
 * ----------------------------
 *
 * 1. Mantenimento dell'ID di processo.
 * 
 * 2. Mantenimento dello spazio di indirizzamento del processo.
 * 
 * 3. Interruzione del processo attualmente in esecuzione.
 *
 * 4. Abbandono dell'immagine del programma corrente (che sara' sovrascritta).
 *
 * 5. Lo spazio di indirizzamento esistente in memoria viene occupato dal
 *    nuovo programma.
 *
 * 6. Il nuovo programma riceve il controllo e viene eseguita la *sua* 
 *    funzione main().
 * 
 *
 * Utilizzo congiunto di fork() e di exec()
 * ----------------------------------------
 *
 * Quando la shell GNU bash lancia un nuovo comando utilizza entrambe le 
 * funzioni, fork() e exec():
 *
 * - la funzione fork() crea un nuovo processo
 *
 * - il nuovo processo figlio lancia il comando tramite exec()
 * 
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
    pid_t wpid;		/* PID per wait() */
    int status;		/* Stato ritornato da wait() */

    /* Si crea il processo figlio */
    pid = fork();

    if (pid == -1) {
	/* La chiamata fork() e' fallita */
	fprintf(stderr, "%s: Failed to fork()\n", strerror(errno));
	exit(EXIT_FAILURE);
    } else if (pid == 0) {
	/* Questo e' il processo figlio */
	static char *argv[] = { "ps", "a", NULL };

	printf("PID %ld: Child started, parent is %ld.\n",
	       (long) getpid(), (long) getppid());

	/* 
	 * Chiama execv() per rimpiazzare il processo corrente con 
	 * /bin/ps. Se il programma ps non e' presente nella 
	 * directory /bin del tuo sistema, puoi cambiare il 
	 * percorso (assoluto) del primo argomento di execv().
	 */
	execv("/bin/ps", argv);
	/* 
	 * Se il controllo e' giunto qui allora si e' verificato
	 * un errore nella chiamata execv().  
	 */
	fprintf(stderr, "%s: execv()\n", strerror(errno));
    } else {
	/* Questo e' il processo padre */
	printf("PID %ld: Started child PID %ld.\n", (long) getpid(),
	       (long) pid);

	/* Aspettiamo che il processo figlio termini */
	wpid = wait(&status);	/* Child's exit status */

	if (wpid == -1) {
	    /* La chiamata a wait() e' fallita. */
	    fprintf(stderr, "%s: wait()\n", strerror(errno));
	    return 1;
	} else {
	    /* Il processo figlio e' terminato */
	    if (WIFEXITED(status)) {
		/* Uscita normale -- stampa dello stato */
		printf("Exited: $? = %d\n",
		       WEXITSTATUS(status));
	    } else if (WIFSIGNALED(status)) {
		/* 
		 * Uscita anormale -- il processo figlio 
		 * e' terminato a causa di un segnale 
		 */
		printf("Signal: %d%s\n",
		       WTERMSIG(status), WCOREDUMP(status)
		       ? " with core file." : "");
	    } else {
		/* Il processo e' stato fermato */
		printf("Stopped.\n");
	    }
	}
    }

    return 0;
}

/*
 * Local Variables:
 * ispell-local-dictionary: "italiano"
 * End
 */
