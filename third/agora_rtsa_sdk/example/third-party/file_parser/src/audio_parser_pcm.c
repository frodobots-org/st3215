/*************************************************************
 * Author:	wangjiangyuan (wangjiangyuan@agora.io)
 * Date	 :	Oct 21th, 2020
 * Module:	Agora SD-RTN SDK RTC C API demo application.
 *
 *
 * This is a part of the Agora RTC Service SDK.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#include <stdio.h>
#include <string.h>

#include "file_parser.h"
#include "file_parser_priv.h"

typedef struct {
  FILE *file;
  size_t file_size;
  size_t pcm_frame_len;
} ctx_t;

extern long get_file_size(FILE *f);

static int pcm_open(media_parser_t *h, const char *path)
{
  FILE *file = NULL;

  file = fopen(path, "r");

  if (!file) {
    AGO_LOGE("open %s failed", path);
    return -1;
  }

  ctx_t *p_ctx = (ctx_t *)malloc(sizeof(ctx_t));
  if (!p_ctx) {
    return -1;
  }

  p_ctx->file = file;
  p_ctx->file_size = get_file_size(file);
  p_ctx->pcm_frame_len = (h->parser_cfg.u.audio_cfg.sampleRateHz * h->parser_cfg.u.audio_cfg.framePeriodMs / 1000 *
                          h->parser_cfg.u.audio_cfg.numberOfChannels * sizeof(int16_t));
  h->p_ctx = (void *)p_ctx;

  return 0;
}

static int pcm_obtain_frame(media_parser_t *h, frame_t *p_frame)
{
  ctx_t *p_ctx = (ctx_t *)h->p_ctx;
  if (!p_ctx) {
    AGO_LOGE("parser: invalid ctx");
    return -1;
  }

  if (feof(p_ctx->file)) {
    return -2;
  }

  p_frame->ptr = malloc(p_ctx->pcm_frame_len);
  int ret = fread(p_frame->ptr, 1, p_ctx->pcm_frame_len, p_ctx->file);
  if (ret != p_ctx->pcm_frame_len) {
    if (ret <= 0) {
      memset(p_frame->ptr, 0, p_ctx->pcm_frame_len);
    } else {
      memset(p_frame->ptr + ret, 0, p_ctx->pcm_frame_len - ret);
    }
  }

  p_frame->type = h->type;
  p_frame->len = p_ctx->pcm_frame_len;

  return 0;
}

static int pcm_release_frame(media_parser_t *h, frame_t *p_frame)
{
  free(p_frame->ptr);
  return 0;
}

static int pcm_reset(media_parser_t *h)
{
  ctx_t *p_ctx = (ctx_t *)h->p_ctx;

  fseek(p_ctx->file, 0L, SEEK_SET);

  return 0;
}

static int pcm_close(media_parser_t *h)
{
  ctx_t *p_ctx = (ctx_t *)h->p_ctx;

  fclose(p_ctx->file);

  free(p_ctx);
  h->p_ctx = NULL;

  return 0;
}

media_parser_t audio_parser_pcm = {
  .codec = MEDIA_FILE_TYPE_PCM,
  .name = "pcm",
  .p_ctx = NULL,

  .open = pcm_open,
  .obtain_frame = pcm_obtain_frame,
  .release_frame = pcm_release_frame,
  .reset = pcm_reset,
  .close = pcm_close,
};
