/* @(#)drv_simul.c	1.14 00/04/16 Copyright 1998 J. Schilling */
#ifndef lint
static	char sccsid[] =
	"@(#)drv_simul.c	1.14 00/04/16 Copyright 1998 J. Schilling";
#endif
/*
 *	Simulation device driver
 *
 *	Copyright (c) 1998 J. Schilling
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

#define	DEBUG
#include <mconfig.h>

#include <stdio.h>
#include <standard.h>
#include <stdxlib.h>
#include <unixstd.h>
#include <errno.h>
#include <strdefs.h>

#include <sys/types.h>
#include <sys/time.h>
#include <utypes.h>
#include <btorder.h>
/*#include <scgio.h>*/
#include <scg/scsidefs.h>
#include <scg/scsireg.h>
#include <scg/scsitransp.h>

#include <libport.h>

#include "cdrecord.h"

extern	int	silent;
extern	int	verbose;
extern	int	lverbose;

LOCAL	int	simul_load		__PR((SCSI *scgp));
LOCAL	int	simul_unload		__PR((SCSI *scgp));
LOCAL	cdr_t	*identify_simul		__PR((SCSI *scgp, cdr_t *, struct scsi_inquiry *));
LOCAL	int	getdisktype_simul	__PR((SCSI *scgp, cdr_t *dp, dstat_t *dsp));
LOCAL	int	speed_select_simul	__PR((SCSI *scgp, int *speedp, int dummy));
LOCAL	int	next_wr_addr_simul	__PR((SCSI *scgp, int track, track_t *trackp, long *ap));
LOCAL	int	cdr_write_simul		__PR((SCSI *scgp, caddr_t bp, long sectaddr, long size, int blocks, BOOL islast));
LOCAL	int	open_track_simul	__PR((SCSI *scgp, cdr_t *dp, int track, track_t *trackp));
LOCAL	int	close_track_simul	__PR((SCSI *scgp, int track, track_t *trackp));
LOCAL	int	open_session_simul	__PR((SCSI *scgp, cdr_t *dp, int tracks, track_t *trackp, int toctype, int multi));
LOCAL	int	fixate_simul		__PR((SCSI *scgp, int onp, int dummy, int toctype, int tracks, track_t *trackp));
LOCAL	void	tv_sub			__PR((struct timeval *tvp1, struct timeval *tvp2));

LOCAL int
simul_load(scgp)
	SCSI	*scgp;
{
	return (0);
}

LOCAL int
simul_unload(scgp)
	SCSI	*scgp;
{
	return (0);
}

cdr_t	cdr_cdr_simul = {
	0,
	CDR_TAO|CDR_DAO|CDR_PACKET,
	"cdr_simul",
	"simulation CD-R driver for timing/speed tests",
	0,
	identify_simul,
	drive_attach,
	getdisktype_simul,
	simul_load,
	simul_unload,
	buf_dummy,
	(int(*)__PR((SCSI *)))cmd_dummy,	/* recovery_needed	*/
	(int(*)__PR((SCSI *, int)))cmd_dummy,	/* recover		*/
	speed_select_simul,
	select_secsize,
	next_wr_addr_simul,
	(int(*)__PR((SCSI *, Ulong)))cmd_ill,	/* reserve_track	*/
	cdr_write_simul,
	(int(*)__PR((SCSI *scgp, int, track_t *)))cmd_dummy,	/* send_cue */
	open_track_simul,
	close_track_simul,
	open_session_simul,
	cmd_dummy,
	read_session_offset,
	fixate_simul,
	blank_dummy,
	(int(*)__PR((SCSI *, caddr_t, int, int)))NULL,	/* no OPC	*/
};

cdr_t	cdr_dvd_simul = {
	0,
	CDR_TAO|CDR_DAO|CDR_PACKET,
	"dvd_simul",
	"simulation DVD-R driver for timing/speed tests",
	0,
	identify_simul,
	drive_attach,
	getdisktype_simul,
	simul_load,
	simul_unload,
	buf_dummy,
	(int(*)__PR((SCSI *)))cmd_dummy,	/* recovery_needed	*/
	(int(*)__PR((SCSI *, int)))cmd_dummy,	/* recover		*/
	speed_select_simul,
	select_secsize,
	next_wr_addr_simul,
	(int(*)__PR((SCSI *, Ulong)))cmd_ill,	/* reserve_track	*/
	cdr_write_simul,
	(int(*)__PR((SCSI *scgp, int, track_t *)))cmd_dummy,	/* send_cue */
	open_track_simul,
	close_track_simul,
	open_session_simul,
	cmd_dummy,
	read_session_offset,
	fixate_simul,
	blank_dummy,
	(int(*)__PR((SCSI *, caddr_t, int, int)))NULL,	/* no OPC	*/
};

LOCAL cdr_t *
identify_simul(scgp, dp, ip)
	SCSI			*scgp;
	cdr_t			*dp;
	struct scsi_inquiry	*ip;
{
	return (dp);
}

LOCAL	int	simul_speed = 1;
LOCAL	int	simul_dummy;
LOCAL	int	simul_isdvd;
LOCAL	int	simul_bufsize = 1024;
LOCAL	Uint	sleep_rest;
LOCAL	Uint	sleep_max;
LOCAL	Uint	sleep_min;

LOCAL int
getdisktype_simul(scgp, dp, dsp)
	SCSI	*scgp;
	cdr_t	*dp;
	dstat_t	*dsp;
{
	if (strcmp(dp->cdr_drname, cdr_cdr_simul.cdr_drname) == 0) {
		dsp->ds_maxblocks = 333000;
		simul_isdvd = FALSE;
	} else {
		dsp->ds_maxblocks = 2464153;	/* 4.7 GB  */
/*		dsp->ds_maxblocks = 1927896;*/	/* 3.95 GB */
		simul_isdvd = TRUE;
	}
	return (drive_getdisktype(scgp, dp, dsp));
}


