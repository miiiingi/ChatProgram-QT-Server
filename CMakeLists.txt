cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# 프로젝트 이름과 C++ 표준 설정
project(ChatProgramServer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# Qt5 모듈 찾기
find_package(Qt5 COMPONENTS Core Gui Widgets Network Multimedia MultimediaWidgets Sql REQUIRED)

# GStreamer 및 GLib 찾기
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0 gstreamer-video-1.0 gstreamer-rtsp-server-1.0)

# 소스 파일 및 헤더 파일 추가
set(SOURCES
    main.cpp
    mainwindow.cpp
)

set(HEADERS
    mainwindow.h
)

set(UI
    mainwindow.ui
)

set(TRANSLATIONS
    ChatProgram-Server_ko_KR.ts
)

# UI 파일 처리
qt5_wrap_ui(UIS_HEADERS ${UI})

# Meta-Object Compiler(MOC) 헤더 처리
qt5_wrap_cpp(MOC_HEADERS ${HEADERS})


# 실행 파일 생성
add_executable(ChatProgramServer ${SOURCES} ${MOC_HEADERS} ${UIS_HEADERS} ${QM_FILES})

# include 디렉터리 설정
target_include_directories(ChatProgramServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GSTREAMER_INCLUDE_DIRS}
)

# 라이브러리 링크
target_link_libraries(ChatProgramServer
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    Qt5::Multimedia
    Qt5::MultimediaWidgets
    Qt5::Sql
    ${GSTREAMER_LIBRARIES}
)

# RPATH 설정
set_target_properties(ChatProgramServer PROPERTIES
    INSTALL_RPATH "/usr/local/lib"
)

# 설치 경로 설정
install(TARGETS ChatProgramServer DESTINATION /opt/ChatProgramServer/bin)

