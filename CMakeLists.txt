cmake_minimum_required(VERSION 3.0)
project("pn" VERSION 0.9.0)

include(CheckIncludeFiles)

find_library(
	LIBPHONENUMBER
	phonenumber
	PATHS ENV EXTRA_LIB_PATH
)

find_library(
	LIBGEOCODING
	geocoding
	PATHS ENV EXTRA_LIB_PATH
)

find_library(
	LIBICUUC
	icuuc
	PATHS ENV EXTRA_LIB_PATH
)

include_directories(
	BEFORE
	SYSTEM ${EXTRA_INCLUDE_PATH}
)

check_include_files(
	"stdio.h;errno.h;stdio.h;stddef.h;string.h;stdlib.h;sys/types.h;sys/stat.h;gawkapi.h"
	HAVE_GAWK_H
)

add_compile_definitions(VERSION=\"${PROJECT_VERSION}\")

if(${LIBPHONENUMBER-NOTFOUND})
	message(FATAL_ERROR "could not find prerequisite: libphonenumber")
endif(${LIBPHONENUMBER-NOTFOUND})

if(${LIBGEOCODING-NOTFOUND})
	message(FATAL_ERROR "could not find prerequisite: libgeocoding")
endif(${LIBGEOCODING-NOTFOUND})

if(${LIBICUUC-NOTFOUND})
	message(FATAL_ERROR "could not find prerequisite: libicuuc")
endif(${LIBICUUC-NOTFOUND})

add_executable(
	pn
	src/pn.cc
)

target_link_libraries(
	pn
	${LIBICUUC}
	${LIBPHONENUMBER}
	${LIBGEOCODING}
)

install(
	TARGETS pn
	RUNTIME DESTINATION bin
)

install(
	FILES pn.1
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man1
)

if( HAVE_GAWK_H )
	add_library(
		gawkpn
		SHARED
		gawkext/gawkpnwrap.c
		gawkext/gawkpnimpl.cc
	)

	set_target_properties(gawkpn PROPERTIES PREFIX "")

	target_link_libraries(
		gawkpn
		${LIBICUUC}
		${LIBPHONENUMBER}
		${LIBGEOCODING}
	)

	execute_process(
		COMMAND
			gawk "BEGIN {print ENVIRON[\"AWKLIBPATH\"]}"
		OUTPUT_VARIABLE
			AWKLIBPATH
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	install(
		TARGETS gawkpn
		DESTINATION ${AWKLIBPATH}
	)
endif( HAVE_GAWK_H )
