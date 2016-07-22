# User must set environment variable TIFF_DIR to libtiff directory
# This will define:
# TIFF_INCLUDE_DIR
# TIFF_LIBRARY_DIR
# TIFF_DLL_RELEASE - runtime library for Windows RELEASE config
# TIFF_DLL_DEBUG - runtime library for Windows DEBUG config
# TIFF_LIB_RELEASE - library to link against in Windows RELEASE config
# TIFF_LIB_DEBUG - library to link against in Windows DEBUG config
# TIFF_LIBRARY - library to link against for OS X

if(WIN32)
	find_path(TIFF_INCLUDE_DIR
		NAMES
			tiffio.h
		PATHS
			$ENV{TIFF_DIR}/libtiff
		DOC "The directory where tiffio.h resides"
	)

	if (ARCH STREQUAL "x86")
		find_path(TIFF_LIBRARY_DIR
			NAMES
				libtiff.lib
			PATHS
				$ENV{TIFF_DIR}/libtiff/
			DOC "The directory where TIFF libraries are stored"
		)
	else(ARCH STREQUAL "x86")
		# 64-bit
		find_path(TIFF_LIBRARY_DIR
			NAMES
				libtiff.lib
			PATHS
				$ENV{TIFF_DIR}/libtiff/
			DOC "The directory where TIFF libraries are stored"
		)
	endif(ARCH STREQUAL "x86")

	if(TIFF_INCLUDE_DIR)
		set(TIFF_LIB_RELEASE "${TIFF_LIBRARY_DIR}/libtiff.lib")
		set(TIFF_LIB_DEBUG "${TIFF_LIBRARY_DIR}/libtiff.lib")
		set(TIFF_DLL_RELEASE "${TIFF_LIBRARY_DIR}/libtiff.dll")
		set(TIFF_DLL_DEBUG "${TIFF_LIBRARY_DIR}/libtiff.dll")
	endif(TIFF_INCLUDE_DIR)

endif(WIN32)

if(TIFF_INCLUDE_DIR)
	set(TIFF_FOUND 1 CACHE BOOL "LibTIFF found")
else(TIFF_INCLUDE_DIR)
	set(TIFF_FOUND 0 CACHE BOOL "LibTIFF found")
endif(TIFF_INCLUDE_DIR)

mark_as_advanced(TIFF_FOUND)