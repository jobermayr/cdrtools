/* @(#)mconfig.h	1.13 97/05/21 Copyright 1995 J. Schilling */
/*
 *	definitions for machine configuration
 *
 *	Copyright (c) 1995 J. Schilling
 *
 *	This file must be included before any other file.
 *	Use only cpp instructions.
 *
 *	NOTE: SING: (Schily Is Not Gnu)
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef	HAVE_CONFIG_H	/* XXX gnu autoconf stuff (not yet active...) */
#include <config.h>
#endif

#ifndef _MCONFIG_H
#define _MCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(unix) || defined(__unix) || defined(__unix__)
#	define	IS_UNIX
#endif

#ifdef	__MSDOS__
#	define	IS_MSDOS
#endif

#if defined(tos) || defined(__tos)
#	define	IS_TOS
#endif

#ifdef	THINK_C
#	define	IS_MAC
#endif

#if defined(sun) || defined(__sun) || defined(__sun__)
#	define	IS_SUN
#endif

/*
 * 1) Compiler and language related headers
 *
 * define PROTOTYPES	to use ansi C prototypes
 *
 * define HAVE_ELF	Object files are stored in System V.4 ELF format.
 *
 * define HAVE_COFF	Object files are stored in System V.2 COFF format.
 *
 * define HAVE_AOUT	Object files are stored in ATT/BSD a.out format.
 *
 * define HAVE_STDARG_H	to use stdarg.h, else use varargs.h
 *			NOTE: SaberC on a Sun has prototypes but no stdarg.h.
 *
 * define HAVE_STDLIB_H	to use general utility defines (malloc(), size_t ...)
 *			some C library prototypes my be found here
 *
 * define HAVE_STRING_H to get NULL and string function prototypes
 *
 * define HAVE_STDC_HEADERS to declare the presence of other ansi C headers
 * define STDC_HEADERS 	same as above (GNU name)
 *
 * define HAVE_LONGLONG	to use long long for space/transfer calculations
 *
 *
 * 2) Operating system related headers
 *
 * define HAVE_UNISTD_H	to use access, lockf and lseek constants
 *			syscall prototypes may be found here also
 *			use sys/file.h otherwise for above constants
 *
 * define HAVE_FCNTL_H	to use access, O_XXX constants for open()
 *			and open(), creat(), fcntl() prototypes
 *			use sys/file.h otherwise for above constants
 *
 * define HAVE_TERMIOS_H to use posix terminal and session control (termios.h)
 * define HAVE_TERMIO_H	to use SV terminal control (termio.h) *- no setpgrp -*
 *			Else use BSD style sgttyb and setpgrp (ioctl.h)
 *			XXX session control should be another define XXX
 *
 * define HAVE_UTIMES	to use BSD utimes() and sys/time.h
 * define HAVE_UTIME_H	to use utime.h for the utimbuf structure declaration
 *			Else declare struct utimbuf yourself.
 *
 * define HAVE_WAIT_H	to use wait.h for prototypes and union wait
 * define HAVE_SYS_WAIT_H else use sys/wait.h
 *			Else declare it by yourself.
 *
 * define HAVE_SYS_SYSTEMINFO_H to use sysinfo()
 * define HAVE_SYS_UTSNAME_H to use uname()
 *
 * define HAVE_SYS_PRIOCNTL_H to use priocntl() instead of nice()/setpriority()
 * define HAVE_SYS_RTPRIOCNTL_H if the system supports real time classes.
 *
 * define HAVE_SYS_MTIO_H to use mtio definitions from sys/mtio.h
 * define HAVE_MTGET_DSREG if struct mtget contains mt_dsreg (drive status)
 * define HAVE_MTGET_RESID if struct mtget contains mt_resid (residual count)
 * define HAVE_MTGET_FILENO if struct mtget contains mt_fileno (file #)
 * define HAVE_MTGET_BLKNO if struct mtget contains mt_blkno (block #0
 *
 * define MAJOR_IN_MKDEV if we should include sys/mkdev.h to get 
 *			major(), minor() and makedev()
 *
 * define MAJOR_IN_SYSMACROS if we should include sys/sysmacros.h to get 
 *			major(), minor() and makedev()
 *
 *			... else look in sys/types.h for major()
 *
 * 3) Miscellaneous operating system/library/processor related things
 *
 * define HAVE_USG_STDIO to enable the use USG stdio.h internals
 *			To to this we need:
 *			f->_flag & _IONBF	... Unbuffered
 *			f->_flag & _IOERR	... I/O error
 *			f->_flag & _IOEOF	... End of File
 *			f->_cnt			... r/w count in buf
 *			f->_ptr			... pointer into buf
 *			_filbuf(FILE * f)	... fill buffer, return 1st ch
 *			_flsbuf(unsigned char *, FILE * f) ... flush buffer
 *
 * define HAVE_STRERROR	may use strerror() instead of sys_errlist[] and sys_nerr
 *
 * define HAVE_MEMMOVE	should use memmove() instead of bcopy()
 *
 * define HAVE_MMAP	may map memory (sys/types.h + sys/mman.h)
 * define HAVE_SMMAP	may map anonymous memory to get shared mem
 *
 * define HAVE_USGSHM	may get shared memory SV style (sys/types.h + sys/ipc.h)
 *
 * define HAVE_USGSEM	may use SysV style shared memory and semaphores.
 *			May use shared memory and semaphores to manage a
 *			sharing buffer and its synchronization.
 *			If this is not defined, use mmap and flock.
 *
 * define HAVE_MSEM	Has OSF/1 style memory semaphores.
 *			Use instead of SysV semaphores or flock
 *			for shared memory synchronisation.
 *
 * define HAVE_LDSTUB	Has SPARC ldstub atomic instruction.
 *			May be used instead of system V semaphores or flock
 *			for shared memory synchronisation.
 *
 * define HAVE_XCHG	Has i386 xchg atomic instruction.
 *			May be used instead of system V semaphores or flock
 *			for shared memory synchronisation.
 *
 * define HAVE_FLOCK	Use flock for synchronization on logfiles.
 *			If this is not defined use lockf.
 *
 * define HAVE_FCHDIR	The fchdir system call may be used
 *			to change the current directory and back.
 *			Else remember the pathname and use chdir.
 *
 * define HAVE_STATVFS	The statvfs and fstatvfs calls are available.
 *			Else get filesystem statistics with
 *			statfs or getmnt (on ultrix).
 *
 * define HAVE_QUOTA	The quota or quotactl system calls are available.
 *
 * define HAVE_YP	To use yellow pages.
 *
 * define HAVE_SHADOW	To use shadow password file.
 *
 * define HAVE_SETREUID	have BSD setreuid()
 * define HAVE_SETRESUID have HPUX only ??? setresuid()
 * define HAVE_SETEUID	have SVr4 seteuid()
 *
 * define HAVE_LCHOWN	Need to use lchown() instead of chown() on symlinks.
 *
 * define HAVE_PROCFS	SVr4 style procfs is available.
 *
 * define HAVE_PROCFS2	SVr4.2 (SMP) style procfs is available.
 *
 * define HAVE_SIGINFO	Use waitid and the siginfo_t structure for waiting
 *			for child processes.
 *			Else use wait3 and union wait.
 *
 * define HAVE_WAIT3	Have wait3.
 *
 * define HAVE_WAITPID	Use waitpid and no resource usage instead of wait3.
 *
 * define HAVE_UNION_WAIT Have union wait in wait.h
 *
 * define HAVE_GETHOSTNAME to use gethostname()
 *
 * define HAVE_STREAMS	Use streams networking calls. Else use sockets.
 *
 * define HAVE_STRPTYS	Use SVr4 style streams pseudo ttys.
 *
 * define HAVE_POLL	Use the poll system call to wait for I/O.
 *			Else use select.
 *
 * define HAVE_SELECT	Use the select system call to wait for I/O.
 *
 * define HAVE_TIRPC	The remote procedure call library is of the
 *			transport independent flavour.
 *
 * define GID_T		The type to use for the getgroups() array.
 *			This should be gid_t, but some BSD based systems
 *			must have int there.
 *
 *
 * 4) Specials for libschily
 *
 * define HAVE_SCANSTACK Scanning of the stack is implemented for this
 *			architecture:
 *				getfp()
 *			and the derived functions:
 *				handlecond(), raisecond()
 *			are working.
 *
 * define HAVE_GETAV0	Scanning of stack and locating the arg vector
 *			is implemented for this architecture:
 *				getav0()
 *			is working.
 *			get_progname() in saveargs.c will work in the main
 *			thread without a prior call to save_args().
 *
 */

