/* @(#)scsi-linux-sg.c	1.5 97/04/06 Copyright 1997 J. Schilling */
#ifndef lint
static	char __sccsid[] =
	"@(#)scsi-linux-sg.c	1.5 97/04/06 Copyright 1997 J. Schilling";
#endif
/*
 *	Interface for Linux generic SCSI implementation (sg).
 *
 *	This is the interface for the broken Linux SCSI generic driver.
 *	This is a hack, that tries to emulate the functionality
 *	of the scg driver.
 *
 *	Design flaws of the sg driver:
 *	-	cannot see if SCSI command could not be send
 *	-	cannot get SCSI status byte
 *	-	cannot get real dma count of tranfer
 *	-	cannot get number of bytes valid in auto sense data
 *	-	to few data in auto sense (CCS/SCSI-2/SCSI-3 needs >= 18)
 *
 *	Copyright (c) 1997 J. Schilling
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

#include <linux/version.h>

#ifndef LINUX_VERSION_CODE	/* Very old kernel? */
#	define LINUX_VERSION_CODE 0
#endif

#if LINUX_VERSION_CODE >= 0x01031a /* <linux/scsi.h> introduced in 1.3.26 */
#if LINUX_VERSION_CODE >= 0x020000 /* <scsi/scsi.h> introduced somewhere. */
/* Need to fine tune the ifdef so we get the transition point right. */
#include <scsi/scsi.h>
#else
#include <linux/scsi.h>
#endif
#else
#define __KERNEL__
#include <linux/fs.h>
#undef __KERNEL__
#include "block/blk.h"
#include "scsi/scsi.h"
#endif

#include "scsi/sg.h"

LOCAL	int	pack_id = 5;

struct sg_rq {
	struct sg_header	hd;
	unsigned char		buf[SG_BIG_BUFF];
} sg_rq;

char	*SCSIbuf;

#define	MAX_SCG		1	/* Max # of SCSI controllers */

LOCAL	int	scgfiles[8][8];

#ifdef	SG_BIG_BUFF
#define	MAX_DMA_LINUX	SG_BIG_BUFF
#else
#define	MAX_DMA_LINUX	(32*1024)
#endif


LOCAL	int	scsi_send	__PR((int f, struct scg_cmd *sp));
LOCAL	void	scsi_setup	__PR((int f));
LOCAL	void	scsi_settimeout	__PR((int f, int timeout));

EXPORT
int scsi_open()
{
	register int	f;
	register int	i;
	register int	nopen = 0;
	char		devname[32];

	for (i=0; i < 8; i++) {
		for (f=0; f < 8 ; f++)
			scgfiles[i][f] = -1;

	}
	for (i=0; i < 8; i++) {
		sprintf(devname, "/dev/sg%c", i+'a');
		f = open(devname, 2);
		if (f < 0) {
			if (errno != ENOENT && errno != ENXIO && errno != ENODEV)
				comerr("Cannot open '%s'.\n", devname);
		} else {
			nopen++;
			scsi_setup(f);
		}
	}
	return (nopen);
}

LOCAL void
scsi_setup(f)
	int	f;
{
	struct sg_rep {
		struct sg_header	hd;
		unsigned char		rbuf[100];
	} sg_rep;
	struct	sg_id {
		long	l1; /* target | lun << 8 | channel << 16 | low_ino << 24 */
		long	l2; /* Unique id */
	} sg_id;
	int	n;
	int	Target;
	int	Lun;


	/* Eat any unwanted garbage from prior use of this device */

	n = fcntl(f, F_GETFL);	/* Be very proper about this */
	fcntl(f, F_SETFL, n|O_NONBLOCK);

	fillbytes((caddr_t)&sg_rep, sizeof(struct sg_header), '\0');
	sg_rep.hd.reply_len = sizeof(struct sg_header);

	while (read(f, &sg_rep, sizeof(sg_rep)) >= 0 || errno != EAGAIN)
		;

	fcntl(f, F_SETFL, n);

	scsi_settimeout(f, deftimeout);

	ioctl(f, SCSI_IOCTL_GET_IDLUN, &sg_id);
	if (debug)
		printf("l1: 0x%lX l2: 0x%lX\n", sg_id.l1, sg_id.l2);

	Target	= sg_id.l1 & 0xFF;
	Lun	= (sg_id.l1 >> 8) & 0xFF;

	scgfiles[Target][Lun] = f;
}

LOCAL long
scsi_maxdma()
{
	long maxdma = MAX_DMA_LINUX;

	return (maxdma);
}