LOCAL int
speed_select_simul(scgp, speedp, dummy)
	SCSI	*scgp;
	int	*speedp;
	int	dummy;
{
	long	val;
	char	*p;

	if (speedp)
		simul_speed = *speedp;
	simul_dummy = dummy;

	if ((p = getenv("CDR_SIMUL_BUFSIZE")) != NULL) {
		if (getnum(p, &val) == 1)
			simul_bufsize = val / 1024;
	}

	/*
	 * sleep_max is the time to empty the drive's buffer in �s.
	 * sector size is from 2048 bytes to 2352 bytes.
	 * If sector size is 2048 bytes, 1k takes 6.666 ms.
	 * If sector size is 2352 bytes, 1k takes 5.805 ms.
	 * We take the 6 ms as an average between both values.
	 * simul_bufsize is the number of kilobytes in drive buffer.
	 */
	sleep_max = 6 * 1000 * simul_bufsize / simul_speed;
	if (lverbose) {
		printf("Simulation drive buffer size: %d KB\n", simul_bufsize);
		printf("Mamimum reserve time in drive buffer: %d.%3.3d ms\n",
					sleep_max / 1000,
					sleep_max % 1000);
	}
	return (0);
}

LOCAL int
next_wr_addr_simul(scgp, track, trackp, ap)
	SCSI	*scgp;
	int	track;
	track_t	*trackp;
	long	*ap;
{
	if (ap)
		*ap = 0L;
	return (0);
}

LOCAL int
cdr_write_simul(scgp, bp, sectaddr, size, blocks, islast)
	SCSI	*scgp;
	caddr_t	bp;		/* address of buffer */
	long	sectaddr;	/* disk address (sector) to put */
	long	size;		/* number of bytes to transfer */
	int	blocks;		/* sector count */
	BOOL	islast;		/* last write for track */
{
	Uint	sleep_time;
	Uint	sleep_diff;

	struct timeval	tv1;
static	struct timeval	tv2;

	if (lverbose > 1 && islast)
		printf("\nWriting last record for this track.\n");

	gettimeofday(&tv1, (struct timezone *)0);
	if (tv2.tv_sec != 0) {		/* Already did gettimeofday(&tv2) */
		tv_sub(&tv1, &tv2);
		if (sleep_rest != 0) {
			sleep_diff = tv1.tv_sec * 1000000 + tv1.tv_usec;

			if (sleep_min > (sleep_rest - sleep_diff))
				sleep_min = (sleep_rest - sleep_diff);

			if (sleep_diff > sleep_rest) {
				printf("Buffer underrun: actual delay was %d.%3.3d ms, max delay was %d.%3.3d ms.\n",
						sleep_diff / 1000,
						sleep_diff % 1000,
						sleep_rest / 1000,
						sleep_rest % 1000);
				if (!simul_dummy)
					return (-1);
			}
			/*
			 * If we spent time outside the write function
			 * subtract this time.
			 */
			sleep_diff = tv1.tv_sec * 1000000 + tv1.tv_usec;
			if (sleep_rest >= sleep_diff)
				sleep_rest -= sleep_diff;
			else
				sleep_rest = 0;
		}
	}
	/*
	 * Speed 1 ist 150 Sektoren/s
	 */
	sleep_time = 1000000 * blocks / 75 / simul_speed;
	sleep_time += sleep_rest;
	if (sleep_time > sleep_max) {
		int	mod;

		sleep_rest = sleep_max;
		sleep_time -= sleep_rest;
		mod = sleep_time % 20000;
		sleep_rest += mod;
		sleep_time -= mod;
		if (sleep_time > 0)
			usleep(sleep_time);
	} else {
		sleep_rest = sleep_time;
	}

	gettimeofday(&tv2, (struct timezone *)0);
	return (size);
}

LOCAL int
open_track_simul(scgp, dp, track, trackp)
	SCSI	*scgp;
	cdr_t	*dp;
	int	track;
	track_t *trackp;
{
	sleep_min = 999 * 1000000;
	return (0);
}

LOCAL int
close_track_simul(scgp, track, trackp)
	SCSI	*scgp;
	int	track;
	track_t	*trackp;
{
	if (lverbose) {
		printf("Remaining reserve time in drive buffer: %d.%3.3d ms\n",
					sleep_rest / 1000,
					sleep_rest % 1000);
		printf("Minimum reserve time in drive buffer: %d.%3.3d ms\n",
					sleep_min / 1000,
					sleep_min % 1000);
	}
	usleep(sleep_rest);
	sleep_rest = 0;
	return (0);
}

LOCAL int
open_session_simul(scgp, dp, tracks, trackp, toctype, multi)
	SCSI	*scgp;
	cdr_t	*dp;
	int	tracks;
	track_t	*trackp;
	int	toctype;
	int	multi;
{
	return (0);
}

LOCAL int
fixate_simul(scgp, onp, dummy, toctype, tracks, trackp)
	SCSI	*scgp;
	int	onp;
	int	dummy;
	int	toctype;
	int	tracks;
	track_t	*trackp;
{
	return (0);
}

LOCAL void
tv_sub(tvp1, tvp2)
	struct timeval	*tvp1;
	struct timeval	*tvp2;
{
	tvp1->tv_sec -= tvp2->tv_sec;
	tvp1->tv_usec -= tvp2->tv_usec;

	while (tvp1->tv_usec < 0) {
		tvp1->tv_usec += 1000000;
		tvp1->tv_sec -= 1;
	}
}