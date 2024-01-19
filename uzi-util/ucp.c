
/**************************************************
UZI (Unix Z80 Implementation) Utilities:  ucp.c
***************************************************/


#include <stdio.h>
#include "unix.h";
#include "extern.h"

int16  *syserror = &udata.u_error;
static char cwd[100];
static char line[128];

#ifndef CPM
extern char *sys_errlist[];
#endif


main(argc,argval)
int argc;
char *argval[];
{
    int rdev;
    char cmd[30], arg1[30], arg2[30], arg3[30];
    int count;
    struct filesys fsys;
    int j;
    char *argv[5];

    if (argc < 2)
	rdev = 0;
    else
	rdev = atoi(argval[1]);

    xfs_init(rdev);
    strcpy(cwd,"/");

    for(;;)
    {
	printf("unix: ");
	if (gets(line) == NULL)
	{
	    xfs_end();
	    exit();
	}
	cmd[0] = '\0';
	*arg1 = '\0';
	arg2[0] = '\0';
	arg3[0] = '\0';
	count = sscanf(line,"%s %s %s %s",cmd, arg1, arg2, arg3);
	if (count == 0 || cmd[0] == '\0')
	    continue;

	_sync();

	if (strcmp(cmd,"\n") == 0)
	    continue;
	else if (strcmp(cmd,"exit") == 0)
	{
	    xfs_end();
	    exit();
	}
	else if (strcmp(cmd,"ls") == 0)
	{
	    if (*arg1 )
		ls(arg1);
	    else
		ls(".");
	}
	else if (strcmp(cmd,"cd") == 0)
	{
	    if (*arg1 )
	    {
		strcpy(cwd,arg1);
		if (_chdir(arg1) != 0)
		{
		    printf("cd: error number %d\n",*syserror);
		}
	    }
	}
	else if (strcmp(cmd,"mkdir") == 0)
	{
	    if (*arg1 )
		mkdir(arg1);
	}
	else if (strcmp(cmd,"mknod") == 0)
	{
	    if (*arg1  && *arg2 && *arg3 )
		mknod(arg1,arg2, arg3);
	}
	else if (strcmp(cmd,"chmod") == 0)
	{
	    if (*arg1 && *arg2 )
		chmod(arg1,arg2);
	}
	else if (strcmp(cmd,"get") == 0)
	{
            if (*arg1 )
		get(arg1,0);
	}
	else if (strcmp(cmd,"bget") == 0)
	{
            if (*arg1 )
		get(arg1,1);
	}
	else if (strcmp(cmd,"put") == 0)
	{
            if (*arg1 )
		put(arg1,0);
	}
	else if (strcmp(cmd,"bput") == 0)
	{
            if (*arg1 )
		put(arg1,1);
	}
	else if (strcmp(cmd,"type") == 0)
	{
            if (*arg1 )
		type(arg1);
	}
	else if (strcmp(cmd,"dump") == 0)
	{
            if (*arg1 )
		fdump(arg1);
	}
	else if (strcmp(cmd,"rm") == 0)
	{
            if (*arg1 )
		unlink(arg1);
	}
	else if (strcmp(cmd,"df") == 0)
	{
	    for (j=0; j < 4; ++j)
	    {
		_getfsys(j,&fsys);
		if (fsys.s_mounted)
		{
		    printf("%d:   %u blks used %u blks free %u inodes used %u inodes free\n",
		    j,(fsys.s_fsize - fsys.s_isize)-fsys.s_tfree, fsys.s_tfree,
		    (8*(fsys.s_isize-2)-fsys.s_tinode), fsys.s_tinode);
		}
	    }
	}
	else if (strcmp(cmd,"rmdir") == 0)
	{
            if (*arg1 )
		rmdir(arg1);
	}
	else if (strcmp(cmd,"mount") == 0)
	{
	    if (*arg1 && *arg2)
		if (_mount(arg1,arg2,0) != 0)
		{
		    printf("Mount error.%d\n", *syserror);
		}
	}
	else if (strcmp(cmd,"umount") == 0)
	{
	    if (*arg1)
		if (_umount(arg1) != 0)
		{
		    printf("Umount error.%d\n", *syserror);
		}
	}
	else
	{
	    printf("Unknown command\n");
	}
    }
}



