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

DDL_INLINE struct ddl_context_type *ddl_get_context(void)
{
	static struct ddl_context_type ddl_context;
	return &ddl_context;
}

DDL_INLINE void ddl_move_client_state(struct ddl_client_context_type *p_ddl,
				      enum ddl_client_state_type e_client_state)
{
	p_ddl->e_client_state = e_client_state;
}

DDL_INLINE void ddl_move_command_state(struct ddl_context_type *p_ddl_context,
				       enum ddl_cmd_state_type e_command_state)
{
	p_ddl_context->e_cmd_state = e_command_state;
}

u32 ddl_client_transact(u32 operation,
			struct ddl_client_context_type **pddl_client)
{
	u32 ret_status = VCD_ERR_FAIL;
	u32 n_counter;
	struct ddl_context_type *p_ddl_context;

	p_ddl_context = ddl_get_context();
	switch (operation) {
	case DDL_FREE_CLIENT:
		{
			if (NULL != pddl_client && NULL != *pddl_client) {
				u32 n_channel_id;
				n_channel_id = (*pddl_client)->n_channel_id;
				if (n_channel_id < VCD_MAX_NO_CLIENT) {
					p_ddl_context->
					    a_ddl_clients[n_channel_id] = NULL;
				} else {
					VIDC_LOG_STRING("CHID_CORRUPTION");
				}
				DDL_FREE(*pddl_client);
				ret_status = VCD_S_SUCCESS;
			}
			break;
		}
	case DDL_GET_CLIENT:
		{
			ret_status = VCD_ERR_MAX_CLIENT;
			for (n_counter = 0; n_counter < VCD_MAX_NO_CLIENT &&
			     ret_status == VCD_ERR_MAX_CLIENT; ++n_counter) {
				if (NULL ==
				    p_ddl_context->a_ddl_clients[n_counter]) {
					*pddl_client =
					    (struct ddl_client_context_type *)
					    DDL_MALLOC(sizeof
					       (struct ddl_client_context_type)
					       );
					if (NULL == *pddl_client) {
						ret_status = VCD_ERR_ALLOC_FAIL;
					} else {
						DDL_MEMSET(*pddl_client, 0,
						   sizeof(struct
						   ddl_client_context_type));
						p_ddl_context->
						    a_ddl_clients[n_counter] =
						    *pddl_client;
						(*pddl_client)->n_channel_id =
						    n_counter;
						(*pddl_client)->p_ddl_context =
						    p_ddl_context;
						ret_status = VCD_S_SUCCESS;
					}
				}
			}
			break;
		}
	case DDL_INIT_CLIENTS:
		{
			for (n_counter = 0; n_counter < VCD_MAX_NO_CLIENT;
			     ++n_counter) {
				p_ddl_context->a_ddl_clients[n_counter] = NULL;
			}
			ret_status = VCD_S_SUCCESS;
			break;
		}
	case DDL_ACTIVE_CLIENT:
		{
			for (n_counter = 0; n_counter < VCD_MAX_NO_CLIENT;
			     ++n_counter) {
				if (NULL !=
				    p_ddl_context->a_ddl_clients[n_counter]) {
					ret_status = VCD_S_SUCCESS;
					break;
				}
			}
			break;
		}
	default:
		{
			ret_status = VCD_ERR_ILLEGAL_PARM;
			break;
		}
	}
	return ret_status;
}

