
/**************************************************
UZI (Unix Z80 Implementation) Utilities:  xdevtty.c
***************************************************/


#include "unix.h"
#include "extern.h"

#define LINESIZ 128

static char line[LINESIZ];

tty_read(minor, rawflag)
int16 minor;
int rawflag;
{
    int nread;

    line[0] = udata.u_count;
    line[1] = 0;
    bdos(10,line);  /* Read console buffer */
    bdos(2,'\n');
    nread = line[1];
    line[nread+2] = '\n';
    bcopy(line+2,udata.u_base,nread+1);
    return(nread+1);
}


tty_write(minor, rawflag)
int16 minor;
int rawflag;
{
    while (udata.u_count-- != 0)
    {
        if (*udata.u_base=='\n')
	    bdos(2,'\r');
        bdos(2,*udata.u_base);
	++udata.u_base;
    }
}


_putc(c)
int c;
{
    bdos(2,c);
}


tty_open(minor)
int minor;
{
    return(0);
}


tty_close(minor)
int minor;
{
    return(0);
}


tty_ioctl(minor)
int minor;
{
    return(-1);
}
