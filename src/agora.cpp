/*
 * Copyright 2023 Ethan. All rights reserved.
 */
#include "agora.h"
#include "agora_rtc_api.h"
#include <string>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <cjson/cJSON.h>
#include <curl/curl.h>

#define MAX_CHN_NUM 3

typedef struct agora {
	uint32_t	conn_id[3];
	agora_connnected_cb_t ccb;
	agora_msg_cb_t	mcb;
	bool		user_connected[4];
} agora_t;

static agora_t g_agora;

static size_t write_memory_cb(void *ptr, size_t size, size_t nmemb, void *context)
{
	size_t bytec = size*nmemb;
	std::string *result = (std::string*)context;

	result->append((char*)ptr, bytec);
	return nmemb;
}

static void __on_rtm_data(const char *user_id, const void *data, size_t data_len)
{
	agora_t *ago = &g_agora;
	if (ago->mcb)
		ago->mcb((const char *)data, data_len);
}

static void __on_rtm_event(const char *user_id, rtm_event_type_e event_id, rtm_err_code_e event_code)
{
	printf("<<<<<<<<<<<<<<<<<< user_id[%s] event id[%u], event code[%u] >>>>>>>>>>>>>>>>>>\n", user_id, event_id, event_code);
}

static void __on_rtm_send_data_res(const char *rtm_uid, uint32_t msg_id, rtm_msg_state_e state)
{
}

static agora_rtm_handler_t rtm_handler = {
	.on_rtm_data = __on_rtm_data,
	.on_rtm_event = __on_rtm_event,
	.on_send_rtm_data_result = __on_rtm_send_data_res,
};

static int basic_auth_post(std::string url, std::string username, std::string password, std::string body, std::string &result)
{
	CURL *curl;
	CURLcode res;
	struct curl_slist *headers = NULL;

	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5000);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);
		curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_cb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&result);

		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		res = curl_easy_perform(curl);
		if(res != CURLE_OK) {
			printf("curl_easy_perform() failed: %s", curl_easy_strerror(res));
		}
		else {
			//std::cout << result << std::endl;
		}

		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
		curl_global_cleanup();
	}

	return res;
}

static void __on_join_channel_success(connection_id_t conn_id, uint32_t uid, int elapsed)
{
	printf("uid %d join successfully by conn id %d\n", uid, conn_id);
	
	agora_t *ago = &g_agora;
	if (ago->ccb)
		ago->ccb((int)uid);
}

static void __on_reconnecting(connection_id_t conn_id)
{
	printf("func: %s, conn_id= %d\n", __func__,  conn_id);
}

static void __on_connection_lost(connection_id_t conn_id)
{
	printf("func: %s, conn_id= %d\n", __func__,  conn_id);
}

static void __on_rejoin_channel_success(connection_id_t conn_id, uint32_t uid, int elapsed_ms)
{

}

static void __on_user_joined(connection_id_t conn_id, uint32_t uid, int elapsed_ms)
{
	printf("func:%s, user joind... conn_id=%d\n", __func__, conn_id);
	agora_t *ago = &g_agora;
	ago->user_connected[conn_id] = true;
}

static void __on_user_offline(connection_id_t conn_id, uint32_t uid, int reason)
{
	printf("func:%s, user offline ...\n", __func__);
	agora_t *ago = &g_agora;
	ago->user_connected[conn_id] = false;
}

static void __on_user_mute_audio(connection_id_t conn_id, uint32_t uid, bool muted)
{
}

static void __on_user_mute_video(connection_id_t conn_id, uint32_t uid, bool muted)
{

}

static void __on_error(connection_id_t conn_id, int code, const char *msg)
{
	printf("error: [msg:%s]\n", msg);
}

static void __on_license_failed(connection_id_t conn_id, int reason)
{
}

static void __on_audio_data(connection_id_t conn_id, const uint32_t uid, uint16_t sent_ts,
		const void *data, size_t len, const audio_frame_info_t *info_ptr)
{

}

static void __on_mixed_audio_data(connection_id_t conn_id, const void *data, size_t len,
		const audio_frame_info_t *info_ptr)
{
}

static void __on_video_data(connection_id_t conn_id, const uint32_t uid, uint16_t sent_ts,
		const void *data, size_t len, const video_frame_info_t *info_ptr)
{
}

static void __on_target_bitrate_changed(connection_id_t conn_id, uint32_t target_bps)
{
	printf("finc:%s, target_bps=%d.\n", __func__, target_bps);
}

static void __on_key_frame_gen_req(connection_id_t conn_id, uint32_t uid,
		video_stream_type_e stream_type)
{
}

