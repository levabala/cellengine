project(cellengine)

add_library(cell SHARED cell.c)
add_library(print SHARED print.c)

add_executable(main main.c)

target_link_libraries(main cell print)
