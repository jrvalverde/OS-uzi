
/**************************************************
UZI (Unix Z80 Implementation) Utilities:  xmachdep.c
***************************************************/


#include "unix.h"
#include "extern.h"


/* This is called at the very beginning to initialize everything. */
/* It is the equivalent of main() */

fs_init()
{
    inint = 0;
    udata.u_euid = 0;
    udata.u_insys = 1;
}


/* This checks to see if a user-suppled address is legitimate */
valadr(base,size)
char *base;
uint16 size;
{
    return(1);
}


/* This adds two tick counts together.
The t_time field holds up to one second of ticks,
while the t_date field counts minutes */

addtick(t1,t2)
time_t *t1, *t2;
{

    t1->t_time += t2->t_time;
    t1->t_date += t2->t_date;
    if (t1->t_time >= 60*TICKSPERSEC)
    {
	t1->t_time -= 60*TICKSPERSEC;
	++t1->t_date;
    }
}

incrtick(t)
time_t *t;
{
    if (++t->t_time == 60*TICKSPERSEC)
    {
	t->t_time = 0;
	++t->t_date;
    }
}



stkreset()
{
#asm 8080
	POP	H
	LXI	SP,udata?-2
	PCHL
#endasm
}


/* Port addresses of clock chip registers. */

#define SECS 0xe2
#define MINS 0xe3
#define HRS 0xe4
#define DAY 0xe6
#define MON 0xe7
#define YEAR 86

sttime()
{
    panic("Calling sttime");
}


rdtime(tloc)
time_t *tloc;
{
    tloc->t_time = (tread(SECS)>>1) | (tread(MINS)<<5) | (tread(HRS)<<11);
    tloc->t_date = tread(DAY) | (tread(MON)<<5) | (YEAR<<9);
}


/* Update global time of day */
rdtod()
{
    tod.t_time = (tread(SECS)>>1) | (tread(MINS)<<5) | (tread(HRS)<<11);
    tod.t_date = tread(DAY) | (tread(MON)<<5) | (YEAR<<9);
}


/* Read BCD clock register, convert to binary. */
tread(port)
uint16 port;
{
    int n;

    n = in(port);
    return ( 10*((n>>4)&0x0f) + (n&0x0f) );
}


/* Disable interrupts */
di()
{
#asm 8080
	DI	;disable interrupts
#endasm
}

/* Enable interrupts if we are not in service routine */
ei()
{
    if (inint)
	return;
    ;	/* Empty statement necessary to fool compiler */

#asm 8080
	EI	;disable interrupts
#endasm
}



/* This shifts an unsigned int right 8 places. */

shift8()
{
#asm 8080
	POP	D  	;ret addr
	POP	H
	MOV	L,H
	MVI	H,0
	MOV	A,L
	ANA	A	;set Z flag on result
	PUSH	H
	PUSH	D	;restore stack
#endasm
}


/* This prints an error message and dies. */

panic(s)
char *s;
{
    inint = 1;
    kprintf("PANIC: %s\n",s);
    idump();
    abort();
}


warning(s)
char *s;
{
    kprintf("WARNING: %s\n",s);
}


kputs(s)
char *s;
{
    while (*s)
	kputchar(*(s++));
}

kputchar(c)
int c;
{
    if (c == '\n')
	_putc('\r');
    _putc(c);
}



idump()
{
    inoptr ip;
    ptptr pp;
    extern struct cinode i_tab[];
    bufptr j;

    kprintf(
        "   MAGIC  DEV  NUM  MODE  NLINK (DEV) REFS DIRTY err %d root %d\n",
            udata.u_error, root - i_tab);

    for (ip=i_tab; ip < i_tab+ITABSIZE; ++ip)
    {
	kprintf("%d %d %d %u 0%o %d %d %d %d\n",
	       ip-i_tab, ip->c_magic,ip->c_dev, ip->c_num,
	       ip->c_node.i_mode,ip->c_node.i_nlink,ip->c_node.i_addr[0],
	       ip->c_refs,ip->c_dirty);
	ifnot (ip->c_magic)	
	    break;
    }

    kprintf("\n   STAT WAIT   PID PPTR  ALARM  PENDING  IGNORED\n");
    for (pp=ptab; pp < ptab+PTABSIZE; ++pp)
    {
	kprintf("%d %d    0x%x %d %d  %d 0x%x 0x%x\n",
	       pp-ptab, pp->p_status, pp->p_wait,  pp->p_pid,
	       pp->p_pptr-ptab, pp->p_alarm, pp->p_pending,
		pp->p_ignored);
        ifnot(pp->p_pptr)
	    break;
    }	
    
    kprintf("\ndev blk drty bsy\n");
    for (j=bufpool; j < bufpool+NBUFS; ++j)
	kprintf("%d %u %d %d\n",j->bf_dev,j->bf_blk,j->bf_dirty,j->bf_busy);

    kprintf("\ninsys %d ptab %d call %d cwd %d sp 0x%x\n",
	udata.u_insys,udata.u_ptab-ptab, udata.u_callno, udata.u_cwd-i_tab,
       udata.u_sp);
}



/* Short version of printf to save space */
kprintf(nargs)
	{
	register char **arg, *fmt;
	register c, base;
	char s[7], *itob();

	arg = (char **)&nargs + nargs;
	fmt = *arg;
	while (c = *fmt++) {
		if (c != '%') {
			kputchar(c);
			continue;
			}
		switch (c = *fmt++) {
		case 'c':
			kputchar(*--arg);
			continue;
		case 'd':
			base = -10;
			goto prt;
		case 'o':
			base = 8;
			goto prt;
		case 'u':
			base = 10;
			goto prt;
		case 'x':
			base = 16;
		prt:
			kputs(itob(*--arg, s, base));
			continue;
		case 's':
			kputs(*--arg);
			continue;
		default:
			kputchar(c);
			continue;
			}
		}
	}
