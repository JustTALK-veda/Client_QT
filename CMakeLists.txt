cmake_minimum_required(VERSION 3.16)

project(JustTalk VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
# find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui
        VideoThread.h
        VideoThread.cpp
        meeting.ui
        meeting.h
        meeting.cpp
        # Coordinate.h
        # TcpThread.h
        # TcpThread.cpp
        # lobby.h
        # lobby.cpp
        # config/rpi_ip.json
        #client_tls.c
        #client_tls.h

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

find_package(Qt6 6.9.1 REQUIRED COMPONENTS Widgets Core Network Multimedia MultimediaWidgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)

#Gstreamer/Glib/GIO설정

pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-video-1.0)
pkg_check_modules(GSTREAMER_APP REQUIRED gstreamer-app-1.0)
pkg_check_modules(GLIB REQUIRED glib-2.0 gobject-2.0 gio-2.0)
pkg_check_modules(GST_RTSP_SERVER REQUIRED gstreamer-rtsp-server-1.0)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(
    ${Qt6Widgets_INCLUDE_DIRS}
    ${GST_INCLUDE_DIRS}
    ${GSTREAMER_APP_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${GST_RTSP_SERVER_INCLUDE_DIRS}

)
message(STATUS "GLIB_INCLUDE_DIRS = ${GLIB_INCLUDE_DIRS}")

link_directories(
    ${GST_LIBRARY_DIRS}
    ${GSTREAMER_APP_LIBRARY_DIRS}
    ${GLIB_LIBRARY_DIRS}
    ${GST_RTSP_SERVER_LIBRARY_DIRS}

)
add_definitions(${GLIB_CFLAGS_OTHER})




qt_add_executable(JustTalk
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    Coordinate.h
    TcpThread.h TcpThread.cpp
    config/rpi_ip.json
    rtspserver.h RtspServer.cpp
    audio_control.h audio_control.cpp
    CameraWidget.h CameraWidget.cpp






    # client_tls.h client_tls.c
    # mypage.h mypage.cpp
)

qt_finalize_executable(JustTalk)

target_link_libraries(JustTalk PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Network
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    ${GST_LIBRARIES}
    ${GSTREAMER_APP_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${OpenCV_LIBS}
    ${OPENSSL_LIBRARIES}
    ${GST_RTSP_SERVER_LIBRARIES}
)
target_link_libraries(JustTalk PRIVATE Qt6::Widgets)

# macOS 전용 codesign + 번들 속성
if(APPLE)
    add_custom_command(TARGET JustTalk POST_BUILD
        COMMAND codesign --force --deep --sign - "${CMAKE_BINARY_DIR}/JustTalk.app"
    )

    set_target_properties(JustTalk PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.example.JustTalk
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
    )
else()
    set_target_properties(JustTalk PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# config 폴더 복사
add_custom_command(TARGET JustTalk POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/config
            $<TARGET_FILE_DIR:JustTalk>/config
)

# 설치 경로 지정
include(GNUInstallDirs)
install(TARGETS JustTalk
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
#     qt_add_executable(JustTalk
#         MANUAL_FINALIZATION
#         ${PROJECT_SOURCES}
#         videothread.h videothread.cpp
#     )
# # Define target properties for Android with Qt 6 as:
# #    set_property(TARGET JustTalk APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
# else()
#     if(ANDROID)
#         add_library(JustTalk SHARED
#             ${PROJECT_SOURCES}
#         )
# # Define properties for Android with Qt 5 after find_package() calls as:
# #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#     else()
#         add_executable(JustTalk
#             ${PROJECT_SOURCES}
#         )
#     endif()
# endif()

# target_link_libraries(JustTalk PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# # Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# # If you are developing for iOS or macOS you should consider setting an
# # explicit, fixed bundle identifier manually though.
# if(${QT_VERSION} VERSION_LESS 6.1.0)
#   set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.JustTalk)
# endif()
# set_target_properties(JustTalk PROPERTIES
#     ${BUNDLE_ID_OPTION}
#     MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#     MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#     MACOSX_BUNDLE TRUE
#     WIN32_EXECUTABLE TRUE
# )

# include(GNUInstallDirs)
# install(TARGETS JustTalk
#     BUNDLE DESTINATION .
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

# if(QT_VERSION_MAJOR EQUAL 6)
#     qt_finalize_executable(JustTalk)
# endif()