#if	defined(SOL2) || defined(SOL2) || defined(S5R4) || defined(__S5R4) \
							|| defined(SVR4)
#	ifndef	__SVR4
#		define	__SVR4
#	endif
#endif

#ifdef	__SVR4
#	ifndef	SVR4
#		define	SVR4
#	endif
#endif

/*
 * SunOS 4.x
 */
#if defined(IS_SUN) && !defined(__SVR4)
	/*
	 * Sun C defines __STDC__ as zero. 
	 */
#	ifdef __STDC__
#		define	PROTOTYPES
#		ifndef SABER
#			define	HAVE_STDARG_H
#			define	HAVE_LONGLONG
#		endif
#	endif
#	define	HAVE_AOUT
#	define	HAVE_STDLIB_H
#	define	HAVE_STRING_H
#	define	HAVE_UNISTD_H
#	define	HAVE_FCNTL_H
#	define	HAVE_TERMIOS_H
#	define	HAVE_TERMIO_H
#	define	HAVE_UTIMES
#	define	HAVE_UTIME_H
#	define	HAVE_SYS_WAIT_H
#	define	HAVE_SYS_UTSNAME_H
#	define	HAVE_SYS_MTIO_H
#	define	HAVE_MTGET_DSREG
#	define	HAVE_MTGET_RESID
#	define	HAVE_MTGET_FILENO
#	define	HAVE_MTGET_BLKNO
#	define	MAJOR_IN_SYSMACROS
#	define	HAVE_UNION_WAIT
#	define	HAVE_USG_STDIO
#	define	HAVE_MMAP
#	define	HAVE_SMMAP
#	define	HAVE_USGSHM
#	define	HAVE_USGSEM
#	ifdef sparc
#		define	HAVE_LDSTUB
#	endif
#	define	HAVE_FLOCK
#	define	HAVE_FCHDIR
#	define	HAVE_QUOTA
#	define	HAVE_YP
#	define	HAVE_SETREUID
#	define	HAVE_WAIT3
#	define	HAVE_GETHOSTNAME
#	define	HAVE_SELECT
#	define	GID_T		int
#	define	USLEEPRETURN_T	unsigned int
#	define	HAVE_GETAV0	/* SunOS < 5 only runs on sparc/mc680xx */
#	define	HAVE_SCANSTACK	/* SunOS < 5 only runs on sparc/mc680xx */
#endif