ls(path)
char *path;
{
    struct direct buf;
    struct stat statbuf;
    char dname[128];
    int d;
    
    d = _open(path,0);
    if (d < 0)
    {
	printf("ls: can't open %s\n",path);
	return;
    }

    while (_read(d,(char *)&buf,16) == 16)
    {
	if (buf.d_name[0] == '\0')
	    continue;

	if (path[0] != '.' || path[1] )
	{
	    strcpy(dname,path);
	    strcat(dname,"/");
	}
	else
	    dname[0] = '\0';
	strcat(dname,buf.d_name);

	if (_stat(dname,&statbuf) != 0)
	{
	    printf("ls: can't stat %s\n",dname);
	    continue;
	}

	if ((statbuf.st_mode & F_MASK) == F_DIR)
	    strcat(dname,"/");

	printf("%-6d %-15s",
		(statbuf.st_mode & F_CDEV) ?
		    statbuf.st_rdev :
		    512*statbuf.st_size.o_blkno + statbuf.st_size.o_offset,
		dname);

	printf("  0%-6o %-2d %-5d\n", statbuf.st_mode, statbuf.st_nlink,
		statbuf.st_ino);
    }
    _close(d);
}


chmod(path,modes)
char *path;
char *modes;
{
    int mode;
    int dev;

    mode = -1;
    sscanf(modes,"%o",&mode);
    if (mode == -1 )
    {
	printf("chmod: bad mode\n");
	return(-1);
    }

    if (_chmod(path,mode))
    {
	printf("_chmod: error %d\n",*syserror);
	return(-1);
    }

}


mknod(path,modes, devs)
char *path;
char *modes;
char *devs;
{
    int mode;
    int dev;

    mode = -1;
    sscanf(modes,"%o",&mode);
    if (mode == -1 )
    {
	printf("mknod: bad mode\n");
	return(-1);
    }
    
    if ((mode & F_MASK) != F_BDEV && (mode & F_MASK) != F_CDEV)
    {
	printf("mknod: mode is not device\n");
	return(-1);
    }

    dev = -1;
    sscanf(devs,"%d",&dev);
    if (dev == -1 )
    {
	printf("mknod: bad device\n");
	return(-1);
    }

    if (_mknod(path,mode,dev) != 0)
    {
	printf("_mknod: error %d\n",*syserror);
	return(-1);
    }

    return(0);
}



mkdir(path)
char *path;
{

    char dot[100];

    if ( _mknod(path, 040000 | 0777, 0) != 0)
    {
	printf("mkdir: mknod error %d\n",*syserror);
	return(-1);
    }

    strcpy(dot,path);
    strcat(dot,"/.");
    if ( _link(path,dot) != 0)
    {
	printf("mkdir: link dot error %d\n",*syserror);
	return(-1);
    }

    strcpy(dot,path);
    strcat(dot,"/..");
    if ( _link(".",dot) != 0)
    {
	printf("mkdir: link dotdot error %d\n",*syserror);
	return(-1);
    }

    return(0);
}




get(arg,binflag)
char *arg;
int binflag;
{
    FILE *fp;
    int d;
    char cbuf[512];
    int nread;

    fp = fopen(arg,binflag ? "rb" : "r");
    if (fp == NULL)
    {
	printf("Source file not found\n");
	return(-1);
    }

    d = _creat(arg,0666);
    if (d < 0)
    {
	printf("Cant open unix file error %d\n",*syserror);
   	return(-1);
    }
    
    for (;;)
    {
        nread = fread(cbuf,1,512,fp);
        if (nread == 0)
	    break;
        if (_write(d,cbuf,nread) != nread)
        {
	    printf("_write error %d\n",*syserror);
	    fclose(fp);
	    _close(d);
	    return(-1);
        }
    }
    fclose(fp);
    _close(d);
    return(0);
}


