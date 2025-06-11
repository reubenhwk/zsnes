/*
 * Copyright (C) 1997-2008 ZSNES Team ( zsKnight, _Demo_, pagefault, Nach )
 *
 * http://www.zsnes.com
 * http://sourceforge.net/projects/zsnes
 * https://zsnes.bountysource.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <string.h>

#include "../c_init.h"
#include "../cpu/regs.h"
#include "../link.h"
#include "sw_draw.h"

u1* ScreenPtr;
u1* SurfBufD;

void ClearWin16(void)
{
    u1* edi = SurfBufD;
    s4 ebx = 0;
    do
        memset(edi, 0, SurfaceX * 2);
    while (edi += pitch, ++ebx != SurfaceY);
}

void ClearWin32(void)
{
    u1* edi = SurfBufD;
    s4 ebx = 0;
    do
        memset(edi, 0, SurfaceX * 4);
    while (edi += pitch, ++ebx != SurfaceY);
}

void DrawWin256x224x16(void)
{
    u1 const* esi = ScreenPtr;
    u1* edi = SurfBufD;
    u4 eax = 0;
#ifdef __WIN32__
    u4 const edx = resolutn;
#endif
    do {
        memcpy(edi, esi, 512);
        edi += pitch;
        esi += 576;
    }
#ifdef __WIN32__
    while (++eax != edx);
#else
    while (++eax != 223);
#endif
    memset(edi, 0, 512);
}

#ifdef __WIN32__

void DrawWin256x224x32(void)
{
    u4 eax = 0;
    u4 const edx = resolutn;
    u1 const* esi = ScreenPtr;
    u1* edi = SurfBufD;
    u8 const mm4 = copymaskRB;
    u8 const mm5 = copymaskG;
    u8 const mm6 = copymagic;
    do {
        u4 ecx = 64;
        do {
            asm(
                "movq (%0), %%mm0\n\t"
                "movq %%mm0, %%mm1\n\t"
                "punpcklwd %%mm0, %%mm0\n\t"
                "movq %%mm0, %%mm2\n\t"
                "pand %2, %%mm0\n\t"
                "pmaddwd %4, %%mm0\n\t"
                "punpckhwd %%mm1, %%mm1\n\t"
                "movq %%mm1, %%mm3\n\t"
                "pand %2, %%mm1\n\t"
                "pmaddwd %4, %%mm1\n\t"
                "pslld $5, %%mm2\n\t"
                "pslld $5, %%mm3\n\t"
                "pand %3, %%mm2\n\t"
                "pand %3, %%mm3\n\t"
                "por  %%mm2, %%mm0\n\t"
                "por  %%mm3, %%mm1\n\t"
                "movq %%mm0,  (%1)\n\t"
                "movq %%mm1, 8(%1)" ::"r"(esi),
                "r"(edi), "y"(mm4), "y"(mm5), "y"(mm6)
                : "memory", "mm0", "mm1", "mm2", "mm3");
            esi += 8;
            edi += 16;
        } while (--ecx != 0);
        edi += pitch - 1024;
        esi += 576 - 512;
    } while (++eax != edx);
    asm volatile("emms");
}

#endif

void DrawWin320x240x16(void)
{
    u1 const* esi = ScreenPtr;
    u1* edi = SurfBufD;
    u4 const edx = resolutn;

    {
        u4 ebx = 0;
        do
            memset(edi, 0, 640);
        while (edi += pitch, ++ebx != 8);
    }

    {
        u4 ebx = 0;
        do {
            memset(edi, 0, 64);
            memcpy(edi + 64, esi, 512);
            memset(edi + 64 + 512, 0, 64);
        } while (esi += 576, edi += pitch, ++ebx != edx);
    }

    memset(edi, 0, 512);
}
