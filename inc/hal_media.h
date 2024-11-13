/*
 * Copyright 2023 Ethan. All rights reserved.
 */

#ifndef __HAL_MEDIA_H__
#define __HAL_MEDIA_H__
#include <stdint.h>

#define AI_DEV_MAX 2
#define AO_DEV_MAX 2
#define AI_CHN_MAX 255
#define AO_CHN_MAX 2

typedef enum hal_media_device_name {
	HMDN_MIC = 1,
	HMDN_SPK,
	HMDN_FCAM,
	HMDN_BCAM
} hal_media_device_name_e;

typedef enum hal_frame_type {
	HFT_I = 1,
	HFT_B,
	HFT_P
} hal_frame_type_e;

typedef enum hal_enc_type {
	HET_UNKNOWN = 0,
	HET_PCM = 1,
	HET_PCMA,
	HET_PCMU,
	HET_OPUS,
	HET_AAC,
	HET_H264,
	HET_H265,
	HET_MAX
} hal_enc_type_e;

typedef struct hal_frame {
	hal_enc_type_e		m_enc_type;
	hal_frame_type_e	m_frame_type;
	uint8_t			*m_data;
	uint32_t		m_len;
	uint64_t		pts;
	uint64_t		dts;
} hal_frame_t;

typedef enum hal_stream_type {
	HST_UNKNOWN = 0,
	HST_MAIN = 1,
	HST_SUB = 2,
	HST_MAX
} hal_stream_type_e;

typedef struct hal_stream {
	hal_frame_t		m_frame;
	uint8_t			m_chn;
	hal_stream_type_e	m_stream_type;
} hal_stream_t;
#endif /*__HAL_MEDIA_H__*/
