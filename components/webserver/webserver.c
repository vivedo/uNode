/* Copyright (c) 2020, Edoardo Viviani <edoardo.viviani@gmail.com>
 * All rights reserved.
 *
 * This software is distributed under the Simplified BSD License
 */

#include "webserver.h"
#include "webcontent.h"

#include <esp_log.h>
#include <sys/param.h>
#include <http_parser.h>
#include <esp_system.h>
#include <esp_http_server.h>

static const char *TAG = "web";

static esp_err_t web_index_get_handler(httpd_req_t *req);
static esp_err_t web_wifi_get_handler(httpd_req_t *req);
static esp_err_t web_dmx_get_handler(httpd_req_t *req);
static esp_err_t web_wifi_post_handler(httpd_req_t *req);
static esp_err_t web_dmx_post_handler(httpd_req_t *req);

static httpd_uri_t web_index_get = { .uri = "/",     .method = HTTP_GET,  .handler = web_index_get_handler, .user_ctx = NULL };
static httpd_uri_t web_wifi_get  = { .uri = "/wifi", .method = HTTP_GET,  .handler = web_wifi_get_handler,  .user_ctx = NULL };
static httpd_uri_t web_dmx_get   = { .uri = "/dmx",  .method = HTTP_GET,  .handler = web_dmx_get_handler,   .user_ctx = NULL };
static httpd_uri_t web_wifi_post = { .uri = "/wifi", .method = HTTP_POST, .handler = web_wifi_post_handler, .user_ctx = NULL };
static httpd_uri_t web_dmx_post  = { .uri = "/dmx",  .method = HTTP_POST, .handler = web_dmx_post_handler,  .user_ctx = NULL };

static httpd_handle_t server = NULL;

void start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    ESP_LOGD(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGD(TAG, "Registering URI handlers");

        httpd_register_uri_handler(server, &web_index_get);
        httpd_register_uri_handler(server, &web_wifi_get);
        httpd_register_uri_handler(server, &web_dmx_get);
        httpd_register_uri_handler(server, &web_wifi_post);
        httpd_register_uri_handler(server, &web_dmx_post);
    }

    ESP_LOGD(TAG, "Error starting server!");
}

void stop_webserver(void) {
    httpd_stop(server);
}

/* GET / handler */
static esp_err_t web_index_get_handler(httpd_req_t *req) {
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    char macStr[18];
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    httpd_resp_send_chunk(req, web_common_header, strlen(web_common_header));
    httpd_resp_send_chunk(req, macStr, 18);
    httpd_resp_send_chunk(req, web_index, strlen(web_index));
    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

/* GET /wifi handler */
static esp_err_t web_wifi_get_handler(httpd_req_t *req) {
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    char macStr[18];
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    httpd_resp_send_chunk(req, web_common_header, strlen(web_common_header));
    httpd_resp_send_chunk(req, macStr, 18);
    httpd_resp_send_chunk(req, web_common_header2, strlen(web_common_header2));

    char buf[strlen(web_wifi_settings) + 32];
    sprintf(buf, web_wifi_settings, "TO-BE-READ-FROM-FLASH");
    httpd_resp_send_chunk(req, buf, strlen(buf));

    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
};

/* GET /dmx handler */
static esp_err_t web_dmx_get_handler(httpd_req_t *req) {
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    char macStr[18];
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    httpd_resp_send_chunk(req, web_common_header, strlen(web_common_header));
    httpd_resp_send_chunk(req, macStr, 18);
    httpd_resp_send_chunk(req, web_common_header2, strlen(web_common_header2));

    char buf[strlen(web_dmx_settings) + 26];
    sprintf(buf, web_dmx_settings, "checked", "disabled", 1000, 2000);
    httpd_resp_send_chunk(req, buf, strlen(buf));

    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
};

/* POST /wifi handler */
static esp_err_t web_wifi_post_handler(httpd_req_t *req) {

    httpd_resp_set_hdr(req, "Location", "/wifi");
    httpd_resp_set_status(req, "301 Moved Permanently");
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
};

/* POST /dmx handler */
static esp_err_t web_dmx_post_handler(httpd_req_t *req) {
    uint8_t body_len = MAX(req->content_len, 64); // in POST /dmx content_len should be max 22 characters
    char body[body_len];

    httpd_req_recv(req, body, body_len);

    uint16_t uniA = 0, uniB = 0;
    bool artnet = false;

    // This is a really bad body parser, but there's no need to add junk for something this simple
    char *param = strtok(body, "&");
    while(param != NULL) {
        param[2] = '\0';

        if(strcmp(param, "au") == 0)
            uniA = atoi(&param[3]);
        else if(strcmp(param, "bu") == 0)
            uniB = atoi(&param[3]);
        else if(strcmp(param, "pr") == 0)
            artnet = param[3] == 'a';

        param = strtok(NULL, "&");
    }

    printf("UNIA: %d | UNIB: %d, | MODE: %s", uniA, uniB, artnet ? "ArtNet" : "sACN");

    httpd_resp_set_hdr(req, "Location", "/dmx");
    httpd_resp_set_status(req, "301 Moved Permanently");
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
};

// /* An HTTP POST handler */
//static esp_err_t echo_post_handler(httpd_req_t *req) {
//    char buf[100];
//    int ret, remaining = req->content_len;
//
//    while (remaining > 0) {
//        /* Read the data for the request */
//        if ((ret = httpd_req_recv(req, buf,
//                                  MIN(remaining, sizeof(buf)))) <= 0) {
//            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
//                /* Retry receiving if timeout occurred */
//                continue;
//            }
//            return ESP_FAIL;
//        }
//
//        /* Send back the same data */
//        httpd_resp_send_chunk(req, buf, ret);
//        remaining -= ret;
//
//        /* Log data received */
//        ESP_LOGD(TAG, "=========== RECEIVED DATA ==========");
//        ESP_LOGD(TAG, "%.*s", ret, buf);
//        ESP_LOGD(TAG, "====================================");
//    }
//
//    // End response
//    httpd_resp_send_chunk(req, NULL, 0);
//    return ESP_OK;
//}