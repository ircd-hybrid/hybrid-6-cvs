/************************************************************************
 *   IRC - Internet Relay Chat, ircd/s_zip.c
 *   Copyright (C) 1996  Christophe Kalt
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
 */

#ifndef lint
static  char rcsid[] = "@(#)$Id: s_zip.c,v 1.1 1998/10/06 04:42:35 db Exp $";
#endif

#include "struct.h"
#include "sys.h"
#include "h.h"

#ifdef	ZIP_LINKS
/*
** Important note:
**	The provided buffers for uncompression and compression *MUST* be big
**	enough for any operation to complete.
**
**	s_bsd.c current settings are that the biggest packet size is 16k
**	(but socket buffers are set to 8k..)
*/

/*
** size of the buffer holding compressed data
**
** outgoing data:
**	must be enough to hold compressed data resulting of the compression
**	of up to ZIP_MAXIMUM bytes
** incoming data:
**	must be enough to hold cptr->zip->inbuf + what was just read
**	(cptr->zip->inbuf should never hold more than ONE compression block.
**	The biggest block allowed for compression is ZIP_MAXIMUM bytes)
*/
#define	ZIP_BUFFER_SIZE		(ZIP_MAXIMUM + READBUF_SIZE)

/*
** size of the buffer where zlib puts compressed data
**	must be enough to hold uncompressed data resulting of the
**	uncompression of zibuffer
**
**	I'm assuming that at best, ratio will be 25%. (tests show that
**	best ratio is around 40%).
*/
#define	UNZIP_BUFFER_SIZE	4*ZIP_BUFFER_SIZE

/* buffers */
static	char	unzipbuf[UNZIP_BUFFER_SIZE];
static	char	zipbuf[ZIP_BUFFER_SIZE];

/*
** zip_init
**	Initialize compression structures for a server.
**	If failed, zip_free() has to be called.
*/
int	zip_init(aClient *cptr)
{
  cptr->zip  = (aZdata *) MyMalloc(sizeof(aZdata));
  cptr->zip->incount = 0;
  cptr->zip->outcount = 0;

  cptr->zip->in  = (z_stream *) MyMalloc(sizeof(z_stream));
  cptr->zip->in->total_in = 0;
  cptr->zip->in->total_out = 0;
  cptr->zip->in->zalloc = (alloc_func)0;
  cptr->zip->in->zfree = (free_func)0;
  cptr->zip->in->data_type = Z_ASCII;
  if (inflateInit(cptr->zip->in) != Z_OK)
    {
      cptr->zip->out = NULL;
      return -1;
    }

  cptr->zip->out = (z_stream *) MyMalloc(sizeof(z_stream));
  cptr->zip->out->total_in = 0;
  cptr->zip->out->total_out = 0;
  cptr->zip->out->zalloc = (alloc_func)0;
  cptr->zip->out->zfree = (free_func)0;
  cptr->zip->out->data_type = Z_ASCII;
  if (deflateInit(cptr->zip->out, ZIP_LEVEL) != Z_OK)
    return -1;

  return 0;
}

/*
** zip_free
*/
void	zip_free(aClient *cptr)
{
  cptr->flags2 &= ~FLAGS2_ZIP;
  if (cptr->zip)
    {
      if (cptr->zip->in)
	inflateEnd(cptr->zip->in);
      MyFree(cptr->zip->in);
      if (cptr->zip->out)
	deflateEnd(cptr->zip->out);
      MyFree(cptr->zip->out);
      MyFree(cptr->zip);
      cptr->zip = NULL;
    }
}

