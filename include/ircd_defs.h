/* - Internet Relay Chat, include/ircd_defs.h
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Computing Center
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * $Id: ircd_defs.h,v 1.4 1999/07/15 08:47:30 tomh Exp $
 *
 * ircd_defs.h - Global size definitions for record entries used
 * througout ircd. Please think 3 times before adding anything to this
 * file.
 */
#ifndef	INCLUDED_ircd_defs_h
#define INCLUDED_ircd_defs_h
#ifndef INCLUDED_config_h
#include "config.h"
#endif
#if !defined(CONFIG_H_LEVEL_6)
#  error Incorrect config.h for this revision of ircd.
#endif

#define	HOSTLEN		63	/* Length of hostname.  Updated to         */
				/* comply with RFC1123                     */

#define	NICKLEN		9	/* Necessary to put 9 here instead of 10
				 * if s_msg.c/m_nick has been corrected.
				 * This preserves compatibility with old
				 * servers --msa
				 */
#define	USERLEN		10
#define	REALLEN	 	50
#define TOPICLEN 	120	/* old value 90, truncated on other servers */
#define	KILLLEN		90	
#define	CHANNELLEN	200
#define	KEYLEN		23
#define	BUFSIZE		512	/* WARNING: *DONT* CHANGE THIS!!!! */
#define	MAXRECIPIENTS 	20
#define	MAXBANS		25	/* bans + exceptions together */
#define	MAXBANLENGTH	1024

#define OPERWALL_LEN    400	/* can be truncated on other servers */

#define	USERHOST_REPLYLEN	(NICKLEN+HOSTLEN+USERLEN+5)
#define MAX_DATE_STRING 32      /* maximum string length for a date string */

#define READBUF_SIZE    16384	/* used in s_bsd *AND* s_zip.c ! */

/* 
 * Macros everyone uses :/ moved here from sys.h
 */
#define MyFree(x)       if ((x)) free((x))

#define DEBUG_BLOCK_ALLOCATOR
#ifdef DEBUG_BLOCK_ALLOCATOR
extern const char* BH_CurrentFile;
extern int         BH_CurrentLine;

#define free_client(x)  { BH_CurrentFile = __FILE__; \
                          BH_CurrentLine = __LINE__;\
			  _free_client((x)); }

#define free_link(x)    { BH_CurrentFile = __FILE__; \
                          BH_CurrentLine = __LINE__;\
			  _free_link((x)); }

#define free_user(x,y)  { BH_CurrentFile = __FILE__; \
                          BH_CurrentLine = __LINE__;\
			  _free_user((x), (y)); }

#ifdef FLUD
#define free_fludbot(x) { BH_CurrentFile = __FILE__;\
                          BH_CurrentLine = __LINE__;\
                          BlockHeapFree(free_fludbots, (x)); }
#endif

#else
#define free_client(x) _free_client((x))
#define free_link(x)   _free_link((x))
#define free_user(x,y) _free_user((x), (y))

#ifdef FLUD
#define free_fludbot(x) BlockHeapFree(free_fludbots, (x))
#endif

#endif


#endif /* INCLUDED_ircd_defs_h */
