add_library(parser_base_lib STATIC 
	parser/parser.c
	parser/debug.c
	parser/symtab.c
	parser/semantics.c
)
target_link_libraries(parser_base_lib PUBLIC
	scanner_base_lib
)

add_executable(run_parser parser/main.c)
target_link_libraries(run_parser PRIVATE
	parser_base_lib
)

# Testing lists
list(APPEND Inputs	example1 example2 example3 example4)
list(APPEND Results	result1 result2 result3 result4)

# Define a test
if (BASH)
	enable_testing()

	set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
	set(EXE $<TARGET_FILE:run_parser>)

	foreach(file IN ZIP_LISTS Inputs Results)
		add_test(
			NAME ${file_0}.kpl 
			COMMAND ${BASH} -c 
			"\"${EXE}\" \"${TEST_DIR}/${file_0}.kpl\" | colordiff - \"${TEST_DIR}/${file_1}.txt\" -y -W 80"
		)
	endforeach()

else()
	message(STATUS "Tests are not available for this platform (requiring Bash/Linux)")
endif()