cmake_minimum_required(VERSION 3.5)

idf_component_register(SRCS "fake_dns.c" INCLUDE_DIRS "." PRIV_REQUIRES lwip tcpip_adapter common)