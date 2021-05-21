cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include" "/${CMAKE_SYSROOT}usr/local/include/cairo")
LINK_DIRECTORIES("${CMAKE_SYSROOT}/usr/lib" "${CMAKE_SYSROOT}/usr/local/lib")

OPTION(TRANSPARENT_BK "Whether background of application is transparent" OFF)
if(TRANSPARENT_BK)
  MESSAGE(STATUS "The background of applications is transparent.")
  ADD_DEFINITIONS(-DTRANSPARENT_BK)
endif()

find_package(MiniGUI 5.0.2 REQUIRED)
find_package(mGEff 1.5.0 REQUIRED)
find_package(GLIB 2.0 REQUIRED)
find_package(hicairo REQUIRED)
#find_package(Libhirsvg 2.0 REQUIRED)
find_package(HiSVG REQUIRED)

set(MINIGUI_LIBRARIES ${MiniGUI_LIBRARIES})
set(MINIGUI_INCLUDE_DIRS ${MiniGUI_INCLUDE_DIRS})

set(MGEFF_LIBRARIES ${mGEff_LIBRARIES})
set(MGEFF_INCLUDE_DIRS ${mGEff_INCLUDE_DIRS})

# copy the necessary resource files to /bin/res
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/res)
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/config)
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/layout)

list(APPEND HISHELL_RES_FILES
    "${PROJECT_SOURCE_DIR}/src/mginit/res/circle.svg"
    "${PROJECT_SOURCE_DIR}/src/chgconfig/res/single.svg"
    "${PROJECT_SOURCE_DIR}/src/chgconfig/res/multiple.svg"
    "${PROJECT_SOURCE_DIR}/src/svgshow/res/gear.svg"
    "${PROJECT_SOURCE_DIR}/src/svgshow/res/cool.svg"
    "${PROJECT_SOURCE_DIR}/src/svgshow/res/heat.svg"
    "${PROJECT_SOURCE_DIR}/src/svgshow/res/dry.svg"
    "${PROJECT_SOURCE_DIR}/src/svgshow/res/switch.svg"
    "${PROJECT_SOURCE_DIR}/src/svgshow/res/windcircle.svg"
    "${PROJECT_SOURCE_DIR}/src/svgshow/res/windnature.svg"
    "${PROJECT_SOURCE_DIR}/src/svgshow/res/windlittle.svg"
    "${PROJECT_SOURCE_DIR}/src/svgshow/res/light.svg"
    "${PROJECT_SOURCE_DIR}/src/svgshow/res/health.svg"
    "${PROJECT_SOURCE_DIR}/src/svgshow/res/speedhigh.svg"
    "${PROJECT_SOURCE_DIR}/src/svgshow/res/speedmid.svg"
    "${PROJECT_SOURCE_DIR}/src/svgshow/res/speedlow.svg"
    "${PROJECT_SOURCE_DIR}/src/svgshow/res/left.svg"
    "${PROJECT_SOURCE_DIR}/src/svgshow/res/up.svg"
)

file(COPY ${HISHELL_RES_FILES} DESTINATION ${PROJECT_SOURCE_DIR}/bin/res)
file(COPY "${PROJECT_SOURCE_DIR}/src/layout/MiniGUI.cfg" DESTINATION ${PROJECT_SOURCE_DIR}/bin)
file(COPY "${PROJECT_SOURCE_DIR}/src/layout/manifest.json" DESTINATION ${PROJECT_SOURCE_DIR}/bin/layout)
file(COPY "${PROJECT_SOURCE_DIR}/src/layout/default_layout.css" DESTINATION ${PROJECT_SOURCE_DIR}/bin/layout)
file(COPY "${PROJECT_SOURCE_DIR}/src/layout/newconfig.json" DESTINATION ${PROJECT_SOURCE_DIR}/bin/layout)
file(COPY "${PROJECT_SOURCE_DIR}/src/layout/chgconfig1.css" DESTINATION ${PROJECT_SOURCE_DIR}/bin/layout)
file(COPY "${PROJECT_SOURCE_DIR}/src/layout/chgconfig2.css" DESTINATION ${PROJECT_SOURCE_DIR}/bin/layout)
file(COPY "${PROJECT_SOURCE_DIR}/src/layout/newconfig.json" DESTINATION ${PROJECT_SOURCE_DIR}/bin/layout)
file(COPY "${PROJECT_SOURCE_DIR}/src/layout/svgshow1.css" DESTINATION ${PROJECT_SOURCE_DIR}/bin/layout)
file(COPY "${PROJECT_SOURCE_DIR}/src/layout/svgshow2.css" DESTINATION ${PROJECT_SOURCE_DIR}/bin/layout)
file(COPY "${PROJECT_SOURCE_DIR}/src/layout/wheel1.css" DESTINATION ${PROJECT_SOURCE_DIR}/bin/layout)
file(COPY "${PROJECT_SOURCE_DIR}/src/config" DESTINATION ${PROJECT_SOURCE_DIR}/bin/)

# set output path
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# set sub-directory
add_subdirectory(src)
