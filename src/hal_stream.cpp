#include "hal_stream.h"
#include "gst/gst.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define CAMERA_NUM	1

#define CAMERA1_PIPELINE_SOURCE "v4l2src device=/dev/v4l/by-id/usb-HRY_USB_Camera_20191204-video-index0 ! image/jpeg, width=(int)640, height=(int)480, framerate=(fraction)30/1 ! videorate max-rate=30 ! avdec_mjpeg ! videoconvert ! v4l2h264enc min-force-key-unit-interval=1000000000 capture-io-mode=4 output-io-mode=4 extra-controls=encode,video_bitrate=1200000,video_bitrate_mode=0 name=encoder3 ! video/x-h264, stream-format=(string)byte-stream, level=(string)4, alighnment=(string)au ! h264parse config-interval=-1 ! appsink name=app-sink"

#define CAMERA2_PIPELINE_SOURCE "v4l2src device=/dev/v4l/by-id/usb-Integrated_Webcam_Integrated_Webcam-video-index0 ! image/jpeg, width=(int)640, height=(int)480, framerate=(fraction)30/1 ! videorate max-rate=30 ! avdec_mjpeg ! videoconvert ! v4l2h264enc min-force-key-unit-interval=1000000000 capture-io-mode=4 output-io-mode=4 extra-controls=encode,video_bitrate=1200000,video_bitrate_mode=0 name=encoder3 ! video/x-h264, stream-format=(string)byte-stream, level=(string)4, alighnment=(string)au ! h264parse config-interval=-1 ! appsink name=app-sink"

#define CAMERA3_PIPELINE_SOURCE "v4l2src device=/dev/v4l/by-id/usb-RYS_USB_Camera_200901010001-video-index0 ! image/jpeg, width=(int)640, height=(int)480, framerate=(fraction)30/1 ! videorate max-rate=30 ! avdec_mjpeg ! videoconvert ! v4l2h264enc min-force-key-unit-interval=1000000000 capture-io-mode=4 output-io-mode=4 extra-controls=encode,video_bitrate=1200000,video_bitrate_mode=0 name=encoder2 ! video/x-h264, stream-format=(string)byte-stream, level=(string)4, alighnment=(string)au ! h264parse config-interval=-1 ! appsink name=app-sink"

const char *g_camera_pipeline[] = {CAMERA1_PIPELINE_SOURCE, CAMERA2_PIPELINE_SOURCE, CAMERA3_PIPELINE_SOURCE};

typedef struct CameraService {
  uint32_t	ch;
  GstElement	*pipeline;
  GstElement	*app_sink;
} CameraService;

typedef struct media_device {
	CameraService camera_services[CAMERA_NUM];
	bool inited;
	hal_frame_cb_t cb;
} media_device_t;

static media_device_t g_media_deivce;

static GstFlowReturn on_front_cam_data(GstElement *sink, void *data)
{
	media_device_t *md = &g_media_deivce;
	CameraService *camera_service = (CameraService*)data;
	uint32_t ch = camera_service->ch;
	GstSample *sample;
	GstBuffer *buffer;
	GstMapInfo info;

	uint8_t *buf;
	g_signal_emit_by_name(sink, "pull-sample", &sample);

	if(sample) {
		buffer = gst_sample_get_buffer(sample);
		gst_buffer_map(buffer, &info, GST_MAP_READ);
		buf = info.data + 6;
		printf("ch [%d].\n", ch);
		
		hal_frame_t frame;
		memset(&frame, 0, sizeof(hal_frame_t));
		frame.m_frame_type = (buf[4] & 0x05) == 0x05 ? HFT_I : HFT_B;
		frame.m_data = buf;
		frame.m_len = info.size - 6;
		frame.m_enc_type = HET_H264;

		if (md->cb)
			md->cb(ch, &frame, NULL);

		gst_buffer_unmap(buffer, &info);
		gst_sample_unref(sample);
		return GST_FLOW_OK;
	}

	return GST_FLOW_ERROR;
}


int media_device_init(hal_frame_cb_t cb)
{
	media_device_t *md = &g_media_deivce;
	memset(md, 0, sizeof(media_device_t));
	md->inited = true;
	md->cb = cb;

	gst_init(NULL, NULL);

	for (int i = 0; i < CAMERA_NUM; i++) {
		md->camera_services[i].ch = i;
		md->camera_services[i].pipeline = gst_parse_launch(g_camera_pipeline[i], NULL);
		md->camera_services[i].app_sink = gst_bin_get_by_name(GST_BIN(md->camera_services[i].pipeline), "app-sink");
		g_signal_connect(md->camera_services[i].app_sink, "new-sample", G_CALLBACK(on_front_cam_data), &md->camera_services[i]);
		g_object_set(md->camera_services[i].app_sink, "emit-signals", TRUE, NULL);
	}
	
	printf("media_device_init success.\n");
	return 0;
}

void meida_device_final()
{
	media_device_t *md = &g_media_deivce;
	memset(md, 0, sizeof(media_device_t));
	md->inited = false;
}

int media_device_start(int ch, const void *ctx)
{
	if (ch >= CAMERA_NUM) {
		printf("media_device_start failed.\n");
		return -1;
	}

	media_device_t *md = &g_media_deivce;
	gst_element_set_state(md->camera_services[ch].pipeline, GST_STATE_PLAYING);

	return 0;
}

int media_device_stop(int ch)
{
	media_device_t *md = &g_media_deivce;

	return 0;
}

int media_device_stop_all()
{
	media_device_t *md = &g_media_deivce;

	return 0;
}
