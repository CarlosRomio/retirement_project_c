cmake_minimum_required (VERSION 3.8)

project(reiment VERSION 1.0
    DESCRIPTION "Reiment"
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# LTO
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

function(load_system)
    if (MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4250")
    elseif (APPLE)
        set(GUI_OPTION MACOSX_BUNDLE)
    else()
        add_compile_options("-Werror")
    endif()
endfunction()

load_system()

# Sources
file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

list(FILTER SRC_FILES EXCLUDE REGEX ".*CMakeCXXCompilerId.cpp")

add_executable(${PROJECT_NAME} ${GUI_OPTION} ${SRC_FILES})

# Includes
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# OpenSSL (opcional mas recomendado)
find_package(OpenSSL)
if (OpenSSL_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()

# Drogon
find_package(Drogon REQUIRED)

# Banco
find_package(libpqxx REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Drogon::Drogon
    pqxx
)

# Otimizações
if(CMAKE_BUILD_TYPE MATCHES "Release|MinSizeRel")
    message(STATUS "Aplicando otimizações de tamanho para o build ${CMAKE_BUILD_TYPE}")

    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

    target_compile_options(${PROJECT_NAME} PRIVATE -ffunction-sections -fdata-sections)
    target_link_options(${PROJECT_NAME} PRIVATE "-Wl,--gc-sections")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}>
        COMMENT "Stripping symbols from ${PROJECT_NAME}...")
endif()

# Migrations
target_compile_definitions(${PROJECT_NAME} PRIVATE
    MIGRATIONS_PATH="${CMAKE_SOURCE_DIR}/src/database/migrations"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/database/migrations
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/migrations
)