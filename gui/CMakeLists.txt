cmake_minimum_required(VERSION 3.16)

project(ft_otp_gui VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(CORE_SOURCES
        ../core/TOTPGenerator.cpp
        ../core/FileHandler.cpp
        ../core/TOTPGenerator.hpp
        ../core/FileHandler.hpp
        ../core/qrencode.cpp
        ../core/qrencode.hpp
        ../core/qrgenerator.cpp
        ../core/qrgenerator.hpp
)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ft_otp_gui
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${CORE_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ft_otp_gui APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ft_otp_gui SHARED
            ${PROJECT_SOURCES}
        	${CORE_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ft_otp_gui
            ${PROJECT_SOURCES}
        	${CORE_SOURCES}
        )
    endif()
endif()

# Add the core directory to the include paths
target_include_directories(ft_otp_gui PRIVATE ../core)
target_link_libraries(ft_otp_gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets cryptopp qrencode png)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ft_otp_gui)
endif()
set_target_properties(ft_otp_gui PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ft_otp_gui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ft_otp_gui)
endif()
