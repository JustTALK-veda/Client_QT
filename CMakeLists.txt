cmake_minimum_required(VERSION 3.16)

project(mytest_cut VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 공통 소스 파일
set(PROJECT_SOURCES
    main.cpp
    Coordinate.h
    TcpThread.h
    TcpThread.cpp
    MainWindow.h
    MainWindow.cpp
    VideoThread.h
    VideoThread.cpp
    lobby.h
    lobby.cpp
    config/rpi_ip.json
)

#  window/mac 설정

if(APPLE)
    message(STATUS "macOS용 설정")

    # macOS용 OpenSSL
    set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl@3")
    set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
    set(OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libssl.dylib")
    set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libcrypto.dylib")
    set(OPENSSL_LIBRARIES ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})

    # macOS 번들 설정
    set(MACOSX_BUNDLE TRUE)
    set(MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Info.plist")

elseif(WIN32)
    message(STATUS "Windows용 설정")

    # Windows용 OpenSSL
    set(OPENSSL_ROOT_DIR "C:/OpenSSL-Win64")
    set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
    set(OPENSSL_LIBRARIES "${OPENSSL_ROOT_DIR}/lib/libssl.lib" "${OPENSSL_ROOT_DIR}/lib/libcrypto.lib")

    # Windows용 OpenCV 경로(맞춰서 설치)
    set(OpenCV_DIR "C:/opencv/build")
endif()

# 공용

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

find_package(Qt6 6.9.1 REQUIRED COMPONENTS Widgets Core Network Multimedia)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-video-1.0)
pkg_check_modules(GSTREAMER_APP REQUIRED gstreamer-app-1.0)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(
    ${Qt6Widgets_INCLUDE_DIRS}
    ${GST_INCLUDE_DIRS}
    ${GSTREAMER_APP_INCLUDE_DIRS}
)

link_directories(
    ${GST_LIBRARY_DIRS}
    ${GSTREAMER_APP_LIBRARY_DIRS}
)

qt_add_executable(mytest_cut
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

qt_finalize_executable(mytest_cut)

target_link_libraries(mytest_cut PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Network
    Qt6::Multimedia
    ${GST_LIBRARIES}
    ${GSTREAMER_APP_LIBRARIES}
    ${OpenCV_LIBS}
    ${OPENSSL_LIBRARIES}
)

# macOS 전용 codesign + 번들 속성
if(APPLE)
    add_custom_command(TARGET mytest_cut POST_BUILD
        COMMAND codesign --force --deep --sign - "${CMAKE_BINARY_DIR}/mytest_cut.app"
    )

    set_target_properties(mytest_cut PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.example.mytest_cut
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
    )
else()
    set_target_properties(mytest_cut PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# config 폴더 복사
add_custom_command(TARGET mytest_cut POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/config
            $<TARGET_FILE_DIR:mytest_cut>/config
)

# 설치 경로 지정
include(GNUInstallDirs)
install(TARGETS mytest_cut
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
