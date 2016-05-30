cmake_minimum_required (VERSION 2.8)
project (buggiPlayer)

include_directories (${SOLUTION_DIR}/../libs_src/freeglut/include
                     ${SOLUTION_DIR}/../libs_src/glew-1.12.0/include
                     ${SOLUTION_DIR}/../libs_src/boost
                     ${SOLUTION_DIR}/../libs_src/tclap-1.2.1/include
                     ${SOLUTION_DIR}/
                     ${SOLUTION_DIR}/../libs_src/glm-0.9.7.1
                     ${SOLUTION_DIR}/src/buggiLib
                     ${SOLUTION_DIR}/src/common
                     )

set(SOURCE_TERRAIN ${PROJECT_SOURCE_DIR}/terrain/Terrain.cpp
                   ${PROJECT_SOURCE_DIR}/terrain/World.cpp
                   )
set(HEADER_TERRAIN ${PROJECT_SOURCE_DIR}/terrain/Terrain.h
                   ${PROJECT_SOURCE_DIR}/terrain/World.h
                   )

set(SOURCE_CASTLEFEST   ${PROJECT_SOURCE_DIR}/main.cpp )
		 
set(ALL_CASTLEFEST_SOURCES  ${SOURCE_CASTLEFEST}
							${HEADER_TERRAIN}
                            ${SOURCE_TERRAIN} )

SOURCE_GROUP("Resources\\oldShaders" FILES ${SOLUTION_DIR}/../resources/shaders/shaders_oldGL/FragmentShader.fs ${SOLUTION_DIR}/../resources/shaders/shaders_oldGL/VertexShader.vs)
SOURCE_GROUP("Resources\\newShaders" FILES ${SOLUTION_DIR}/../resources/shaders/shaders_newGL/FragmentShader.fs ${SOLUTION_DIR}/../resources/shaders/shaders_newGL/VertexShader.vs)

if(WIN32)
    add_executable (CastleFest WIN32 ${ALL_CASTLEFEST_SOURCES} )
else()
    add_executable (CastleFest ${ALL_CASTLEFEST_SOURCES} )
endif()

add_subdirectory(../buggiLib BuggiLib)

SOURCE_GROUP("Resources\\oldShaders" FILES ${SOLUTION_DIR}/../resources/shaders/shaders_oldGL/FragmentShader.fs ${SOLUTION_DIR}/../resources/shaders/shaders_oldGL/VertexShader.vs)
SOURCE_GROUP("Resources\\newShaders" FILES ${SOLUTION_DIR}/../resources/shaders/shaders_newGL/FragmentShader.fs ${SOLUTION_DIR}/../resources/shaders/shaders_newGL/VertexShader.vs)

if(WIN32)
    target_link_libraries( CastleFest debug ${SOLUTION_DIR}\\..\\libs_bin\\Windows\\debug\\glew32sd.lib
                                      debug ${SOLUTION_DIR}\\..\\libs_bin\\Windows\\debug\\freeglut_staticd.lib
                                      debug ${SOLUTION_DIR}\\..\\libs_bin\\Windows\\debug\\opencv_world310d.lib)

    target_link_libraries( CastleFest optimized ${SOLUTION_DIR}\\..\\libs_bin\\Windows\\release\\glew32s.lib
                                      optimized ${SOLUTION_DIR}\\..\\libs_bin\\Windows\\release\\freeglut_static.lib
                                      optimized ${SOLUTION_DIR}\\..\\libs_bin\\Windows\\release\\opencv_world310.lib)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    link_directories( ${OGL_PATH} )
    link_directories( ${SOLUTION_DIR}/../libs_bin/ubuntu/ )
    include_directories ( ${SOLUTION_DIR}/../libs_src/ )
    target_link_libraries( CastleFest ${SOLUTION_DIR}/../libs_bin/ubuntu/libglut.a )
endif()


target_link_libraries( CastleFest BuggiLib )

if(WIN32)
set(WINDOWS_SPECIFIC_COPY COMMAND ${CMAKE_COMMAND} -E copy_directory  ${SOLUTION_DIR}/../resources/shaders/. ${PROJECT_BINARY_DIR}/Debug/shaders/.
                          COMMAND ${CMAKE_COMMAND} -E copy_directory  ${SOLUTION_DIR}/../resources/shaders/. ${PROJECT_BINARY_DIR}/Release/shaders/. )
endif(WIN32)

add_custom_target(
  CastleFestShadersResources ALL
    #copy resourses, which olways needed to be up to date. For example shaders - you can modify them, and just build - updated shader will be copyed to correct places.
    #Do not place big files here!!! This copy will perform after every build!!!!!
    COMMAND ${CMAKE_COMMAND} -E copy_directory  ${SOLUTION_DIR}/../resources/shaders/. ${PROJECT_BINARY_DIR}/shaders
    ${WINDOWS_SPECIFIC_COPY}
)

add_dependencies (CastleFest CastleFestShadersResources)


