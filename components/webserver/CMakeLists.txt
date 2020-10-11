cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "webserver.c"
        INCLUDE_DIRS "."
        PRIV_REQUIRES esp_http_server http_parser)