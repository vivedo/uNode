cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "dmx.c"
        INCLUDE_DIRS "."
        PRIV_REQUIRES common)