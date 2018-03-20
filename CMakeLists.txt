cmake_minimum_required(VERSION 3.5)

PROJECT(NAVIRICE_RENDERER)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_BINARY_DIR}")

set(CMAKE_CXX_STANDARD 11)

set(SOURCES
	src/tracegrid.c
	src/filesys.c
	src/vbomanager.c
	src/mathlib.c
	src/stringlib.c
	src/viewportmanager.c
	src/cvarmanager.c
	src/matrixlib.c
	src/modelmanager.c
	src/hashtables.c
	src/glfwmanager.c
	src/planebox.c
	src/shadermanager.c
	src/glmanager.c
	src/fsquad.c
	src/contextmanager.c
	src/entitymanager.c
	src/worldrenderer.c
	src/headclient.c
	src/gamecodemanager.c
	src/bboxrenderer.c
	src/navmanager.c
	src/stb_truetype_precompile.c
	src/ubomanager.c
	src/text.c
	src/renderqueue.c
	src/naviclient.c
	src/firstbouncerenderer.c
	src/proto/requestHeader.pb-c.c
	src/proto/step.pb-c.c
	src/proto/response.pb-c.c
	)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "COPIED SHADERS SRC")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/models
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "COPIED MODELS SRC")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/planeboxes
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "COPIED PLANEBOXES SRC")

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS} )

set(LIBS ${OPENGL_LIBRARIES} glfw)

set(CMAKE_C_FLAGS "-Wall -Ofast -fstrict-aliasing -march=native -g")
set(CMAKE_CXX_FLAGS "-Wall -Ofast -fstrict-aliasing -march=native -g")

add_executable(renderer src/renderer.c ${SOURCES})
target_link_libraries(renderer -lepoxy -lm -ldl -lprotobuf-c ${LIBS})

