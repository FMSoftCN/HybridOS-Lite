cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include" "/usr/local/include/librsvg-2.0" "/usr/local/include/cairo")
LINK_DIRECTORIES("/usr/lib" "/usr/local/lib")

find_package(MiniGUI 5.0.2 REQUIRED)
find_package(mGEff 1.5.0 REQUIRED)
find_package(GLIB 2.0 REQUIRED)
find_package(hicairo REQUIRED)
find_package(Libhirsvg 2.0 REQUIRED)

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
    "${PROJECT_SOURCE_DIR}/mginit/res/circle.svg"
    "${PROJECT_SOURCE_DIR}/chgconfig/res/single.svg"
    "${PROJECT_SOURCE_DIR}/chgconfig/res/multiple.svg"
    "${PROJECT_SOURCE_DIR}/gear/res/gear.svg"
)

file(COPY ${HISHELL_RES_FILES} DESTINATION ${PROJECT_SOURCE_DIR}/bin/res)
file(COPY "${PROJECT_SOURCE_DIR}/layout/MiniGUI.cfg" DESTINATION ${PROJECT_SOURCE_DIR}/bin)
file(COPY "${PROJECT_SOURCE_DIR}/layout/hybridos.cfg" DESTINATION ${PROJECT_SOURCE_DIR}/bin)
file(COPY "${PROJECT_SOURCE_DIR}/layout/manifest.json" DESTINATION ${PROJECT_SOURCE_DIR}/bin/layout)
file(COPY "${PROJECT_SOURCE_DIR}/layout/default_layout.css" DESTINATION ${PROJECT_SOURCE_DIR}/bin/layout)
file(COPY "${PROJECT_SOURCE_DIR}/layout/newconfig.json" DESTINATION ${PROJECT_SOURCE_DIR}/bin/layout)
file(COPY "${PROJECT_SOURCE_DIR}/layout/chgconfig1.css" DESTINATION ${PROJECT_SOURCE_DIR}/bin/layout)
file(COPY "${PROJECT_SOURCE_DIR}/layout/chgconfig2.css" DESTINATION ${PROJECT_SOURCE_DIR}/bin/layout)
file(COPY "${PROJECT_SOURCE_DIR}/layout/newconfig.json" DESTINATION ${PROJECT_SOURCE_DIR}/bin/layout)
file(COPY "${PROJECT_SOURCE_DIR}/layout/gear1.css" DESTINATION ${PROJECT_SOURCE_DIR}/bin/layout)
file(COPY "${PROJECT_SOURCE_DIR}/layout/gear2.css" DESTINATION ${PROJECT_SOURCE_DIR}/bin/layout)
file(COPY "${PROJECT_SOURCE_DIR}/config" DESTINATION ${PROJECT_SOURCE_DIR}/bin/)

# set output path
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# set sub-directory
add_subdirectory(mginit)
add_subdirectory(wallpaper)
add_subdirectory(gear)
add_subdirectory(chgconfig)
