/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/* inflate.c -- zlib decompression
 * Copyright (C) 1995-2005 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 *
 * Based on zlib 1.2.3 but modified for the Linux Kernel by
 * Richard Purdie <richard@openedhand.com>
 *
 * Changes mainly for static instead of dynamic memory allocation
 *
 */

#include <linux/zutil.h>
#include "inftrees.h"
#include "inflate.h"
#include "inffast.h"
#include "infutil.h"

int zlib_inflate_workspacesize(void)
{
    return sizeof(struct inflate_workspace);
}

int zlib_inflateReset(z_streamp strm)
{
    struct inflate_state *state;

    if (strm == NULL || strm->state == NULL) return Z_STREAM_ERROR;
    state = (struct inflate_state *)strm->state;
    strm->total_in = strm->total_out = state->total = 0;
    strm->msg = NULL;
    strm->adler = 1;        /* to support ill-conceived Java test suite */
    state->mode = HEAD;
    state->last = 0;
    state->havedict = 0;
    state->dmax = 32768U;
    state->hold = 0;
    state->bits = 0;
    state->lencode = state->distcode = state->next = state->codes;

    /* Initialise Window */
    state->wsize = 1U << state->wbits;
    state->write = 0;
    state->whave = 0;

    return Z_OK;
}

#if 0
int zlib_inflatePrime(z_streamp strm, int bits, int value)
{
    struct inflate_state *state;

    if (strm == NULL || strm->state == NULL) return Z_STREAM_ERROR;
    state = (struct inflate_state *)strm->state;
    if (bits > 16 || state->bits + bits > 32) return Z_STREAM_ERROR;
    value &= (1L << bits) - 1;
    state->hold += value << state->bits;
    state->bits += bits;
    return Z_OK;
}
#endif

int zlib_inflateInit2(z_streamp strm, int windowBits)
{
    struct inflate_state *state;

    if (strm == NULL) return Z_STREAM_ERROR;
    strm->msg = NULL;                 /* in case we return an error */

    state = &WS(strm)->inflate_state;
    strm->state = (struct internal_state *)state;

    if (windowBits < 0) {
        state->wrap = 0;
        windowBits = -windowBits;
    }
    else {
        state->wrap = (windowBits >> 4) + 1;
    }
    if (windowBits < 8 || windowBits > 15) {
        return Z_STREAM_ERROR;
    }
    state->wbits = (unsigned)windowBits;
    state->window = &WS(strm)->working_window[0];

    return zlib_inflateReset(strm);
}

/*
   Return state with length and distance decoding tables and index sizes set to
   fixed code decoding.  This returns fixed tables from inffixed.h.
 */
static void zlib_fixedtables(struct inflate_state *state)
{
#   include "inffixed.h"
    state->lencode = lenfix;
    state->lenbits = 9;
    state->distcode = distfix;
    state->distbits = 5;
}


/*
   Update the window with the last wsize (normally 32K) bytes written before
   returning. This is only called when a window is already in use, or when
   output has been written during this inflate call, but the end of the deflate
   stream has not been reached yet. It is also called to window dictionary data
   when a dictionary is loaded.

   Providing output buffers larger than 32K to inflate() should provide a speed
   advantage, since only the last 32K of output is copied to the sliding window
   upon return from inflate(), and since all distances after the first 32K of
   output will fall in the output data, making match copies simpler and faster.
   The advantage may be dependent on the size of the processor's data caches.
 */
static void zlib_updatewindow(z_streamp strm, unsigned out)
{
    struct inflate_state *state;
    unsigned copy, dist;

    state = (struct inflate_state *)strm->state;

    /* copy state->wsize or less output bytes into the circular window */
    copy = out - strm->avail_out;
    if (copy >= state->wsize) {
        memcpy(state->window, strm->next_out - state->wsize, state->wsize);
        state->write = 0;
        state->whave = state->wsize;
    }
    else {
        dist = state->wsize - state->write;
        if (dist > copy) dist = copy;
        memcpy(state->window + state->write, strm->next_out - copy, dist);
        copy -= dist;
        if (copy) {
            memcpy(state->window, strm->next_out - copy, copy);
            state->write = copy;
            state->whave = state->wsize;
        }
        else {
            state->write += dist;
            if (state->write == state->wsize) state->write = 0;
            if (state->whave < state->wsize) state->whave += dist;
        }
    }
}