/*
 * AIX
 */
#if	defined(_IBMR2) || defined(_AIX)
#	define	IS_UNIX		/* ??? really ??? */
#ifndef	PROTOTYPES
#	define	PROTOTYPES
#endif
#	define	HAVE_COFF
#	define	HAVE_STDARG_H
#	define	HAVE_STDLIB_H
#	define	HAVE_STRING_H
#	define	HAVE_LONGLONG
#	define	HAVE_UNISTD_H
#	define	HAVE_FCNTL_H
#	define	HAVE_TERMIOS_H
#	define	HAVE_TERMIO_H
#	define	HAVE_UTIMES
#	define	HAVE_UTIME_H
#	define	HAVE_SYS_WAIT_H
#	define	HAVE_SYS_UTSNAME_H
#	define	MAJOR_IN_SYSMACROS
/*#	define	HAVE_USG_STDIO*/
#	define	HAVE_STRERROR
/*#	define	HAVE_MEMMOVE*/
#	define	HAVE_MMAP
#	define	HAVE_SMMAP
#	define	HAVE_USGSHM
#	define	HAVE_USGSEM
#	define	HAVE_MSEM
#	define	HAVE_FLOCK
#	define	HAVE_QUOTA
#	define	HAVE_YP
#	define	HAVE_WAIT3
#	define	HAVE_WAITPID
#	define	HAVE_GETHOSTNAME
#	define	HAVE_STREAMS
#	define	HAVE_POLL
#	define	HAVE_SELECT

#	define	GID_T		gid_t
#	define	USLEEPRETURN_T	int
#endif

/*
 * Silicon Graphics	(must be before SVR4)
 */
#if defined(sgi) || defined(__sgi)
#	define	__NOT_SVR4__	/* Not a real SVR4 implementation */

