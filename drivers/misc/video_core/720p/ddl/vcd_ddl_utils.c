/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Alternatively, and instead of the terms immediately above, this
 * software may be relicensed by the recipient at their option under the
 * terms of the GNU General Public License version 2 ("GPL") and only
 * version 2.  If the recipient chooses to relicense the software under
 * the GPL, then the recipient shall replace all of the text immediately
 * above and including this paragraph with the text immediately below
 * and between the words START OF ALTERNATE GPL TERMS and END OF
 * ALTERNATE GPL TERMS and such notices and license terms shall apply
 * INSTEAD OF the notices and licensing terms given above.
 *
 * START OF ALTERNATE GPL TERMS
 *
 * Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *
 * This software was originally licensed under the Code Aurora Forum
 * Inc. Dual BSD/GPL License version 1.1 and relicensed as permitted
 * under the terms thereof by a recipient under the General Public
 * License Version 2.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * END OF ALTERNATE GPL TERMS
 *
 */

#include "video_core_type.h"
#include "vcd_ddl_utils.h"

#define DEBUG 0

#if DEBUG
#define DBG(x...) printk(KERN_DEBUG x)
#else
#define DBG(x...)
#endif


static unsigned int g_ddl_dec_t1, g_ddl_enc_t1;
static unsigned int g_ddl_dec_tavg, g_ddl_enc_tavg;
static unsigned int g_ddl_dec_count, g_ddl_enc_count;

#ifdef NO_IN_KERNEL_PMEM

void ddl_pmem_alloc(struct ddl_buf_addr_type *buff_addr, u32 size, u32 align)
{
	u32 n_guard_bytes, n_align_mask;
	u32 n_physical_addr, n_align_offset;
	dma_addr_t phy_addr;

	if (DDL_LINEAR_BUFFER_ALIGN_BYTES == align) {

		n_guard_bytes = 31;
		n_align_mask = 0xFFFFFFE0U;

	} else {

		n_guard_bytes = DDL_TILE_BUF_ALIGN_GUARD_BYTES;
		n_align_mask = DDL_TILE_BUF_ALIGN_MASK;
	}

	buff_addr->p_virtual_base_addr =
		kmalloc((size + n_guard_bytes), GFP_KERNEL);

	if (buff_addr->p_virtual_base_addr == NULL) {

		DBG(KERN_ERR "ERROR %s:%u kamlloc failes to allocate"
		       " size + n_guard_bytes = %u\n", __func__, __LINE__,
		       (size + n_guard_bytes));
		return;
	}

	phy_addr = dma_map_single(NULL, buff_addr->p_virtual_base_addr,
				  size + n_guard_bytes, DMA_TO_DEVICE);

	buff_addr->n_buffer_size = size;
	n_physical_addr = (u32) phy_addr;
	buff_addr->p_align_physical_addr =
	    (u32 *) ((n_physical_addr + n_guard_bytes) & n_align_mask);
	n_align_offset =
	    (u32) (buff_addr->p_align_physical_addr) - n_physical_addr;
	buff_addr->p_align_virtual_addr =
	    (u32 *) ((u32) (buff_addr->p_virtual_base_addr)
		     + n_align_offset);
}

void ddl_pmem_free(struct ddl_buf_addr_type buff_addr)
{
	if (buff_addr.p_virtual_base_addr != 0) {
		kfree(buff_addr.p_virtual_base_addr);
		buff_addr.n_buffer_size = 0;
		buff_addr.p_virtual_base_addr = NULL;
	}
}

#else

