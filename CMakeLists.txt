cmake_minimum_required(VERSION 3.27)
project(AtomicEngine)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Python REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

function(add_shaders TARGET_NAME)
    file(GLOB_RECURSE shader_src "shader/*.vert" "shader/*.frag")

    set(shader_commands)
    set(shader_outputs)

    foreach(shader_path IN LISTS shader_src)
        cmake_path(GET shader_path FILENAME shader_name)

        list(APPEND shader_commands COMMAND)
        list(APPEND shader_commands Vulkan::glslc)
        list(APPEND shader_commands ${shader_path})
        list(APPEND shader_commands -o)
        list(APPEND shader_commands ${CMAKE_CURRENT_BINARY_DIR}/${shader_name}.spv)

        list(APPEND shader_outputs ${CMAKE_CURRENT_BINARY_DIR}/${shader_name}.spv)
    endforeach()

    add_custom_target(
        ${TARGET_NAME} ALL
        ${shader_commands}
        COMMAND Python::Interpreter ${CMAKE_SOURCE_DIR}/shader_bundle.py ${CMAKE_SOURCE_DIR} ${shader_outputs}
        SOURCES ${shader_src}
        BYPRODUCTS ${shader_outputs}
    )
endfunction()

file(GLOB_RECURSE src "src/*.c")
file(GLOB_RECURSE headers "include/*.h")

add_shaders(Shaders)

add_executable(AtomicEngine ${src} ${headers})

target_include_directories(AtomicEngine PUBLIC "include/")
target_link_libraries(AtomicEngine vulkan glfw m)

if(DEFINED DEBUG)
    target_compile_definitions(AtomicEngine PUBLIC -DDEBUG=1)
    target_compile_options(AtomicEngine PUBLIC -g)
endif()

if(DEFINED TIMER)
    target_compile_options(AtomicEngine PUBLIC -pg)
    target_link_options(AtomicEngine PUBLIC -pg)
endif()

