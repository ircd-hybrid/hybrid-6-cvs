/*
 * $Id: acconfig.h,v 1.2 1999/07/15 03:09:53 db Exp $
 */

/* Define only one of POSIX, BSD, or SYSV signals.  */
/* Define if your system has reliable POSIX signals.  */
#undef POSIX_SIGNALS

/* Define if your system has reliable BSD signals.  */
#undef BSD_RELIABLE_SIGNALS

/* Define if your system has unreliable SYSV signals.  */
#undef SYSV_UNRELIABLE_SIGNALS

/* Define MALLOCH to <malloc.h> if needed.  */
#if !defined(STDC_HEADERS)
#undef MALLOCH
#endif

/* Chose only one of NBLOCK_POSIX, NBLOCK_BSD, and NBLOCK_SYSV */
/* Define if you have posix non-blocking sockets (O_NONBLOCK) */
#undef NBLOCK_POSIX

/* Define if you have BSD non-blocking sockets (O_NDELAY) */
#undef NBLOCK_BSD

/* Define if you have SYSV non-blocking sockets (FIONBIO) */
#undef NBLOCK_SYSV

/* Define if you have the mmap() function.  */
#undef USE_MMAP

/* Define if you are running AIX.  */
#undef OS_AIX

/* Define if you are running DYNIXPTX.  */
#undef OS_DYNIXPTX

/* Define if you are running HPUX.  */
#undef OS_HPUX

/* Define if you are running MIPS.  */
#undef OS_MIPS

/* Define if you are running NeXT.  */
#undef OS_NEXT

/* Define if you are running SOLARIS2.  */
#undef OS_SOLARIS2