u32 ddl_decoder_dpb_transact(struct ddl_decoder_data_type *p_decoder,
			     struct ddl_frame_data_type_tag *p_in_out_frame,
			     u32 n_operation)
{
	u32 vcd_status = VCD_S_SUCCESS;
	u32 n_loopc;
	struct ddl_frame_data_type_tag *p_found_frame = NULL;
	struct ddl_mask_type *p_dpb_mask = &p_decoder->dpb_mask;

	switch (n_operation) {
	case DDL_DPB_OP_MARK_BUSY:
	case DDL_DPB_OP_MARK_FREE:
		{
			for (n_loopc = 0; NULL == p_found_frame &&
			     n_loopc < p_decoder->dp_buf.n_no_of_dec_pic_buf;
			     ++n_loopc) {
				if (p_in_out_frame->vcd_frm.p_physical ==
				    p_decoder->dp_buf.
				    a_dec_pic_buffers[n_loopc].vcd_frm.
				    p_physical) {
					p_found_frame =
					    &(p_decoder->dp_buf.
					      a_dec_pic_buffers[n_loopc]);
					break;
				}
			}

			if (NULL != p_found_frame) {
				if (DDL_DPB_OP_MARK_BUSY == n_operation) {
					p_dpb_mask->n_hw_mask &=
					    (~(0x1 << n_loopc));
					*p_in_out_frame = *p_found_frame;
				} else if (DDL_DPB_OP_MARK_FREE ==
						n_operation) {
					p_dpb_mask->n_client_mask |=
					    (0x1 << n_loopc);
					*p_found_frame = *p_in_out_frame;
				}
			} else {
				p_in_out_frame->vcd_frm.p_physical = NULL;
				vcd_status = VCD_ERR_BAD_POINTER;
				VIDC_LOG_STRING("BUF_NOT_FOUND");
			}
			break;
		}
	case DDL_DPB_OP_SET_MASK:
		{
			p_dpb_mask->n_hw_mask |= p_dpb_mask->n_client_mask;
			p_dpb_mask->n_client_mask = 0;
			vidc_720p_decode_set_dpb_release_buffer_mask
			    (p_dpb_mask->n_hw_mask);
			break;
		}
	case DDL_DPB_OP_INIT:
		{
			u32 n_dpb_size;
			n_dpb_size = (0 == p_decoder->n_meta_data_offset) ?
			    p_decoder->dp_buf.a_dec_pic_buffers[0].vcd_frm.
			    n_alloc_len : p_decoder->n_meta_data_offset;
			vidc_720p_decode_set_dpb_details(p_decoder->dp_buf.
						  n_no_of_dec_pic_buf,
						  n_dpb_size,
						  p_decoder->ref_buffer.
						  p_align_physical_addr);
			for (n_loopc = 0;
			     n_loopc < p_decoder->dp_buf.n_no_of_dec_pic_buf;
			     ++n_loopc) {
				vidc_720p_decode_set_dpb_buffers(n_loopc,
							  (u32 *)
							  p_decoder->
							  dp_buf.
							  a_dec_pic_buffers
							  [n_loopc].
							  vcd_frm.
							  p_physical);
				VIDC_LOG1("DEC_DPB_BUFn_SIZE",
					   p_decoder->dp_buf.
					   a_dec_pic_buffers[n_loopc].vcd_frm.
					   n_alloc_len);
			}
			break;
		}
	case DDL_DPB_OP_RETRIEVE:
		{
			u32 n_position;
			if (0 != p_dpb_mask->n_client_mask) {
				n_position = 0x1;
				for (n_loopc = 0;
				     n_loopc <
				     p_decoder->dp_buf.n_no_of_dec_pic_buf
				     && p_found_frame == NULL; ++n_loopc) {
					if (p_dpb_mask->
					    n_client_mask & n_position) {
						p_found_frame =
						    &p_decoder->dp_buf.
						    a_dec_pic_buffers[n_loopc];
						p_dpb_mask->n_client_mask &=
						    ~(n_position);
					}
					n_position <<= 1;
				}
			} else if (0 != p_dpb_mask->n_hw_mask) {
				n_position = 0x1;
				for (n_loopc = 0;
				     n_loopc <
				     p_decoder->dp_buf.n_no_of_dec_pic_buf
				     && p_found_frame == NULL; ++n_loopc) {
					if (p_dpb_mask->n_hw_mask
							& n_position) {
						p_found_frame =
						    &p_decoder->dp_buf.
						    a_dec_pic_buffers[n_loopc];
						p_dpb_mask->n_hw_mask &=
						    ~(n_position);
					}
					n_position <<= 1;
				}
			}
			if (NULL != p_found_frame)
				*p_in_out_frame = *p_found_frame;
			else
				p_in_out_frame->vcd_frm.p_physical = NULL;
			break;
		}
	}
	return vcd_status;
}

void ddl_release_context_buffers(struct ddl_context_type *p_ddl_context)
{
	ddl_pmem_free(p_ddl_context->context_buf_addr);
	ddl_pmem_free(p_ddl_context->db_line_buffer);
	ddl_pmem_free(p_ddl_context->data_partition_tempbuf);
	ddl_pmem_free(p_ddl_context->metadata_shared_input);
	ddl_pmem_free(p_ddl_context->dbg_core_dump);

	vcd_fw_release();
}

void ddl_release_client_internal_buffers(struct ddl_client_context_type *p_ddl)
{
	if (p_ddl->b_decoding) {
		struct ddl_decoder_data_type *p_decoder =
		    &(p_ddl->codec_data.decoder);
		ddl_pmem_free(p_decoder->h264Vsp_temp_buffer);
		ddl_pmem_free(p_decoder->dpb_comv_buffer);
		ddl_pmem_free(p_decoder->ref_buffer);
		DDL_FREE(p_decoder->dp_buf.a_dec_pic_buffers);
		ddl_decode_dynamic_property(p_ddl, FALSE);
		p_decoder->decode_config.n_sequence_header_len = 0;
		p_decoder->decode_config.p_sequence_header = NULL;
		p_decoder->dpb_mask.n_client_mask = 0;
		p_decoder->dpb_mask.n_hw_mask = 0;
		p_decoder->dp_buf.n_no_of_dec_pic_buf = 0;
		p_decoder->n_dynamic_prop_change = 0;

	} else {
		struct ddl_encoder_data_type *p_encoder =
		    &(p_ddl->codec_data.encoder);
		ddl_pmem_free(p_encoder->enc_dpb_addr);
		ddl_pmem_free(p_encoder->seq_header);
		ddl_encode_dynamic_property(p_ddl, FALSE);
		p_encoder->n_dynamic_prop_change = 0;
	}
}
