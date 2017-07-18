set(MODULE ${PROJECT_NAME}PythiaUtil)
set(NAME_LIB ${MODULE})

set(SOURCES_LIB
	pyargs.cxx
	pyutil.cxx
	outkinematics.cxx
	#pythia_wrapper.cxx
)

# Headers from sources
string(REPLACE ".cxx" ".h" HEADERS_LIB "${SOURCES_LIB}")

add_library(${NAME_LIB} SHARED ${SOURCES_LIB})
target_include_directories(${NAME_LIB} PUBLIC ${PYTHIA8_DIR}/include ${ROOT_INCLUDE_DIR} ${FASTJET_DIR}/include)
target_link_libraries(${NAME_LIB} ${PYTHIA8_LDFLAGS} ${ROOT_LIBRARIES} ${FASTJET_LIBS} ${PROJECT_NAME}Util)

install(TARGETS ${NAME_LIB} LIBRARY DESTINATION lib)

install(FILES ${HEADERS_LIB} DESTINATION include/${MODULE})
