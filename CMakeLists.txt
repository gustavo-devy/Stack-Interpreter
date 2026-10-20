cmake_minimum_required(VERSION 3.20)

project(Stack-Interpreter VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SOURCE_FILES src/main.c
    src/file/file.c
    src/lexer/lexer.c
    src/parser/parser.c
    src/compile/compile.c
    src/vm/vm.c
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

if (MSVC)
    target_compile_options(Stack-Interpreter PRIVATE /W4)
else ()
    target_compile_options(Stack-Interpreter PRIVATE -Wall -Wextra -Wpedantic -g)
endif ()

target_include_directories(${PROJECT_NAME} PRIVATE include)