/*
 * At the end of a Deflate-compressed PPP packet, we expect to have seen
 * a `stored' block type value but not the (zero) length bytes.
 */
/*
   Returns true if inflate is currently at the end of a block generated by
   Z_SYNC_FLUSH or Z_FULL_FLUSH. This function is used by one PPP
   implementation to provide an additional safety check. PPP uses
   Z_SYNC_FLUSH but removes the length bytes of the resulting empty stored
   block. When decompressing, PPP checks that at the end of input packet,
   inflate is waiting for these length bytes.
 */
static int zlib_inflateSyncPacket(z_streamp strm)
{
    struct inflate_state *state;

    if (strm == NULL || strm->state == NULL) return Z_STREAM_ERROR;
    state = (struct inflate_state *)strm->state;

    if (state->mode == STORED && state->bits == 0) {
	state->mode = TYPE;
        return Z_OK;
    }
    return Z_DATA_ERROR;
}

/* Macros for inflate(): */

/* check function to use adler32() for zlib or crc32() for gzip */
#define UPDATE(check, buf, len) zlib_adler32(check, buf, len)

/* Load registers with state in inflate() for speed */
#define LOAD() \
    do { \
        put = strm->next_out; \
        left = strm->avail_out; \
        next = strm->next_in; \
        have = strm->avail_in; \
        hold = state->hold; \
        bits = state->bits; \
    } while (0)

/* Restore state from registers in inflate() */
#define RESTORE() \
    do { \
        strm->next_out = put; \
        strm->avail_out = left; \
        strm->next_in = next; \
        strm->avail_in = have; \
        state->hold = hold; \
        state->bits = bits; \
    } while (0)

/* Clear the input bit accumulator */
#define INITBITS() \
    do { \
        hold = 0; \
        bits = 0; \
    } while (0)

/* Get a byte of input into the bit accumulator, or return from inflate()
   if there is no input available. */
#define PULLBYTE() \
    do { \
        if (have == 0) goto inf_leave; \
        have--; \
        hold += (unsigned long)(*next++) << bits; \
        bits += 8; \
    } while (0)

/* Assure that there are at least n bits in the bit accumulator.  If there is
   not enough available input to do that, then return from inflate(). */
#define NEEDBITS(n) \
    do { \
        while (bits < (unsigned)(n)) \
            PULLBYTE(); \
    } while (0)

/* Return the low n bits of the bit accumulator (n < 16) */
#define BITS(n) \
    ((unsigned)hold & ((1U << (n)) - 1))

/* Remove n bits from the bit accumulator */
#define DROPBITS(n) \
    do { \
        hold >>= (n); \
        bits -= (unsigned)(n); \
    } while (0)

/* Remove zero to seven bits as needed to go to a byte boundary */
#define BYTEBITS() \
    do { \
        hold >>= bits & 7; \
        bits -= bits & 7; \
    } while (0)

/* Reverse the bytes in a 32-bit value */
#define REVERSE(q) \
    ((((q) >> 24) & 0xff) + (((q) >> 8) & 0xff00) + \
     (((q) & 0xff00) << 8) + (((q) & 0xff) << 24))

