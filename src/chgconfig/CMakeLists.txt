cmake_minimum_required(VERSION 3.10)
project(chgconfig)
aux_source_directory(. DIR_SRCS)
add_executable(chgconfig ${DIR_SRCS})

if(PLATFORM_PX30)
    include_directories(${GLIB_INCLUDE_DIRS} ${HISVG_INCLUDE_DIR} ${MINIGUI_INCLUDE_DIRS} ${MGEFF_INCLUDE_DIRS})
else()
   include_directories(${GLIB_INCLUDE_DIRS} ${HISVG_INCLUDE_DIR} ${HICairo_INCLUDE_DIRS} ${MINIGUI_INCLUDE_DIRS} ${MGEFF_INCLUDE_DIRS})
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-g ${CMAKE_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "-g ${CMAKE_C_FLAGS}")
endif(CMAKE_COMPILER_IS_GNUCXX)

target_link_libraries(chgconfig ${MINIGUI_LIBRARIES} ${MGEFF_LIBRARIES} ${HISVG_LIBRARIES} ${GLIB_LIBRARIES} hicairo hibus hibox hidomlayout common) 
install(TARGETS chgconfig DESTINATION ${CMAKE_INSTALL_PREFIX}/libexec/)
