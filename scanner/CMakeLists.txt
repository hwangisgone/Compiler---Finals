add_library(scanner_base_lib STATIC 
	scanner/scanner.c
	scanner/charcode.c
	scanner/token.c
	scanner/error.c
	scanner/reader.c
)
target_include_directories(scanner_base_lib PUBLIC scanner)

add_executable(run_scanner scanner/main.c)
target_link_libraries(run_scanner PRIVATE
	scanner_base_lib
)




# Define a test
if (BASH)
	enable_testing()

	set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
	set(EXE $<TARGET_FILE:run_scanner>)
	list(APPEND Inputs	example1 example2 example3	error1 error2 error3 error4 error5)
	list(APPEND Results	result1 result2 result3			erout1 erout2 erout3 erout4 erout5)

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