/*
   inflate() uses a state machine to process as much input data and generate as
   much output data as possible before returning.  The state machine is
   structured roughly as follows:

    for (;;) switch (state) {
    ...
    case STATEn:
        if (not enough input data or output space to make progress)
            return;
        ... make progress ...
        state = STATEm;
        break;
    ...
    }

   so when inflate() is called again, the same case is attempted again, and
   if the appropriate resources are provided, the machine proceeds to the
   next state.  The NEEDBITS() macro is usually the way the state evaluates
   whether it can proceed or should return.  NEEDBITS() does the return if
   the requested bits are not available.  The typical use of the BITS macros
   is:

        NEEDBITS(n);
        ... do something with BITS(n) ...
        DROPBITS(n);

   where NEEDBITS(n) either returns from inflate() if there isn't enough
   input left to load n bits into the accumulator, or it continues.  BITS(n)
   gives the low n bits in the accumulator.  When done, DROPBITS(n) drops
   the low n bits off the accumulator.  INITBITS() clears the accumulator
   and sets the number of available bits to zero.  BYTEBITS() discards just
   enough bits to put the accumulator on a byte boundary.  After BYTEBITS()
   and a NEEDBITS(8), then BITS(8) would return the next byte in the stream.

   NEEDBITS(n) uses PULLBYTE() to get an available byte of input, or to return
   if there is no input available.  The decoding of variable length codes uses
   PULLBYTE() directly in order to pull just enough bytes to decode the next
   code, and no more.

   Some states loop until they get enough input, making sure that enough
   state information is maintained to continue the loop where it left off
   if NEEDBITS() returns in the loop.  For example, want, need, and keep
   would all have to actually be part of the saved state in case NEEDBITS()
   returns:

    case STATEw:
        while (want < need) {
            NEEDBITS(n);
            keep[want++] = BITS(n);
            DROPBITS(n);
        }
        state = STATEx;
    case STATEx:

   As shown above, if the next state is also the next case, then the break
   is omitted.

   A state may also return if there is not enough output space available to
   complete that state.  Those states are copying stored data, writing a
   literal byte, and copying a matching string.

   When returning, a "goto inf_leave" is used to update the total counters,
   update the check value, and determine whether any progress has been made
   during that inflate() call in order to return the proper return code.
   Progress is defined as a change in either strm->avail_in or strm->avail_out.
   When there is a window, goto inf_leave will update the window with the last
   output written.  If a goto inf_leave occurs in the middle of decompression
   and there is no window currently, goto inf_leave will create one and copy
   output to the window for the next call of inflate().

   In this implementation, the flush parameter of inflate() only affects the
   return code (per zlib.h).  inflate() always writes as much as possible to
   strm->next_out, given the space available and the provided input--the effect
   documented in zlib.h of Z_SYNC_FLUSH.  Furthermore, inflate() always defers
   the allocation of and copying into a sliding window until necessary, which
   provides the effect documented in zlib.h for Z_FINISH when the entire input
   stream available.  So the only thing the flush parameter actually does is:
   when flush is set to Z_FINISH, inflate() cannot return Z_OK.  Instead it
   will return Z_BUF_ERROR if it has not reached the end of the stream.
 */

