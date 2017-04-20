cmake_minimum_required(VERSION 3.0)
project("pn")

find_library(
	LIBPHONENUMBER
	phonenumber
)

if(${LIBPHONENUMBER-NOTFOUND})
	message(FATAL_ERROR "could not find prerequisite: libphonenumber")
endif(${LIBPHONENUMBER-NOTFOUND})

add_executable(
	pn
	src/pn.cc
)

target_link_libraries(
	pn
	${LIBPHONENUMBER}
)

install(
	TARGETS pn
	RUNTIME DESTINATION bin
)
