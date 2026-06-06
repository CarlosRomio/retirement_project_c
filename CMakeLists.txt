cmake_minimum_required(VERSION 3.8)

project(reiment VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

function(load_system)
    if (MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4250")
    elseif (APPLE)
        set(GUI_OPTION MACOSX_BUNDLE)
    endif()
endfunction()

load_system()

# Sources
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
list(FILTER SRC_FILES EXCLUDE REGEX ".*CMakeCXXCompilerId.cpp")

add_executable(${PROJECT_NAME} ${GUI_OPTION} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE src)

# OpenSSL
find_package(OpenSSL)
if (OpenSSL_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()

# Drogon
find_package(Drogon REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)

# 🔥 pqxx (modo compatível com seu sistema)
target_link_libraries(${PROJECT_NAME} PRIVATE pqxx pq)

# Otimizações
if(CMAKE_BUILD_TYPE MATCHES "Release|MinSizeRel")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

    target_compile_options(${PROJECT_NAME} PRIVATE -ffunction-sections -fdata-sections)
    target_link_options(${PROJECT_NAME} PRIVATE "-Wl,--gc-sections")
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