#	define	PROTOTYPES
#	define	HAVE_ELF
#	define	HAVE_COFF
#	define	HAVE_STDARG_H
#	define	HAVE_LONGLONG
#	define	HAVE_UNISTD_H
#	define	HAVE_FCNTL_H
#	define	HAVE_TERMIOS_H
#	define	HAVE_TERMIO_H
#	define	HAVE_SYS_TIME_H
#	define	HAVE_UTIMES
#	define	HAVE_UTIME_H
#	define	HAVE_WAIT_H
#	define	HAVE_SYS_WAIT_H
#	define	HAVE_SYS_SYSTEMINFO_H
#	define	HAVE_SYS_UTSNAME_H
#	define	HAVE_SYS_MTIO_H
#	define	HAVE_MTGET_DSREG
#	define	HAVE_MTGET_RESID
#	define	HAVE_MTGET_FILENO
#	define	HAVE_MTGET_BLKNO
#	define	MAJOR_IN_MKDEV
#	define	HAVE_USG_STDIO
#	define	HAVE_STRERROR
#	define	HAVE_MEMMOVE
#	define	HAVE_MMAP
#	define	HAVE_SMMAP
#	define	HAVE_USGSHM
#	define	HAVE_USGSEM
#	define	HAVE_FLOCK
#	define	HAVE_FCHDIR
#	define	HAVE_STATVFS
#	define	HAVE_QUOTA
#	define	HAVE_YP
#	define	HAVE_SHADOW
#	define	HAVE_PROCFS
#	define	HAVE_SIGINFO
#	define	HAVE_WAIT3
#	define	HAVE_WAITPID
#	define	HAVE_GETHOSTNAME
#	define	HAVE_STREAMS
#	define	HAVE_STRPTYS
#	define	HAVE_POLL
#	define	HAVE_SELECT
#	define	HAVE_STRPTYS
#	define	GID_T		gid_t
/*#	define	USLEEPRETURN_T	unsigned int*/
#	define	vfork		fork
#endif

#if defined(__osf__)
#	define	PROTOTYPES
#	define	HAVE_COFF
#	define	HAVE_STDARG_H
#	define	HAVE_LONGLONG
#	define	HAVE_UNISTD_H
#	define	HAVE_TERMIOS_H
#	define	HAVE_UTIME_H
#	define	HAVE_MSEM
#	define	HAVE_FLOCK
#	define	HAVE_FCHDIR
#	define	HAVE_QUOTA
#	define	HAVE_YP
#	define	HAVE_PROCFS
#	define	GID_T		gid_t
#	define	USLEEPRETURN_T	void
#	define	USLEEPISVOID
#endif

#if defined(mips) && !(defined(ultrix) || defined(sony) || defined(sgi))
#	define	HAVE_COFF
#	define	HAVE_UNISTD_H
#	define	HAVE_USGSHM
#	define	HAVE_USGSEM
#	define	HAVE_FLOCK
#	define	HAVE_YP
#	define	GID_T		int
#	define	USLEEPRETURN_T	unsigned int
#endif

#if defined(sony)
#	ifdef mips
#		define	HAVE_COFF
#	else
#		define	HAVE_AOUT
#	endif
#	define	HAVE_USGSHM
#	define	HAVE_USGSEM
#	define	HAVE_FLOCK
#	define	HAVE_QUOTA
#	define	HAVE_YP
#	define	GID_T		int
#	define	USLEEPRETURN_T	unsigned int
#	ifndef	SEEK_SET
#	define	SEEK_SET	0 /* Set file pointer to "offset" */
#	define	SEEK_CUR	1 /* Set file pointer to current plus "offset" */
#	define	SEEK_END	2 /* Set file pointer to EOF plus "offset" */
#	endif
#endif

#if defined(ultrix)
#	ifdef mips
#		define	HAVE_COFF
#	else
#		define	HAVE_AOUT
#	endif
#	define	HAVE_STDLIB_H
#	define	HAVE_UNISTD_H
#	define	HAVE_TERMIOS_H
#	define	HAVE_UTIME_H
#	define	HAVE_USGSHM
#	define	HAVE_USGSEM
#	define	HAVE_FLOCK
#	define	HAVE_QUOTA
#	define	HAVE_YP
#	define	GID_T		int
#	define	USLEEPRETURN_T	unsigned int
#endif

/*
 * HP/UX
 */