/*
** unzip_packet
** 	Unzip the content of cptr->zip->inbuf and of the buffer,
**	put anything left in cptr->zip->inbuf, update cptr->zip->incount
**
**	will return the uncompressed buffer, length will be updated.
**	if a fatal error occurs, length will be set to -1
*/
char *	unzip_packet(aClient *cptr, char *buffer, int *length)
{
  Reg	z_stream *zin = cptr->zip->in;
  int	r;

  if (cptr->zip->incount + *length > ZIP_BUFFER_SIZE) /* sanity check */
    {
    }
  /* put everything in zipbuf */
  bcopy(cptr->zip->inbuf, zipbuf, cptr->zip->incount);
  bcopy(buffer, zipbuf + cptr->zip->incount, *length);

  zin->next_in = zipbuf;
  zin->avail_in = cptr->zip->incount + *length;
  zin->next_out = unzipbuf;
  zin->avail_out = UNZIP_BUFFER_SIZE;
  switch (r = inflate(zin, Z_PARTIAL_FLUSH))
    {
    case Z_OK:
      if (zin->avail_in)
	{
	  /* put the leftover in cptr->zip->inbuf */
	  bcopy(zin->next_in, cptr->zip->inbuf, zin->avail_in);
	  cptr->zip->incount = zin->avail_in;
	}
      *length = UNZIP_BUFFER_SIZE - zin->avail_out;
      return unzipbuf;

    case Z_BUF_ERROR: /*no progress possible or output buffer too small*/
      if (zin->avail_out == 0)
	{
	  sendto_ops("inflate() returned Z_BUF_ERROR: %s",
		      (zin->msg) ? zin->msg : "?");
	  *length = -1;
	}
      break;

    case Z_DATA_ERROR: /* the buffer might not be compressed.. */
      if ((IsCapable(cptr, CAP_ZIP))  && !strncmp("ERROR ", buffer, 6))
	{
	  cptr->flags2 &= ~FLAGS2_ZIP;
	  cptr->caps &= ~CAP_ZIP;
	  /*
	   * This is not sane at all.  But if other server
	   * has sent an error now, it is probably closing
	   * the link as well.
	   */
	  return buffer;
	}

	/* no break */

    default: /* error ! */
      /* should probably mark link as dead or something... */
      sendto_ops("inflate() error(%d): %s", r,
		  (zin->msg) ? zin->msg : "?");
      *length = -1; /* report error condition */
      break;
    }
  return NULL;
}

/*
** zip_buffer
** 	Zip the content of cptr->zip->outbuf and of the buffer,
**	put anything left in cptr->zip->outbuf, update cptr->zip->outcount
**
**	if flush is set, then all available data will be compressed,
**	otherwise, compression only occurs if there's enough to compress,
**	or if we are reaching the maximum allowed size during a connect burst.
**
**	will return the uncompressed buffer, length will be updated.
**	if a fatal error occurs, length will be set to -1
*/
char *	zip_buffer(aClient *cptr, char *buffer, int *length, int flush)
{
  Reg	z_stream *zout = cptr->zip->out;
  int	r;

  if (buffer)
    {
      /* concatenate buffer in cptr->zip->outbuf */
      bcopy(buffer, cptr->zip->outbuf + cptr->zip->outcount,*length);
      cptr->zip->outcount += *length;
    }
  *length = 0;

  if (!flush && ((cptr->zip->outcount < ZIP_MINIMUM) ||
		 ((cptr->zip->outcount < (ZIP_MAXIMUM - BUFSIZE)) &&
		  CBurst(cptr))))
    return NULL;

  zout->next_in = cptr->zip->outbuf;
  zout->avail_in = cptr->zip->outcount;
  zout->next_out = zipbuf;
  zout->avail_out = ZIP_BUFFER_SIZE;

  switch (r = deflate(zout, Z_PARTIAL_FLUSH))
    {
    case Z_OK:
      if (zout->avail_in)
	{
	  /* can this occur?? I hope not... */
	  sendto_ops("deflate() didn't process all available data!");
	}
      cptr->zip->outcount = 0;
      *length = ZIP_BUFFER_SIZE - zout->avail_out;
      return zipbuf;

    default: /* error ! */
      sendto_ops("deflate() error(%d): %s", r, (zout->msg) ? zout->msg : "?");
      *length = -1;
      break;
    }
  return NULL;
}

#endif	/* ZIP_LINKS */
