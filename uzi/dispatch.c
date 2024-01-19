/**************************************************
UZI (Unix Z80 Implementation) Kernel:  dispatch.c
***************************************************/


/* Dispatch table for system calls */
int     __exit(),
        _open(),
        _close(),
        _creat(),
        _mknod(),
        _link(),
        _unlink(),
        _read(),
        _write(),
        _seek(),
        _chdir(),
        _sync(),
        _access(),
        _chmod(),
        _chown(),
        _stat(),
        _fstat(),
        _dup(),
        _getpid(),
        _getppid(),
        _getuid(),
        _umask(),
        _getfsys(),
        _execve(),
        _wait(),
        _setuid(),
        _setgid(),
        _time(),
        _stime(),
        _ioctl(),
        _brk(),
        _sbrk(),
        _fork(),
        _mount(),
        _umount(),
        _signal(),
        _dup2(),
        _pause(),
        _alarm(),
        _kill(),
        _pipe(),
        _getgid(),
        _times();

int (*disp_tab[])() =
{       __exit,
        _open,
        _close,
        _creat,
        _mknod,
        _link,
        _unlink,
        _read,
        _write,
        _seek,
        _chdir,
        _sync,
        _access,
        _chmod,
        _chown,
        _stat,
        _fstat,
        _dup,
        _getpid,
        _getppid,
        _getuid,
        _umask,
        _getfsys,
        _execve,
        _wait,
        _setuid,
        _setgid,
        _time,
        _stime,
        _ioctl,
        _brk,
        _sbrk,
        _fork,
        _mount,
        _umount,
        _signal,
        _dup2,
        _pause,
        _alarm,
        _kill,
        _pipe,
        _getgid,
        _times
 };

char dtsize = sizeof(disp_tab) / sizeof(int(*)()) - 1;


