cmake_minimum_required(VERSION 3.18)

project(KPL_Language VERSION 1.0 LANGUAGES C)

# For testing (available only in linux)
include(FindUnixCommands)

add_subdirectory(scanner)
add_subdirectory(parser)