EXPORT void *
scsi_getbuf(amt)
	long	amt;
{
	void	*ret;

	if (scg_maxdma == 0)
		scg_maxdma = scsi_maxdma();

	if (amt <= 0 || amt > scg_maxdma)
		return ((void *)0);
	if (debug)
		printf("scsi_getbuf: %ld bytes\n", amt);
	/*
	 * For performance reason, we allocate pagesize()
	 * bytes before the SCSI buffer to avoid
	 * copying the whole buffer contents when 
	 * setting up the /dev/sg data structures.
	 */
	ret = valloc((size_t)(amt+getpagesize()));
	ret += getpagesize();
	SCSIbuf = ret;
	return (ret);
}

EXPORT
BOOL scsi_havebus(busno)
	int	busno;
{
	return (busno < 0 || busno >= MAX_SCG) ? FALSE : TRUE;
}

EXPORT
int scsi_fileno(busno, tgt, tlun)
	int	busno;
	int	tgt;
	int	tlun;
{
	return (busno < 0 || busno >= MAX_SCG) ? -1 : scgfiles[tgt][tlun];
}

EXPORT
int scsireset()
{
	/*
	 * Do we have a SCSI reset in the Linux sg driver?
	 */
	return (-1);
}

LOCAL void
scsi_settimeout(f, tmo)
	int	f;
	int	tmo;
{
	tmo *= 100;

	if (ioctl(f, SG_SET_TIMEOUT, &tmo) < 0)
		comerr("Cannot set SG_SET_TIMEOUT.\n");
}

LOCAL int
scsi_send(int f, struct scg_cmd *sp)
{
	struct sg_rq	*sgp;
	struct sg_rq	*sgp2;
	int	i;
	int	amt = sp->cdb_len;

	if (f < 0) {
		sp->error = SCG_FATAL;
		return (0);
	}
	if (sp->timeout != deftimeout);
		scsi_settimeout(f, sp->timeout);

	sgp2 = sgp = &sg_rq;
	if (sp->addr == SCSIbuf) {
		sgp = (struct sg_rq *)
			(SCSIbuf - (sizeof(struct sg_header) + amt));
		sgp2 = (struct sg_rq *)
			(SCSIbuf - (sizeof(struct sg_header)));
	}

	fillbytes((caddr_t)sgp, sizeof(struct sg_header), '\0');

	sgp->hd.pack_len = sizeof(struct sg_header) + amt;
	sgp->hd.reply_len = sizeof(struct sg_header) + sp->size;
	sgp->hd.pack_id = pack_id++;
	sgp->hd.result = 0;

	for (i = 0; i < amt; i++ ) {
		sgp->buf[i] = sp->cdb.cmd_cdb[i];;
	}
	if (!(sp->flags & SCG_RECV_DATA)) {
		if ((void *)sp->addr != (void *)&sgp->buf[amt])
			movebytes(sp->addr, &sgp->buf[amt], sp->size);
		amt += sp->size;
		sgp->hd.pack_len += sp->size;
	}
	i = sizeof(struct sg_header) + amt;
	if ((amt = write(f, sgp, i)) < 0) {	/* write */
		scsi_settimeout(f, deftimeout);
		return (-1);
	} else if (amt != i) {
		errmsg("scsi_send(%s) wrote %d bytes (expected %d).\n",
						scsi_command, amt, i);
	}

	if (sp->addr == SCSIbuf) {
		movebytes(sgp, sgp2, sizeof(struct sg_header));
		sgp = sgp2;
	}
	sgp->hd.sense_buffer[0] = 0;
	amt = read(f, sgp, sgp->hd.reply_len);	/* read */

	if (sp->flags & SCG_RECV_DATA && ((void *)sgp->buf != (void *)sp->addr)) {
		movebytes(sgp->buf, sp->addr, sp->size);
	}
	sp->errno = sgp->hd.result;

	if (debug) {
		printf("pack_len: %d, reply_len: %d pack_id: %d result: %d\n",
				sgp->hd.pack_len,
				sgp->hd.reply_len,
				sgp->hd.pack_id,
				sgp->hd.result);
		printf("sense: ");
		for (i=0; i< 16; i++)
			printf("%02X ", sgp->hd.sense_buffer[i]);
		printf("\n");
	}

	sp->resid = sp->size +sizeof(struct sg_header) - amt;
	if (sgp->hd.sense_buffer[0] != 0) {
		sp->scb.chk = 1;
		sp->sense_count = 16;
		movebytes(sgp->hd.sense_buffer, sp->u_sense.cmd_sense, sp->sense_count);
	}
	if (sp->timeout != deftimeout);
		scsi_settimeout(f, deftimeout);
	return 0;
}
