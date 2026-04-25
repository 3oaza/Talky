cmake_minimum_required(VERSION 3.25)
project(Talky LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Dependency Discovery
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)
find_package(onnxruntime REQUIRED)
find_package(spdlog REQUIRED)
find_package(Eigen3 REQUIRED)

# 2. Project Structure
include_directories(include)

set(PROJECT_SOURCES
    src/main.cpp
    include/app_controller.hpp
    include/audio_engine.hpp
    include/audio_output.hpp
    include/tts_manager.hpp
    include/translation_manager.hpp
    include/logger.hpp
)

# 3. Executable Definition
qt_add_executable(Talky ${PROJECT_SOURCES})

# 4. Compiler & Optimization
if(MSVC)
    target_compile_options(Talky PRIVATE /O2 /Ox /Oi /Ot /GL /arch:AVX2)
    target_link_options(Talky PRIVATE /LTCG /OPT:REF /OPT:ICF)
    target_compile_definitions(Talky PRIVATE NDEBUG)
endif()

# 5. Link Dependencies
target_link_libraries(Talky PRIVATE 
    Qt6::Quick
    Qt6::QuickControls2
    onnxruntime
    spdlog::spdlog
    Eigen3::Eigen
    ole32.lib
    dsound.lib
    winmm.lib
)

# 6. Deployment
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    install(TARGETS Talky RUNTIME DESTINATION bin)
    qt_generate_deploy_app_script(TARGET Talky OUTPUT_SCRIPT deploy_script)
    install(SCRIPT ${deploy_script})
endif()
