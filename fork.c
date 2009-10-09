/* 
 * fork.c -- utilizzo della funzione fork
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
 * I numeri identificativi dei processi
 * ------------------------------------
 * 
 * Ogni programma in esecuzione in ambiente Linux e' considerato un 
 * processo. A ogni processo e' associato un numero intero maggiore di
 * zero, detto ID di processo, o anche detto PID (dall'inglese, 
 * Process ID). Numerare i processi serve a semplicare la loro gestione 
 * dato che ogni PID identifica univocamente un processo (e' il kernel
 * di Linux che si assicura che ogni nuovo processo abbia un ID univoco 
 * non utilizzato). 
 *
 * Su Linux esistono particolari PID che hanno un significato speciale. 
 * Essi sono i PID 1, 2 e 3. Vediamo in dettaglio a quali processi si 
 * riferiscono questi PID. 
 *
 * Il PID 1 si riferisce sempre al processo init. Esso e' il padre di tutti
 * gli altri processi, e se, in un certo momento, il padre di un certo processo
 * viene terminato, il processo figlio esistente viene "adottato" dal processo 
 * init. Quindi il processo init diventa anche il padre *diretto* di tutti 
 * i processi orfani.
 *
 * Il PID 2, invece, si riferisce al demone kflushd che si occupa di svuotare 
 * la cache del disco. L'ultimo PID con un significato speciale e' il 3 che e'
 * associato al demone kswapd. Questo demone si occupa di trasferire, quando
 * richiesto, un pezzo di memoria su disco (piu' precisamente nella partizione
 * di swap).
 * 
 * Quindi nessuno dei vostri processi avra' mai il PID pari a 1, 2 o 3. Il PID
 * tra l'altro e' rappresentato dal tipo pid_t (che su macchine a 32 bit e' un
 * intero a 32 bit, con segno). Ma cosa succede quando il kernel arriva fino 
 * al valore massimo rappresentabile da un intero? Semplicemente si ricomincia
 * la numerazione dal primo PID disponibile.
 *
 * 
 * Processi genitore e figlio
 * --------------------------
 *
 * Tutti i processi in esecuzione su una macchina Linux sono stati creati a 
 * partire da un altro processo, ad eccezione del processo init. Se avete 
 * i sorgenti di Linux potrete constatare voi stessi che il processo di init
 * non e' generato per mezzo di fork():
 *     
 *      (init/main.c)
 *      ...
 *      execve("/sbin/init",argv_init,envp_init);
 *      execve("/etc/init",argv_init,envp_init);
 *      execve("/bin/init",argv_init,envp_init);
 *      execve("/bin/sh",argv_init,envp_init);
 *      panic("No init found.  Try passing init= option to kernel.");
 *      ...
 *
 * Quindi quando si crea un processo, viene considerato come padre 
 * (parent process) quello di partenza. Il nuovo processo creato viene 
 * definito processo figlio (child process).  
 * 
 * 
 * Creazione di un processo
 * ------------------------
 *
 * Una delle differenze piu' evidenti tra UNIX e altri sistemi operativi
 * e' il modo in cui vengono creati nuovi processi. Altri sistemi operativi
 * creano un nuovo processo a partire da zero, allocando spazio in memoria
 * e caricando un programma nella memoria appena allocata. UNIX invece non
 * crea mai un processo dal nulla: tutti i processi vengono creati per 
 * clonazione a partire da un processo esistente. Il termine "fork" sta 
 * proprio ad indicare la biforcazione di una strada (i.e. strada = processo). 
 *
 * Su Linux tutti (o meglio *quasi*) i processi vengono creati utilizzando
 * la funzione fork(). Esso fa' il suo lavoro secondo questa sequenza:
 *
 *    1. Il processo padre chiama fork() aspettandosi la restituzione di un 
 *       nuovo PID. 
 *  
 *    2. Il kernel copia "virtualmente" tutta la memoria del processo genitore 
 *       nello spazio di indirizzamento del nuovo processo figlio. In questo
 *       modo, il processo figlio, e' una copia esatta (byte a byte) del 
 *       processo padre. Questa copia include:
 *          - descrittori dei file
 *          - la directory di lavoro corrente
 *          - il valore di umask
 *          - vincoli sulle risorse
 * 
 *    3. Vengono aggiornate alcune entita' del kernel specifiche per il 
 *       processo figlio:
 *          - gli viene assegnato il PID univoco
 *          - gli viene assegnato il PPID (Parent PID, PID del genitore)
 *
 *    4. La funzione fork() determina quale valore restituire ai due 
 *       processi:
 *          - al genitore viene restituito il PID del figlio 
 *          - al processo figlio viene restituito 0 
 * 
 *    5. Si ritorna dalla fork(). A questo punto sono in esecuzione due
 *       processi identici: uno chiamato padre e uno chiamato figlio.
 *
 * 
 * Nota
 * ----
 *
 * Quando nelle funzioni della famiglia printf() usate variabili dichiarate
 * come pid_t, ricordate sempre di convertire la variabile in un tipo 
 * primitivo corrispondente al tipo usato nel formattatore. In questo modo 
 * assicurete una maggiore portabilita' ai vostri programmi rendendoli 
 * insensibili ad eventuali modifiche alla definizione del tipo pid_t.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>		/* per fork() e sleep() */
#include <string.h>		/* per strerror() */
#include <errno.h>		/* per errno */

#include <sys/types.h>		/* per pid_t */

int
main(void)
{
    pid_t pid;		/* PID del processo figlio */
    int sts;		/* Stato ritornato al sistema operativo */

    pid = fork();		/* Crea un nuovo processo figlio */

    if (pid == -1) {
	fprintf(stderr, "failed to fork(): %s\n", strerror(errno));
	sts = 1;
    } else if (pid == 0) {
	printf("PID %ld: Child started, parent is %ld.\n",
	       (long) getpid(), (long) getppid());
	sts = system("ps f");
    } else {
	printf("PID %ld: Started child PID %ld.\n", (long) getpid(),
	       (long) pid);
	sts = 0;
    }

    /* 
     * Spreca un po' di tempo per sincronizzare l'uscita del padre 
     * con l'uscita del figlio. Questo e' rozzamente equivalente 
     * a wait(). */
    sleep(1);
    return sts;
}

/*
 * Local Variables:
 * ispell-local-dictionary: "italiano"
 * End
 */
