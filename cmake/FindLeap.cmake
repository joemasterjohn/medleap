# User must set environment variable LEAP_DIR to LeapSDK directory
# This will define:
# LEAP_INCLUDE_DIR
# LEAP_LIBRARY_DIR
# LEAP_DLL_RELEASE - runtime library for Windows RELEASE config
# LEAP_DLL_DEBUG - runtime library for Windows DEBUG config
# LEAP_LIB_RELEASE - library to link against in Windows RELEASE config
# LEAP_LIB_DEBUG - library to link against in Windows DEBUG config
# LEAP_LIBRARY - library to link against for OS X

if(WIN32)
	find_path(LEAP_INCLUDE_DIR
		NAMES
			Leap.h
		PATHS
			$ENV{LEAP_DIR}/include
		DOC "The directory where Leap.h resides"
	)

	if (ARCH STREQUAL "x86")
		find_path(LEAP_LIBRARY_DIR
			NAMES
				Leap.lib
			PATHS
				$ENV{LEAP_DIR}/lib/x86
			DOC "The directory where Leap libraries are stored"
		)
	else(ARCH STREQUAL "x86")
		# 64-bit
		find_path(LEAP_LIBRARY_DIR
			NAMES
				Leap.lib
			PATHS
				$ENV{LEAP_DIR}/lib/x64
			DOC "The directory where Leap libraries are stored"
		)
	endif(ARCH STREQUAL "x86")

	if(LEAP_INCLUDE_DIR)
		set(LEAP_LIB_RELEASE "${LEAP_LIBRARY_DIR}/Leap.lib")
		set(LEAP_LIB_DEBUG "${LEAP_LIBRARY_DIR}/Leap.lib")
		set(LEAP_DLL_RELEASE "${LEAP_LIBRARY_DIR}/Leap.dll")
		set(LEAP_DLL_DEBUG "${LEAP_LIBRARY_DIR}/Leap.dll")
	endif(LEAP_INCLUDE_DIR)

else(WIN32)
	find_path(LEAP_INCLUDE_DIR
		NAMES
			Leap.h
		PATHS
			$ENV{LEAP_DIR}/include
		DOC "The directory where Leap.h resides"
	)

	find_path(LEAP_LIBRARY_DIR
		NAMES
			libLeap.dylib
		PATHS
			$ENV{LEAP_DIR}/lib/libc++/
		DOC "The directory where Leap libraries are stored"
	)

	if(LEAP_INCLUDE_DIR)
		set(LEAP_LIBRARY "${LEAP_LIBRARY_DIR}/libLeap.dylib")
	endif(LEAP_INCLUDE_DIR)
endif(WIN32)

if(LEAP_INCLUDE_DIR)
	set(LEAP_FOUND 1 CACHE BOOL "Leap SDK found")
else(LEAP_INCLUDE_DIR)
	set(LEAP_FOUND 0 CACHE BOOL "Leap SDK found")
endif(LEAP_INCLUDE_DIR)

mark_as_advanced(LEAP_FOUND)