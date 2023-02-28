project(cellengine)

add_library(cell SHARED cell.c)
add_library(print SHARED print.c)

add_executable(main main.c)

target_link_libraries(main cell print)
target_compile_features(main PUBLIC c_std_99)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wextra -pedantic -Wall")
