cmake_minimum_required(VERSION 3.15)
project(Pok3Dex)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories to match VS structure
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/x64/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/x64/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/x64/$<CONFIG>)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)
find_package(OpenAL REQUIRED)
find_package(Freetype REQUIRED)

# Add FreeGLUT
if(WIN32)
    set(FREEGLUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/freeglut")
    include_directories(${FREEGLUT_DIR}/include)
    link_directories(${FREEGLUT_DIR}/lib)
endif()

# Add source files
set(SOURCES
    main.cpp
)

# Add header files
set(HEADERS
    # Add your header files here if any
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${OPENGL_INCLUDE_DIR}
    ${ASSIMP_INCLUDE_DIR}
    ${OPENAL_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/libs
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${OPENGL_LIBRARIES}
    assimp::assimp
    ${OPENAL_LIBRARY}
    ${FREETYPE_LIBRARIES}
    freeglut
    glew32
)

# Copy DLLs to output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/OpenAL32.dll"
        "${CMAKE_CURRENT_SOURCE_DIR}/brotlicommon.dll"
        "${CMAKE_CURRENT_SOURCE_DIR}/brotlidec.dll"
        "${CMAKE_CURRENT_SOURCE_DIR}/bz2.dll"
        "${CMAKE_CURRENT_SOURCE_DIR}/freeglut.dll"
        "${CMAKE_CURRENT_SOURCE_DIR}/freetype.dll"
        "${CMAKE_CURRENT_SOURCE_DIR}/glew32.dll"
        "${CMAKE_CURRENT_SOURCE_DIR}/libpng16.dll"
        "${CMAKE_CURRENT_SOURCE_DIR}/zlib1.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# Copy assets
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
)

# Copy DLLs to output directory (Windows only)
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FREEGLUT_DIR}/bin/freeglut.dll"
            "${CMAKE_CURRENT_SOURCE_DIR}/libs/glew32.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif() 