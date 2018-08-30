set(LIB_NAME DbLib)

set(HEADER_FILES
	db.h
)

set(SOURCE_FILES
	db.cpp
)


add_library(
	${LIB_NAME}
	STATIC
	${HEADER_FILES}
	${SOURCE_FILES}
)

target_link_libraries(
	${LIB_NAME}
	Qt5::Sql
)