void ddl_pmem_alloc(struct ddl_buf_addr_type *buff_addr, u32 size, u32 align)
{
	u32 n_guard_bytes, n_align_mask;
	s32 n_physical_addr;
	u32 n_align_offset;

	if (DDL_LINEAR_BUFFER_ALIGN_BYTES == align) {

		n_guard_bytes = 31;
		n_align_mask = 0xFFFFFFE0U;

	} else {

		n_guard_bytes = DDL_TILE_BUF_ALIGN_GUARD_BYTES;
		n_align_mask = DDL_TILE_BUF_ALIGN_MASK;
	}

	n_physical_addr = pmem_kalloc((size + n_guard_bytes),
				      PMEM_MEMTYPE_EBI1 | PMEM_ALIGNMENT_4K);
	buff_addr->p_physical_base_addr = (u32 *)n_physical_addr;

	if (IS_ERR((void *)n_physical_addr)) {
		pr_err("%s(): could not allocte in kernel pmem buffers\n",
		       __func__);
		return;
	}

	buff_addr->p_virtual_base_addr =
	    (u32 *) ioremap((unsigned long)n_physical_addr,
			    size + n_guard_bytes);
	memset(buff_addr->p_virtual_base_addr, 0 , size + n_guard_bytes);
	if (buff_addr->p_virtual_base_addr == NULL) {

		pr_err("%s: could not ioremap in kernel pmem buffers\n",
		       __func__);
		pmem_kfree(n_physical_addr);
		return;
	}

	buff_addr->n_buffer_size = size;

	buff_addr->p_align_physical_addr =
	    (u32 *) ((n_physical_addr + n_guard_bytes) & n_align_mask);

	n_align_offset =
	    (u32) (buff_addr->p_align_physical_addr) - n_physical_addr;

	buff_addr->p_align_virtual_addr =
	    (u32 *) ((u32) (buff_addr->p_virtual_base_addr)
		     + n_align_offset);

	pr_debug("%s(): phy addr 0x%08x kernel addr 0x%08x\n", __func__,
		 (u32) buff_addr->p_align_physical_addr,
		 (u32) buff_addr->p_align_physical_addr);

	return;
}

void ddl_pmem_free(struct ddl_buf_addr_type buff_addr)
{
	DBG("\n %s(): ddl_pmem_free v_address %p p_address %p",
			__func__, buff_addr.p_physical_base_addr,
			buff_addr.p_virtual_base_addr);

	if (buff_addr.p_virtual_base_addr)
		iounmap((void *)buff_addr.p_virtual_base_addr);

	if ((buff_addr.p_physical_base_addr != NULL) &&
		pmem_kfree((s32) buff_addr.p_physical_base_addr)) {
		DBG("\n %s(): Error in Freeing ddl_pmem_free "
		"Physical Address %p", __func__,
		buff_addr.p_physical_base_addr);
	}

	buff_addr.n_buffer_size = 0;
	buff_addr.p_virtual_base_addr = NULL;
}
#endif

void ddl_get_core_start_time(u8 codec_type)
{
	u32 *p_ddl_t1 = NULL;
	if (codec_type == 0)
		p_ddl_t1 = &g_ddl_dec_t1;
	else if (codec_type == 1)
		p_ddl_t1 = &g_ddl_enc_t1;

	if (*p_ddl_t1 == 0) {
		struct timeval ddl_tv;
		do_gettimeofday(&ddl_tv);
		*p_ddl_t1 = (ddl_tv.tv_sec * 1000) + (ddl_tv.tv_usec / 1000);
	}
}

void ddl_calc_core_time(u8 codec_type)
{
	u32 *p_ddl_t1 = NULL, *p_ddl_tavg = NULL,
		*p_ddl_count = NULL;
	if (codec_type == 0) {
		DBG("\n720p Core Decode ");
		p_ddl_t1 = &g_ddl_dec_t1;
		p_ddl_tavg = &g_ddl_dec_tavg;
		p_ddl_count = &g_ddl_dec_count;
	} else if (codec_type == 1) {
		DBG("\n720p Core Encode ");
		p_ddl_t1 = &g_ddl_enc_t1;
		p_ddl_tavg = &g_ddl_enc_tavg;
		p_ddl_count = &g_ddl_enc_count;
	}

	if (*p_ddl_t1) {
		int ddl_t2;
		struct timeval ddl_tv;
		do_gettimeofday(&ddl_tv);
		ddl_t2 = (ddl_tv.tv_sec * 1000) + (ddl_tv.tv_usec / 1000);
		*p_ddl_tavg = ((ddl_t2 - *p_ddl_t1) + *p_ddl_tavg *
					 *p_ddl_count) / (*p_ddl_count + 1);
		*p_ddl_count = *p_ddl_count + 1;
		DBG("time %u, average time %u, count %u",
			ddl_t2 - *p_ddl_t1, *p_ddl_tavg, *p_ddl_count);
		*p_ddl_t1 = 0;
	}
}

void ddl_reset_time_variables(u8 codec_type)
{
	if (codec_type == 0) {
		DBG("\n reset Decoder time variables");
		g_ddl_dec_t1 = 0;
		g_ddl_dec_tavg = 0;
		g_ddl_dec_count = 0;
	} else if (codec_type == 1) {
		DBG("\n reset Encoder time variables ");
		g_ddl_enc_t1 = 0;
		g_ddl_enc_tavg = 0;
		g_ddl_enc_count = 0;
	}
}
