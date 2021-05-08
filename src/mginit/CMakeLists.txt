cmake_minimum_required(VERSION 3.10)
project(mginit)
aux_source_directory(. DIR_SRCS)
add_executable(mginit ${DIR_SRCS})
if(PLATFORM_PX30)
include_directories(${MINIGUI_INCLUDE_DIRS} ${MGEFF_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${HISVG_INCLUDE_DIRS})
else()
include_directories(${MINIGUI_INCLUDE_DIRS} ${MGEFF_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${HISVG_INCLUDE_DIRS} ${HICairo_INCLUDE_DIRS})
endif()

target_link_libraries(mginit gobject-2.0 hicairo)
target_link_libraries(mginit ${MINIGUI_LIBRARIES} ${MGEFF_LIBRARIES} ${HISVG_LIBRARIES} ${GLIB_LIBRARIES} hibus hibox hidomlayout)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-g ${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "-g ${CMAKE_C_FLAGS}")
endif(CMAKE_COMPILER_IS_GNUCXX)

install(TARGETS mginit DESTINATION ${CMAKE_INSTALL_PREFIX}/libexec/)
