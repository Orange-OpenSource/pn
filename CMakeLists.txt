cmake_minimum_required(VERSION 3.0)
project("pn")

find_library(
	LIBPHONENUMBER
	phonenumber
)

find_library(
	LIBGEOCODING
	geocoding
)

find_library(
	LIBICUUC
	icuuc
)

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
	DESTINATION ${CMAKE_INSTALL_PREFIX}/man/man1
)