#if defined(__hpux) || defined(hpux)
/*#	define	PROTOTYPES*/
#	define	HAVE_AOUT
#	define	HAVE_STDARG_H
#	define	HAVE_UNISTD_H
#	define	HAVE_FCNTL_H
#	define	HAVE_TERMIOS_H
#	define	HAVE_TERMIO_H
#	define	HAVE_UTIME_H
#	define	HAVE_SYS_WAIT_H
#	define	HAVE_SYS_UTSNAME_H
#	define	HAVE_SYS_MTIO_H
#	define	MAJOR_IN_SYSMACROS
#	define	HAVE_STRERROR
#	define	HAVE_MEMMOVE
#	define	HAVE_MMAP
#	define	HAVE_SMMAP
#	define	HAVE_USGSHM
#	define	HAVE_USGSEM
#if	0
#		define	HAVE_MSEM
#	endif
#	define	HAVE_FCHDIR
#	define	HAVE_QUOTA
#	define	HAVE_YP
#	define	HAVE_SETRESUID
#	define	HAVE_WAIT3
#	define	HAVE_WAITPID
#	define	HAVE_GETHOSTNAME
#	define	HAVE_STREAMS
#	define	HAVE_STRPTYS
#	define	HAVE_POLL
#	define	HAVE_SELECT
#	define	GID_T		gid_t
/*#	define	USLEEPRETURN_T	unsigned int*/
#endif

/*
 * Data General
 */
#if defined(__DGUX__)
#	define	PROTOTYPES
#	define	HAVE_ELF
#	define	HAVE_STDARG_H
#	define	HAVE_UNISTD_H
#	define	HAVE_TERMIOS_H
#	define	HAVE_SYS_TIME_H
#	define	HAVE_UTIMES
#	define	HAVE_UTIME_H
#	undef	HAVE_MTGET_DSREG
#	undef	HAVE_MTGET_RESID
#	undef	HAVE_MTGET_FILENO
#	undef	HAVE_MTGET_BLKNO
#	define	mt_type		mt_model
#	define	mt_dsreg	mt_status1
#	define	mt_erreg	mt_status2
#	define	HAVE_STRERROR
#	define	HAVE_MEMMOVE
#	define	HAVE_USGSEM
#	if defined(__i386_) || defined(i386)
#		define	HAVE_XCHG
#	endif
	/*
	 * DGUX hides its flock as dg_flock.
	 */
#	define	HAVE_FLOCK
#	define	flock	dg_flock
#	define	HAVE_FCHDIR
#	define	HAVE_STATVFS
#	undef	HAVE_QUOTA
#	define	HAVE_YP
#	define	HAVE_SHADOW
#	undef	HAVE_PROCFS
#	undef	HAVE_PROCFS2
#	define	HAVE_WAIT3
#	define	HAVE_UNION_WAIT
/*#	define	HAVE_GETHOSTNAME*/
#	define	HAVE_STREAMS
#	define	HAVE_STRPTYS
#	define	HAVE_POLL
#	define	HAVE_SELECT
#	undef	HAVE_TIRPC
#	define	GID_T		gid_t
#	define	USLEEPRETURN_T	unsigned int
	/*
	 * Use the BSD style wait on DGUX to get the resource usages of child
	 * processes.
	 */
#	define	_BSD_WAIT_FLAVOR

/*#	define	HAVE_GETAV0*/
#	ifdef	i386
#		define	HAVE_SCANSTACK
#	endif
#endif

/*
 * Linux
 */
#if defined(O__linux__) || defined(O__linux)
#	define	PROTOTYPES
#	ifdef	__ELF__
#		define	HAVE_ELF
#	else
#		define	HAVE_AOUT
#	endif
#	define	HAVE_STDARG_H
#	define	HAVE_UNISTD_H
#	define	HAVE_TERMIOS_H
#	define	HAVE_UTIMES
#	define	HAVE_UTIME_H
#	define	HAVE_SYS_UTSNAME_H
#	define	HAVE_USGSHM
#	define	HAVE_USGSEM
#	define	GID_T		gid_t
#	define	USLEEPRETURN_T	void
#	define	USLEEPISVOID
#	define	F_ULOCK	0	/* Unlock a previously locked region */
#	define	F_LOCK	1	/* Lock a region for exclusive use */
#	define	F_TLOCK	2	/* Test and lock a region for exclusive use */
#	define	F_TEST	3	/* Test a region for other processes locks */
#endif

