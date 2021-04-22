# - Try to find Librsvg 2.0



find_package(PkgConfig QUIET)
pkg_check_modules(PC_LIBHIRSVG libhirsvg-2.0)

find_path(LIBHIRSVG_INCLUDE_DIR
    NAMES libhirsvg/rsvg.h
    HINTS ${PC_LIBHIRSVG_INCLUDEDIR} ${PC_LIBHIRSVG_INCLUDE_DIRS}
)

find_library(LIBHIRSVG_LIBRARIES
    NAMES hirsvg-2
    HINTS ${PC_LIBHIRSVG_LIBDIR}
          ${PC_LIBHIRSVG_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Libhirsvg REQUIRED_VARS LIBHIRSVG_LIBRARIES
                                  FOUND_VAR LIBHIRSVG_FOUND)

mark_as_advanced(LIBHIRSVG_INCLUDE_DIR LIBHIRSVG_LIBRARIES)