int zlib_inflate(z_streamp strm, int flush)
{
    struct inflate_state *state;
    const unsigned char *next;  /* next input */
    unsigned char *put;         /* next output */
    unsigned have, left;        /* available input and output */
    unsigned long hold;         /* bit buffer */
    unsigned bits;              /* bits in bit buffer */
    unsigned in, out;           /* save starting available input and output */
    unsigned copy;              /* number of stored or match bytes to copy */
    unsigned char *from;        /* where to copy match bytes from */
    code this;                  /* current decoding table entry */
    code last;                  /* parent table entry */
    unsigned len;               /* length to copy for repeats, bits to drop */
    int ret;                    /* return code */
    static const unsigned short order[19] = /* permutation of code lengths */
        {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

    /* Do not check for strm->next_out == NULL here as ppc zImage
       inflates to strm->next_out = 0 */

    if (strm == NULL || strm->state == NULL ||
        (strm->next_in == NULL && strm->avail_in != 0))
        return Z_STREAM_ERROR;

    state = (struct inflate_state *)strm->state;

    if (state->mode == TYPE) state->mode = TYPEDO;      /* skip check */
    LOAD();
    in = have;
    out = left;
    ret = Z_OK;
    for (;;)
        switch (state->mode) {
        case HEAD:
            if (state->wrap == 0) {
                state->mode = TYPEDO;
                break;
            }
            NEEDBITS(16);
            if (
                ((BITS(8) << 8) + (hold >> 8)) % 31) {
                strm->msg = (char *)"incorrect header check";
                state->mode = BAD;
                break;
            }
            if (BITS(4) != Z_DEFLATED) {
                strm->msg = (char *)"unknown compression method";
                state->mode = BAD;
                break;
            }
            DROPBITS(4);
            len = BITS(4) + 8;
            if (len > state->wbits) {
                strm->msg = (char *)"invalid window size";
                state->mode = BAD;
                break;
            }
            state->dmax = 1U << len;
            strm->adler = state->check = zlib_adler32(0L, NULL, 0);
            state->mode = hold & 0x200 ? DICTID : TYPE;
            INITBITS();
            break;
        case DICTID:
            NEEDBITS(32);
            strm->adler = state->check = REVERSE(hold);
            INITBITS();
            state->mode = DICT;
        case DICT:
            if (state->havedict == 0) {
                RESTORE();
                return Z_NEED_DICT;
            }
            strm->adler = state->check = zlib_adler32(0L, NULL, 0);
            state->mode = TYPE;
        case TYPE:
            if (flush == Z_BLOCK) goto inf_leave;
        case TYPEDO:
            if (state->last) {
                BYTEBITS();
                state->mode = CHECK;
                break;
            }
            NEEDBITS(3);
            state->last = BITS(1);
            DROPBITS(1);
            switch (BITS(2)) {
            case 0:                             /* stored block */
                state->mode = STORED;
                break;
            case 1:                             /* fixed block */
                zlib_fixedtables(state);
                state->mode = LEN;              /* decode codes */
                break;
            case 2:                             /* dynamic block */
                state->mode = TABLE;
                break;
            case 3:
                strm->msg = (char *)"invalid block type";
                state->mode = BAD;
            }
            DROPBITS(2);
            break;
        case STORED:
            BYTEBITS();                         /* go to byte boundary */
            NEEDBITS(32);
            if ((hold & 0xffff) != ((hold >> 16) ^ 0xffff)) {
                strm->msg = (char *)"invalid stored block lengths";
                state->mode = BAD;
                break;
            }
            state->length = (unsigned)hold & 0xffff;
            INITBITS();
            state->mode = COPY;
        case COPY:
            copy = state->length;
            if (copy) {
                if (copy > have) copy = have;
                if (copy > left) copy = left;
                if (copy == 0) goto inf_leave;
                memcpy(put, next, copy);
                have -= copy;
                next += copy;
                left -= copy;
                put += copy;
                state->length -= copy;
                break;
            }
            state->mode = TYPE;
            break;
        case TABLE:
            NEEDBITS(14);
            state->nlen = BITS(5) + 257;
            DROPBITS(5);
            state->ndist = BITS(5) + 1;
            DROPBITS(5);
            state->ncode = BITS(4) + 4;
            DROPBITS(4);
#ifndef PKZIP_BUG_WORKAROUND
            if (state->nlen > 286 || state->ndist > 30) {
                strm->msg = (char *)"too many length or distance symbols";
                state->mode = BAD;
                break;
            }
#endif
            state->have = 0;
            state->mode = LENLENS;
        case LENLENS:
            while (state->have < state->ncode) {
                NEEDBITS(3);
                state->lens[order[state->have++]] = (unsigned short)BITS(3);
                DROPBITS(3);
            }
            while (state->have < 19)
                state->lens[order[state->have++]] = 0;
            state->next = state->codes;
            state->lencode = (code const *)(state->next);
            state->lenbits = 7;
            ret = zlib_inflate_table(CODES, state->lens, 19, &(state->next),
                                &(state->lenbits), state->work);
            if (ret) {
                strm->msg = (char *)"invalid code lengths set";
                state->mode = BAD;
                break;
            }
            state->have = 0;
            state->mode = CODELENS;
        case CODELENS:
            while (state->have < state->nlen + state->ndist) {
                for (;;) {
                    this = state->lencode[BITS(state->lenbits)];
                    if ((unsigned)(this.bits) <= bits) break;
                    PULLBYTE();
                }
                if (this.val < 16) {
                    NEEDBITS(this.bits);
                    DROPBITS(this.bits);
                    state->lens[state->have++] = this.val;
                }
                else {
                    if (this.val == 16) {
                        NEEDBITS(this.bits + 2);
                        DROPBITS(this.bits);
                        if (state->have == 0) {
                            strm->msg = (char *)"invalid bit length repeat";
                            state->mode = BAD;
                            break;
                        }
                        len = state->lens[state->have - 1];
                        copy = 3 + BITS(2);
                        DROPBITS(2);
                    }
                    else if (this.val == 17) {
                        NEEDBITS(this.bits + 3);
                        DROPBITS(this.bits);
                        len = 0;
                        copy = 3 + BITS(3);
                        DROPBITS(3);
                    }
                    else {
                        NEEDBITS(this.bits + 7);
                        DROPBITS(this.bits);
                        len = 0;
                        copy = 11 + BITS(7);
                        DROPBITS(7);
                    }
                    if (state->have + copy > state->nlen + state->ndist) {
                        strm->msg = (char *)"invalid bit length repeat";
                        state->mode = BAD;
                        break;
                    }
                    while (copy--)
                        state->lens[state->have++] = (unsigned short)len;
                }
            }

            /* handle error breaks in while */
            if (state->mode == BAD) break;

            /* build code tables */
            state->next = state->codes;
            state->lencode = (code const *)(state->next);
            state->lenbits = 9;
            ret = zlib_inflate_table(LENS, state->lens, state->nlen, &(state->next),
                                &(state->lenbits), state->work);
            if (ret) {
                strm->msg = (char *)"invalid literal/lengths set";
                state->mode = BAD;
                break;
            }
            state->distcode = (code const *)(state->next);
            state->distbits = 6;
            ret = zlib_inflate_table(DISTS, state->lens + state->nlen, state->ndist,
                            &(state->next), &(state->distbits), state->work);
            if (ret) {
                strm->msg = (char *)"invalid distances set";
                state->mode = BAD;
                break;
            }
            state->mode = LEN;
        case LEN:
            if (have >= 6 && left >= 258) {
                RESTORE();
                inflate_fast(strm, out);
                LOAD();
                break;
            }
            for (;;) {
                this = state->lencode[BITS(state->lenbits)];
                if ((unsigned)(this.bits) <= bits) break;
                PULLBYTE();
            }
            if (this.op && (this.op & 0xf0) == 0) {
                last = this;
                for (;;) {
                    this = state->lencode[last.val +
                            (BITS(last.bits + last.op) >> last.bits)];
                    if ((unsigned)(last.bits + this.bits) <= bits) break;
                    PULLBYTE();
                }
                DROPBITS(last.bits);
            }
            DROPBITS(this.bits);
            state->length = (unsigned)this.val;
            if ((int)(this.op) == 0) {
                state->mode = LIT;
                break;
            }
            if (this.op & 32) {
                state->mode = TYPE;
                break;
            }
            if (this.op & 64) {
                strm->msg = (char *)"invalid literal/length code";
                state->mode = BAD;
                break;
            }
            state->extra = (unsigned)(this.op) & 15;
            state->mode = LENEXT;
        case LENEXT:
            if (state->extra) {
                NEEDBITS(state->extra);
                state->length += BITS(state->extra);
                DROPBITS(state->extra);
            }
            state->mode = DIST;
        case DIST:
            for (;;) {
                this = state->distcode[BITS(state->distbits)];
                if ((unsigned)(this.bits) <= bits) break;
                PULLBYTE();
            }
            if ((this.op & 0xf0) == 0) {
                last = this;
                for (;;) {
                    this = state->distcode[last.val +
                            (BITS(last.bits + last.op) >> last.bits)];
                    if ((unsigned)(last.bits + this.bits) <= bits) break;
                    PULLBYTE();
                }
                DROPBITS(last.bits);
            }
            DROPBITS(this.bits);
            if (this.op & 64) {
                strm->msg = (char *)"invalid distance code";
                state->mode = BAD;
                break;
            }
            state->offset = (unsigned)this.val;
            state->extra = (unsigned)(this.op) & 15;
            state->mode = DISTEXT;
        case DISTEXT:
            if (state->extra) {
                NEEDBITS(state->extra);
                state->offset += BITS(state->extra);
                DROPBITS(state->extra);
            }
#ifdef INFLATE_STRICT
            if (state->offset > state->dmax) {
                strm->msg = (char *)"invalid distance too far back";
                state->mode = BAD;
                break;
            }
#endif
            if (state->offset > state->whave + out - left) {
                strm->msg = (char *)"invalid distance too far back";
                state->mode = BAD;
                break;
            }
            state->mode = MATCH;
        case MATCH:
            if (left == 0) goto inf_leave;
            copy = out - left;
            if (state->offset > copy) {         /* copy from window */
                copy = state->offset - copy;
                if (copy > state->write) {
                    copy -= state->write;
                    from = state->window + (state->wsize - copy);
                }
                else
                    from = state->window + (state->write - copy);
                if (copy > state->length) copy = state->length;
            }
            else {                              /* copy from output */
                from = put - state->offset;
                copy = state->length;
            }
            if (copy > left) copy = left;
            left -= copy;
            state->length -= copy;
            do {
                *put++ = *from++;
            } while (--copy);
            if (state->length == 0) state->mode = LEN;
            break;
        case LIT:
            if (left == 0) goto inf_leave;
            *put++ = (unsigned char)(state->length);
            left--;
            state->mode = LEN;
            break;
        case CHECK:
            if (state->wrap) {
                NEEDBITS(32);
                out -= left;
                strm->total_out += out;
                state->total += out;
                if (out)
                    strm->adler = state->check =
                        UPDATE(state->check, put - out, out);
                out = left;
                if ((
                     REVERSE(hold)) != state->check) {
                    strm->msg = (char *)"incorrect data check";
                    state->mode = BAD;
                    break;
                }
                INITBITS();
            }
            state->mode = DONE;
        case DONE:
            ret = Z_STREAM_END;
            goto inf_leave;
        case BAD:
            ret = Z_DATA_ERROR;
            goto inf_leave;
        case MEM:
            return Z_MEM_ERROR;
        case SYNC:
        default:
            return Z_STREAM_ERROR;
        }

    /*
       Return from inflate(), updating the total counts and the check value.
       If there was no progress during the inflate() call, return a buffer
       error.  Call zlib_updatewindow() to create and/or update the window state.
     */
  inf_leave:
    RESTORE();
    if (state->wsize || (state->mode < CHECK && out != strm->avail_out))
        zlib_updatewindow(strm, out);

    in -= strm->avail_in;
    out -= strm->avail_out;
    strm->total_in += in;
    strm->total_out += out;
    state->total += out;
    if (state->wrap && out)
        strm->adler = state->check =
            UPDATE(state->check, strm->next_out - out, out);

    strm->data_type = state->bits + (state->last ? 64 : 0) +
                      (state->mode == TYPE ? 128 : 0);

    if (flush == Z_PACKET_FLUSH && ret == Z_OK &&
            strm->avail_out != 0 && strm->avail_in == 0)
		return zlib_inflateSyncPacket(strm);

    if (((in == 0 && out == 0) || flush == Z_FINISH) && ret == Z_OK)
        ret = Z_BUF_ERROR;

    return ret;
}

int zlib_inflateEnd(z_streamp strm)
{
    if (strm == NULL || strm->state == NULL)
        return Z_STREAM_ERROR;
    return Z_OK;
}

#if 0
int zlib_inflateSetDictionary(z_streamp strm, const Byte *dictionary,
        uInt dictLength)
{
    struct inflate_state *state;
    unsigned long id;

    /* check state */
    if (strm == NULL || strm->state == NULL) return Z_STREAM_ERROR;
    state = (struct inflate_state *)strm->state;
    if (state->wrap != 0 && state->mode != DICT)
        return Z_STREAM_ERROR;

    /* check for correct dictionary id */
    if (state->mode == DICT) {
        id = zlib_adler32(0L, NULL, 0);
        id = zlib_adler32(id, dictionary, dictLength);
        if (id != state->check)
            return Z_DATA_ERROR;
    }

    /* copy dictionary to window */
    zlib_updatewindow(strm, strm->avail_out);

    if (dictLength > state->wsize) {
        memcpy(state->window, dictionary + dictLength - state->wsize,
                state->wsize);
        state->whave = state->wsize;
    }
    else {
        memcpy(state->window + state->wsize - dictLength, dictionary,
                dictLength);
        state->whave = dictLength;
    }
    state->havedict = 1;
    return Z_OK;
}
#endif

#if 0
/*
   Search buf[0..len-1] for the pattern: 0, 0, 0xff, 0xff.  Return when found
   or when out of input.  When called, *have is the number of pattern bytes
   found in order so far, in 0..3.  On return *have is updated to the new
   state.  If on return *have equals four, then the pattern was found and the
   return value is how many bytes were read including the last byte of the
   pattern.  If *have is less than four, then the pattern has not been found
   yet and the return value is len.  In the latter case, zlib_syncsearch() can be
   called again with more data and the *have state.  *have is initialized to
   zero for the first call.
 */
static unsigned zlib_syncsearch(unsigned *have, unsigned char *buf,
        unsigned len)
{
    unsigned got;
    unsigned next;

    got = *have;
    next = 0;
    while (next < len && got < 4) {
        if ((int)(buf[next]) == (got < 2 ? 0 : 0xff))
            got++;
        else if (buf[next])
            got = 0;
        else
            got = 4 - got;
        next++;
    }
    *have = got;
    return next;
}
#endif

#if 0
int zlib_inflateSync(z_streamp strm)
{
    unsigned len;               /* number of bytes to look at or looked at */
    unsigned long in, out;      /* temporary to save total_in and total_out */
    unsigned char buf[4];       /* to restore bit buffer to byte string */
    struct inflate_state *state;

    /* check parameters */
    if (strm == NULL || strm->state == NULL) return Z_STREAM_ERROR;
    state = (struct inflate_state *)strm->state;
    if (strm->avail_in == 0 && state->bits < 8) return Z_BUF_ERROR;

    /* if first time, start search in bit buffer */
    if (state->mode != SYNC) {
        state->mode = SYNC;
        state->hold <<= state->bits & 7;
        state->bits -= state->bits & 7;
        len = 0;
        while (state->bits >= 8) {
            buf[len++] = (unsigned char)(state->hold);
            state->hold >>= 8;
            state->bits -= 8;
        }
        state->have = 0;
        zlib_syncsearch(&(state->have), buf, len);
    }

    /* search available input */
    len = zlib_syncsearch(&(state->have), strm->next_in, strm->avail_in);
    strm->avail_in -= len;
    strm->next_in += len;
    strm->total_in += len;

    /* return no joy or set up to restart inflate() on a new block */
    if (state->have != 4) return Z_DATA_ERROR;
    in = strm->total_in;  out = strm->total_out;
    zlib_inflateReset(strm);
    strm->total_in = in;  strm->total_out = out;
    state->mode = TYPE;
    return Z_OK;
}
#endif

/*
 * This subroutine adds the data at next_in/avail_in to the output history
 * without performing any output.  The output buffer must be "caught up";
 * i.e. no pending output but this should always be the case. The state must
 * be waiting on the start of a block (i.e. mode == TYPE or HEAD).  On exit,
 * the output will also be caught up, and the checksum will have been updated
 * if need be.
 */
int zlib_inflateIncomp(z_stream *z)
{
    struct inflate_state *state = (struct inflate_state *)z->state;
    Byte *saved_no = z->next_out;
    uInt saved_ao = z->avail_out;

    if (state->mode != TYPE && state->mode != HEAD)
	return Z_DATA_ERROR;

    /* Setup some variables to allow misuse of updateWindow */
    z->avail_out = 0;
    z->next_out = (unsigned char*)z->next_in + z->avail_in;

    zlib_updatewindow(z, z->avail_in);

    /* Restore saved variables */
    z->avail_out = saved_ao;
    z->next_out = saved_no;

    z->adler = state->check =
        UPDATE(state->check, z->next_in, z->avail_in);

    z->total_out += z->avail_in;
    z->total_in += z->avail_in;
    z->next_in += z->avail_in;
    state->total += z->avail_in;
    z->avail_in = 0;

    return Z_OK;
}