/*
 * Linux
 */
#if defined(__linux__) || defined(__linux)
#	define	PROTOTYPES
#	ifdef	__ELF__
#		define	HAVE_ELF
#	else
#		define	HAVE_AOUT
#	endif
#	define	HAVE_STDARG_H
#	define	HAVE_STDLIB_H
#	define	HAVE_STRING_H
#	define	HAVE_STDC_HEADERS
#	define	STDC_HEADERS	/* GNU name */
#	define	HAVE_LONGLONG
#	define	HAVE_UNISTD_H
#	define	HAVE_FCNTL_H
#	define	HAVE_TERMIOS_H
#	define	HAVE_TERMIO_H
#	define	HAVE_UTIMES
#	define	HAVE_UTIME_H
/*#	define	HAVE_WAIT_H*/
#	define	HAVE_SYS_WAIT_H
#	define	HAVE_SYS_UTSNAME_H
#	define	HAVE_SYS_MTIO_H
#	define	HAVE_MTGET_DSREG
#	define	HAVE_MTGET_RESID
#	define	HAVE_MTGET_FILENO
#	define	HAVE_MTGET_BLKNO
#	define	MAJOR_IN_SYSMACROS
#	define	HAVE_STRERROR
#	define	HAVE_MEMMOVE
#	define	HAVE_MMAP
/*#	define	HAVE_SMMAP*/
#	define	HAVE_USGSHM
#	define	HAVE_USGSEM
#	define	HAVE_FLOCK
#	define	HAVE_FCHDIR
#	define	HAVE_STATVFS
#	define	HAVE_QUOTA
#	define	HAVE_YP
#	define	HAVE_SETREUID	/* >= Linux 1.1.37 */
#	define	HAVE_SETEUID
/*#	define	HAVE_PROCFS*/	/* ??? */
/*#	define	HAVE_PROCFS2*/
#	define	HAVE_WAIT3
#	define	HAVE_WAITPID
#	define	HAVE_GETHOSTNAME
#	define	HAVE_SELECT

#	define	GID_T		gid_t
#	define	USLEEPRETURN_T	void
#	define	USLEEPISVOID

/*#	define	HAVE_GETAV0*/
#	define	HAVE_SCANSTACK
#endif


#if defined(__bsdi__)
#	define	PROTOTYPES
#	define	HAVE_AOUT
#	define	HAVE_STDARG_H
#	define	HAVE_LONGLONG
#	define	HAVE_UNISTD_H
#	define	HAVE_TERMIOS_H
#	define	HAVE_UTIMES
#	define	HAVE_UTIME_H
#	define	HAVE_MSEM
#	define	HAVE_FLOCK
#	define	HAVE_FCHDIR
#	define	HAVE_QUOTA
#	define	GID_T		gid_t
#	define	USLEEPRETURN_T	void
#	define	USLEEPISVOID
#	define	F_ULOCK	0	/* Unlock a previously locked region */
#	define	F_LOCK	1	/* Lock a region for exclusive use */
#	define	F_TLOCK	2	/* Test and lock a region for exclusive use */
#	define	F_TEST	3	/* Test a region for other processes locks */
#endif

/*
 * Prototype for FreeBSD
 */
