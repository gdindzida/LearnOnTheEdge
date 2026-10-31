add_executable(tensor_lib_benchmark tensor_benchmark.cpp)
target_link_libraries(tensor_lib_benchmark PRIVATE tensor_lib_static benchmark::benchmark_main)
target_compile_options(tensor_lib_benchmark PRIVATE
    "$<$<AND:$<CONFIG:Release>,$<COMPILE_LANG_AND_ID:CXX,GNU>>:-falign-loops=32>"
)

