cmake_minimum_required(VERSION 3.20.0)
set(SHIELD ssd1306_128x32)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(TX-RX)

file(GLOB_RECURSE APP_SOURCES "src/*.c")

list(FILTER APP_SOURCES EXCLUDE REGEX ".*/others/.*")
list(FILTER APP_SOURCES EXCLUDE REGEX "/drivers/display_handler.*")
list(FILTER APP_SOURCES EXCLUDE REGEX "/fsm/fsm.c")

target_sources(app PRIVATE ${APP_SOURCES})