#if defined(__FreeBSD__)
#	define	NO_FLOATINGPOINT/* No support for nonstd floatingpoint */
#	define	PROTOTYPES
#	define	HAVE_AOUT
#	define	HAVE_STDARG_H
#	define	HAVE_STDLIB_H
#	define	HAVE_STRING_H
#	define	HAVE_STDC_HEADERS
#	define	STDC_HEADERS	/* GNU name */
#	define	HAVE_LONGLONG
#	define	HAVE_UNISTD_H
#	define	HAVE_FCNTL_H
#	define	HAVE_TERMIOS_H
#	define	HAVE_UTIMES
#	define	HAVE_UTIME_H
#	define	HAVE_SYS_WAIT_H
#	define	HAVE_SYS_UTSNAME_H
#	define	HAVE_SYS_MTIO_H
#	define	HAVE_MTGET_DSREG
#	define	HAVE_MTGET_RESID
#	define	HAVE_MTGET_FILENO
#	define	HAVE_MTGET_BLKNO
/*#	define	HAVE_USG_STDIO*/
#	define	HAVE_STRERROR
#	define	HAVE_MEMMOVE
#	define	HAVE_MMAP
#	define	HAVE_SMMAP
#	define	HAVE_USGSHM
#	define	HAVE_USGSEM
#	ifdef sparc
#		define	HAVE_LDSTUB
#	endif
#	ifdef i386
#		define	HAVE_XCHG
#	endif
#	define	HAVE_FLOCK
#	define	HAVE_FCHDIR
#	define	HAVE_QUOTA
#	define	HAVE_YP
#	define	HAVE_SETREUID
/*#	define	HAVE_SETRESUID*/
#	define	HAVE_SETEUID
/*#	define	HAVE_LCHOWN*/
#	define	HAVE_WAIT3
#	define	HAVE_WAITPID
#	define	HAVE_UNION_WAIT
#	define	HAVE_GETHOSTNAME
#	define	HAVE_SELECT
#	define	GID_T		gid_t
/*#	define	USLEEPRETURN_T	unsigned int*/
#	define	USLEEPRETURN_T	void
#	define	USLEEPISVOID

/*#	define	HAVE_GETAV0*/
/*#	define	HAVE_SCANSTACK*/
#endif

/*
 * SysVr4
 */
#if defined(__SVR4) && !defined(__NOT_SVR4__)
#	define	PROTOTYPES
#	define	HAVE_ELF
#	define	HAVE_STDARG_H
#	if defined(IS_SUN)
#		define	HAVE_LONGLONG
#		define	HAVE_UTIMES
#		define	HAVE_QUOTA
#		define	HAVE_GETAV0	/* XXX what about PPC ??? */
#		define	HAVE_SCANSTACK	/* XXX what about PPC ??? */
#	endif
#	define	HAVE_UNISTD_H
#	define	HAVE_FCNTL_H
#	define	HAVE_TERMIOS_H
#	define	HAVE_TERMIO_H
#	define	HAVE_UTIME_H
#	define	HAVE_WAIT_H
#	define	HAVE_SYS_WAIT_H
#	define	HAVE_SYS_SYSTEMINFO_H
#	define	HAVE_SYS_UTSNAME_H
#	define	HAVE_SYS_PRIOCNTL_H
#	define	HAVE_SYS_RTPRIOCNTL_H
#	define	HAVE_SYS_MTIO_H
#	define	HAVE_MTGET_DSREG
#	define	HAVE_MTGET_RESID
#	define	HAVE_MTGET_FILENO
#	define	HAVE_MTGET_BLKNO
#	define	MAJOR_IN_MKDEV
#	define	HAVE_USG_STDIO
#	define	HAVE_STRERROR
#	define	HAVE_MEMMOVE
#	define	HAVE_MMAP
#	define	HAVE_SMMAP
#	define	HAVE_USGSHM
#	define	HAVE_USGSEM
#	ifdef sparc
#		define	HAVE_LDSTUB
#	endif
#	ifdef i386
#		define	HAVE_XCHG
#	endif
#	define	HAVE_FCHDIR
#	define	HAVE_STATVFS
#	define	HAVE_YP
#	define	HAVE_SHADOW
#	define	HAVE_SETEUID
#	define	HAVE_LCHOWN
#	define	HAVE_PROCFS
#	if (defined(i386) && !defined(IS_SUN))
#		define	HAVE_PROCFS2
#		define	HAVE_QUOTA
#	endif
#	define	HAVE_SIGINFO
#	define	HAVE_WAITPID
#	define	HAVE_STREAMS
#	define	HAVE_STRPTYS
#	define	HAVE_POLL
#	define	HAVE_SELECT
#	define	HAVE_TIRPC
#	define	GID_T		gid_t
#	define	USLEEPRETURN_T	unsigned int
#endif

/*
 * Prototype for new systems
 */
#if defined(__NEW_SYSTEM_TO_DEFINE__)
#	define	__NOT_SVR4__	/* Not a real SVR4 implementation */

