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

if(${LIBPHONENUMBER-NOTFOUND})
	message(FATAL_ERROR "could not find prerequisite: libphonenumber")
endif(${LIBPHONENUMBER-NOTFOUND})

if(${LIBGEOCODING-NOTFOUND})
	message(FATAL_ERROR "could not find prerequisite: libgeocoding")
endif(${LIBGEOCODING-NOTFOUND})

add_executable(
	pn
	src/pn.cc
)

target_link_libraries(
	pn
	icuuc
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
