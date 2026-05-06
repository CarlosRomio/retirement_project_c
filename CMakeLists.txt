cmake_minimum_required (VERSION 3.8)

project(reiment VERSION 1.0
    DESCRIPTION "Reiment"
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ativando LTO para melhorar o tempo de build
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
option(CROW_USE_BOOST "Use Boost.Asio for Crow" ON)

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

# Gather source files
file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/crowlib/*
)

# Exclude build and CMake-specific files
list(FILTER SRC_FILES EXCLUDE REGEX ".*CMakeCXXCompilerId.cpp")

# Create the executable
add_executable(${PROJECT_NAME} ${GUI_OPTION} ${SRC_FILES})

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include/crowlib
)


# Configuração do Boost (modificada para o Crow)
find_package(Boost COMPONENTS system date_time REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::system pthread)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES} Boost::boost Boost::system Boost::date_time)
target_compile_definitions(${PROJECT_NAME} PUBLIC CROW_USE_BOOST_ASIO)

# --- Opcional: Se precisar de OpenSSL (para HTTPS) ---
find_package(OpenSSL)
if (OpenSSL_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()
## --- Fim da configuração do Boost ---

# Aplica otimizações de tamanho apenas para builds de Release ou MinSizeRel
if(CMAKE_BUILD_TYPE MATCHES "Release|MinSizeRel")
    message(STATUS "Aplicando otimizações de tamanho para o build ${CMAKE_BUILD_TYPE}")

    # Ativa a Otimização no Tempo de Link (LTO)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

    # Adiciona flags para remover código não utilizado
    target_compile_options(${PROJECT_NAME} PRIVATE -ffunction-sections -fdata-sections)
    target_link_options(${PROJECT_NAME} PRIVATE "-Wl,--gc-sections")

    # Adiciona um passo pós-build para remover símbolos do executável
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}>
                       COMMENT "Stripping symbols from ${PROJECT_NAME}...")
endif()