int agora_init(std::string room, agora_connnected_cb_t ccb, agora_msg_cb_t mcb)
{
	memset(&g_agora, 0, sizeof(g_agora));
	agora_t *ago = &g_agora;
	ago->ccb = ccb;
	ago->mcb = mcb;

	rtc_service_option_t service_opt = { 0 };
	service_opt.area_code = AREA_CODE_GLOB;
	agora_rtc_event_handler_t event_handler;
	memset(&event_handler, 0, sizeof(event_handler));

	event_handler.on_join_channel_success = __on_join_channel_success;
	event_handler.on_reconnecting = __on_reconnecting,
		event_handler.on_connection_lost = __on_connection_lost;
	event_handler.on_rejoin_channel_success = __on_rejoin_channel_success;
	event_handler.on_user_joined = __on_user_joined;
	event_handler.on_user_offline = __on_user_offline;
	event_handler.on_user_mute_audio = __on_user_mute_audio;
	event_handler.on_user_mute_video = __on_user_mute_video;
	event_handler.on_target_bitrate_changed = __on_target_bitrate_changed;
	event_handler.on_key_frame_gen_req = __on_key_frame_gen_req;
	event_handler.on_video_data = __on_video_data;
	event_handler.on_error = __on_error;
	event_handler.on_license_validation_failure = __on_license_failed;
	event_handler.on_mixed_audio_data = __on_mixed_audio_data;
	event_handler.on_audio_data = __on_audio_data;

	int rval = agora_rtc_init("3b64a6f5683d4abe9a7f3f72b7e7e9c8", &event_handler, &service_opt);
	if (rval < 0) {
		printf("agora sdk init failed, rval=%d error=%s\n", rval, agora_rtc_err_2_str(rval));
		return -1;
	}
	
	std::string channel_name = room;
	std::string rtm_uid = room;
	if  (rtm_uid != "gello") 
		rtm_uid += "-arm";
	std::string url = "https://is2ef74oirsuxgzg6e4b6w64xy0zykul.lambda-url.ap-southeast-1.on.aws/api/agora/token";
	//std::string body = "{\"rtc_uid\": 0,\"rtm_uid\": \"mycobot\", \"channel\": \""; //gello\"}";
	std::string body = "{\"rtc_uid\": 0,\"rtm_uid\": \"";
	body += rtm_uid;
	body +=	 "\", \"channel\": \""; //gello\"}";
	body += channel_name;
	body += "\"}";
	std::cout << "body: " << body.c_str() << std::endl;
	printf("body: %s\n", body.c_str());
	std::string token;
	std::string rtm_token;
	std::string rtc_token;
	cJSON *json_data;

	while (1) {
		basic_auth_post(url, "root", "frodobot@2023", body, token);

		cJSON *json = cJSON_Parse(token.c_str());
		if (json == NULL) {
			usleep(1000*1000);
			continue;
		}

		json_data = cJSON_GetObjectItem(json, "rtc_token");
		if(!cJSON_IsString(json_data) || json_data->valuestring == NULL) {
			cJSON_Delete(json);
			break;
		}

		rtc_token = std::string(json_data->valuestring);

		json_data = cJSON_GetObjectItem(json, "rtm_token");
		if(!cJSON_IsString(json_data) || json_data->valuestring == NULL) {
			printf("Cannot get RTM token\n");
			usleep(1000 * 1000);
			cJSON_Delete(json);
			continue;
		}

		rtm_token = std::string(json_data->valuestring);
		cJSON_Delete(json);
		break;
	}

	for (int i = 0; i < MAX_CHN_NUM; i++) {
		rval = agora_rtc_create_connection(&ago->conn_id[i]);
		if (rval < 0) {
			printf("Failed to create connection, reason: %s\b", agora_rtc_err_2_str(rval));
			return -1;
		}

		rtc_channel_options_t channel_options = { 0 };
		memset(&channel_options, 0, sizeof(channel_options));

		//rval = agora_rtc_join_channel(g_camera_services[i].conn_id, "gello", 1000 + i, rtc_token.c_str(), &channel_options);
		rval = agora_rtc_join_channel(ago->conn_id[i], channel_name.c_str(), 1000 + i, rtc_token.c_str(), &channel_options);
		if (rval < 0) {
			printf("Failed to join channel, reason: %s\n", agora_rtc_err_2_str(rval));
			return -1;
		}
	}

	// 3. API:
	printf("rtm_uid: %s.\n", rtm_uid.c_str());	
	rval = agora_rtc_login_rtm(rtm_uid.c_str(), rtm_token.c_str(), &rtm_handler);
	if (rval < 0) {
		printf("login rtm failed\n");
		return -1;
	}

	return 0;
}

void agora_final()
{
	agora_rtc_logout_rtm();
	agora_rtc_fini();
}

int agora_frame_send(int conn_id, const hal_frame_t *frame)
{
	agora_t *ago = &g_agora;

	if (false == ago->user_connected[conn_id])
		return -1;

	video_frame_info_t video_frame_info;
	memset(&video_frame_info, 0, sizeof(video_frame_info));
	video_frame_info.frame_rate = (video_frame_rate_e)30;
	video_frame_info.data_type = VIDEO_DATA_TYPE_H264;
	video_frame_info.stream_type = VIDEO_STREAM_HIGH;
	video_frame_info.frame_type = (frame->m_frame_type == HFT_I)? VIDEO_FRAME_KEY : VIDEO_FRAME_DELTA;

	int rval = agora_rtc_send_video_data(conn_id, frame->m_data, frame->m_len, &video_frame_info);
	if(rval < 0) {
		printf("send failed: %s", agora_rtc_err_2_str(rval));
		return -1;
	}

	return 0;
}
