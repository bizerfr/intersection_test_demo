project (intersection)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_BUILD_TYPE Realse)
#set(CMAKE_BUILD_TYPE Debug)
#message(STATUS "NOTICE: use debug model.")

cmake_minimum_required(VERSION 2.4.5)

set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )

if( COMMAND cmake_policy )
  cmake_policy( SET CMP0003 NEW )  
endif()

#Defines flags to emulate windows behavior for linking error generation
#if(CMAKE_CXX_COMPILER_ID EQUAL Clang
#   OR CMAKE_COMPILER_IS_GNUCC
#   OR CMAKE_COMPILER_IS_GNUCXX)
#  if(UNIX OR APPLE)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
#  endif()
#  if(UNIX AND NOT APPLE)
#    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z defs")
#    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -z defs")
#    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -z defs")
    
#    SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
#    SET(CMAKE_C_FLAGS_DEBUG "-g -O0")
#    SET(CMAKE_CXX_FLAGS_RELEASE "-g -O0")
#    SET(CMAKE_C_FLAGS_RELEASE "-g -O0")
#    message(STATUS "NOTICE: use -g -O0")
#  endif()
#endif()

find_package(Qt5 REQUIRED COMPONENTS Core Widgets OpenGL)
find_package(OpenGL)


set( HDRS glviewer.h scene.h main_window.h  geometric.h)

set( SRCS glviewer.cpp main.cpp main_window.cpp)


if(WIN32)
	add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4503 /nologo" )
endif(WIN32)



if( Qt5_FOUND AND OPENGL_FOUND )
    include_directories(BEFORE . ./build)

    SET(MY_BUILD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build)  #Variable build path
    SET(MY_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR})  #Variable src path
    INCLUDE_DIRECTORIES(${MY_BUILD_PATH})
    INCLUDE_DIRECTORIES(${MY_SRC_PATH})

    # UI files (Qt Designer files)
    qt5_wrap_ui( DT_UI_FILES intersection.ui)

    # The executable itself.
    add_executable( intersection ${SRCS} ${MOCS} ${DT_UI_FILES} )

    # Link with Qt libraries
    target_link_libraries(intersection Qt5::Core Qt5::Widgets Qt5::OpenGL)
    
    # Link with  OpenGL
    target_link_libraries( intersection ${OPENGL_LIBRARY})

else()
  message(STATUS "NOTICE: This program requires Qt5 and OpenGL")

endif()