put(arg,binflag)
char *arg;
int binflag;
{
    FILE *fp;
    int d;
    char cbuf[512];
    int nread;

    fp = fopen(arg,binflag ? "wb" : "w");
    if (fp == NULL)
    {
	printf("Cant open destination file.\n");
	return(-1);
    }

    d = _open(arg,0);
    if (d < 0)
    {
	printf("Cant open unix file error %d\n",*syserror);
   	return(-1);
    }
    
    for (;;)
    {
        if ((nread =_read(d,cbuf,512)) == 0)
	    break;
        if (fwrite(cbuf,1,nread,fp) != nread)
        {
	    printf("fwrite error");
	    fclose(fp);
	    _close(d);
	    return(-1);
        }
    }
    fclose(fp);
    _close(d);
    return(0);
}


type(arg)
char *arg;
{
    int d;
    char cbuf[512];
    int nread;

    d = _open(arg,0);
    if (d < 0)
    {
	printf("Cant open unix file error %d\n",*syserror);
   	return(-1);
    }

    for (;;)
    {
	if( (nread = _read(d,cbuf,512)) == 0)
	    break;

	fwrite(cbuf, 1, nread, stdout);
    }
    _close(d);
    return(0);
}


fdump(arg)
char *arg;
{
    int d;
    char cbuf[512];
    int nread;

    printf("Dump starting.\n");
    d = _open(arg,0);
    if (d < 0)
    {
	printf("Cant open unix file error %d\n",*syserror);
   	return(-1);
    }

    for (;;)
    {
	if( (nread = _read(d,cbuf,512)) == 0)
	    break;
    }
    _close(d);
    printf("Dump done.\n");
    return(0);
}




unlink(path)
char *path;
{
    struct stat statbuf;

    if (_stat(path,&statbuf) != 0)
    {
	printf("unlink: can't stat %s\n",path);
	return(-1);
    }

    if ((statbuf.st_mode & F_MASK) == F_DIR)
    {
	printf("unlink: %s directory\n",path);
	return(-1);
    }

    if (_unlink(path) != 0)
    {
	printf("unlink: _unlink errn=or %d\n",*syserror);
	return(-1);
    }

    return(0);
}


rmdir(path)
char *path;
{
    struct stat statbuf;
    char newpath[100];
    struct direct dir;
    int fd;

    if (_stat(path,&statbuf) != 0)
    {
	printf("rmdir: can't stat %s\n",path);
	return(-1);
    }

    if (statbuf.st_mode & F_DIR == 0)
    {
	printf("rmdir: %s not directory\n",path);
	return(-1);
    }

    if((fd = _open(path,0)) < 0) {
	    printf("rmdir: %s unreadable\n", path);
	    return(-1);
    }
    while(_read(fd, (char *)&dir, sizeof (dir)) == sizeof (dir)) {
	    if(dir.d_ino == 0) continue;
	    if(!strcmp(dir.d_name, ".") || !strcmp(dir.d_name, ".."))
		    continue;
	    printf("rmdir: %s not empty\n", path);
	    _close(fd);
	    return(-1);
    }
    _close(fd);

    strcpy(newpath,path);
    strcat(newpath,"/.");
    if (_unlink(newpath) != 0)
    {
	printf("rmdir: can't unlink \".\"  error %d\n",*syserror);
	return(-1);
    }

    strcat(newpath,".");
    if (_unlink(newpath) != 0)
    {
	printf("rmdir: can't unlink \"..\"  error %d\n",*syserror);
	return(-1);
    }

    if (_unlink(path) != 0)
    {
	printf("rmdir: _unlink error %d\n",*syserror);
	return(-1);
    }

    return(0);
}