#	define	PROTOTYPES
#	define	HAVE_ELF
#	define	HAVE_COFF
#	define	HAVE_AOUT
#	define	HAVE_STDARG_H
#	define	HAVE_STDLIB_H
#	define	HAVE_STRING_H
#	define	HAVE_STDC_HEADERS
#	define	STDC_HEADERS	/* GNU name */
#	define	HAVE_LONGLONG
#	define	HAVE_UNISTD_H
#	define	HAVE_FCNTL_H
#	define	HAVE_TERMIOS_H
#	define	HAVE_TERMIO_H
#	define	HAVE_UTIMES
#	define	HAVE_UTIME_H
#	define	HAVE_WAIT_H
#	define	HAVE_SYS_WAIT_H
#	define	HAVE_SYS_SYSTEMINFO_H
#	define	HAVE_SYS_UTSNAME_H
#	define	HAVE_SYS_PRIOCNTL_H
#	define	HAVE_SYS_RTPRIOCNTL_H
#	define	HAVE_SYS_MTIO_H
#	define	HAVE_MTGET_DSREG
#	define	HAVE_MTGET_RESID
#	define	HAVE_MTGET_FILENO
#	define	HAVE_MTGET_BLKNO
#	define	MAJOR_IN_MKDEV
#	define	MAJOR_IN_SYSMACROS
#	define	HAVE_USG_STDIO
#	define	HAVE_STRERROR
#	define	HAVE_MEMMOVE
#	define	HAVE_MMAP
#	define	HAVE_SMMAP
#	define	HAVE_USGSHM
#	define	HAVE_USGSEM
#	define	HAVE_MSEM
#	define	HAVE_LDSTUB
#	define	HAVE_XCHG
#	define	HAVE_FLOCK
#	define	HAVE_FCHDIR
#	define	HAVE_STATVFS
#	define	HAVE_QUOTA
#	define	HAVE_YP
#	define	HAVE_SHADOW
#	define	HAVE_SETREUID
#	define	HAVE_SETRESUID
#	define	HAVE_SETEUID
#	define	HAVE_LCHOWN
#	define	HAVE_PROCFS
#	define	HAVE_PROCFS2
#	define	HAVE_SIGINFO
#	define	HAVE_WAIT3
#	define	HAVE_WAITPID
#	define	HAVE_UNION_WAIT
#	define	HAVE_GETHOSTNAME
#	define	HAVE_STREAMS
#	define	HAVE_STRPTYS
#	define	HAVE_POLL
#	define	HAVE_SELECT
#	define	HAVE_TIRPC
#	define	GID_T		gid_t
#	define	USLEEPRETURN_T	unsigned int
#	define	USLEEPRETURN_T	void
#	define	USLEEPISVOID

#	define	HAVE_GETAV0
#	define	HAVE_SCANSTACK
#endif

/*
 * If there is no flock defined by the system, use emulation
 * through fcntl record locking.
 */
#ifndef HAVE_FLOCK
#define LOCK_SH         1       /* shared lock */
#define LOCK_EX         2       /* exclusive lock */
#define LOCK_NB         4       /* don't block when locking */
#define LOCK_UN         8       /* unlock */
#endif

#include <prototyp.h>

/*
 * gcc 2.x generally implements the long long type.
 */
#ifdef	__GNUC__
#	if	__GNUC__ > 1
#		ifndef	HAVE_LONGLONG
#			define	HAVE_LONGLONG
#		endif
#	endif
#endif

/*
 * Convert to GNU name
 */
#ifdef	HAVE_STDC_HEADERS
#	ifndef	STDC_HEADERS
#		define	STDC_HEADERS
#	endif
#endif
/*
 * Convert to SCHILY name
 */
#ifdef	STDC_HEADERS
#	ifndef	HAVE_STDC_HEADERS
#		define	HAVE_STDC_HEADERS
#	endif
#endif

#ifdef	IS_UNIX
#	define	PATH_DELIM	'/'
#	define	PATH_DELIM_STR	"/"
#	define	far
#	define	near
#endif

#ifdef	IS_MSDOS
#	define	PATH_DELIM	'\\'
#	define	PATH_DELIM_STR	"\\"
#endif

#ifdef	IS_TOS
#	define	PATH_DELIM	'\\'
#	define	PATH_DELIM_STR	"\\"
#	define	far
#	define	near
#endif

#ifdef	IS_MAC
#	define	PATH_DELIM	':'
#	define	PATH_DELIM_STR	":"
#	define	far
#	define	near
#endif

#ifdef __cplusplus
}
#endif

#endif /* _MCONFIG_H */
