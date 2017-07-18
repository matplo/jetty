set(SOURCES_LIB
	run_pythia.cxx
	fjutils.cxx
)

set(MODULE ${PROJECT_NAME}SubJets)
set(NAME_LIB ${MODULE})
set(SOURCES_EXE main.cxx)
set(NAME_EXE ${MODULE}Exe)

# Headers from sources
string(REPLACE ".cxx" ".h" HEADERS_LIB "${SOURCES_LIB}")
string(REPLACE ".cxx" ".h" HEADERS_EXE "${SOURCES_EXE}")

add_library(${NAME_LIB} SHARED ${SOURCES_LIB})
target_include_directories(${NAME_LIB} PUBLIC ${ROOT_INCLUDE_DIR} ${FASTJET_DIR}/include ${PYTHIA8_DIR}/include)
target_link_libraries(${NAME_LIB} ${PYTHIA8_LDFLAGS} ${FASTJET_LIBS} libRecursiveTools.a ${ROOT_LIBRARIES} jettyUtil)

add_executable(${NAME_EXE} ${SOURCES_EXE})
target_include_directories(${NAME_EXE} PUBLIC ${ROOT_INCLUDE_DIR} ${FASTJET_DIR}/include ${PYTHIA8_DIR}/include)
target_link_libraries(${NAME_EXE} ${NAME_LIB} jettyUtil)

install(TARGETS ${NAME_EXE} RUNTIME DESTINATION bin)

install(TARGETS ${NAME_LIB} LIBRARY DESTINATION lib)

install(FILES ${HEADERS_LIB} DESTINATION include/${MODULE})
