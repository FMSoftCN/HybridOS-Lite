cmake_minimum_required(VERSION 3.10)
project(common_library)
aux_source_directory(. DIR_SRCS)
add_library(common STATIC ${DIR_SRCS})
#target_link_libraries(svgshow gobject-2.0 hicairo)

include_directories(${MINIGUI_INCLUDE_DIRS} ${MGEFF_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${HISVG_INCLUDE_DIR} ${HICairo_INCLUDE_DIRS})
