cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "datastore.c"
        INCLUDE_DIRS "."
        PRIV_REQUIRES nvs_flash common)