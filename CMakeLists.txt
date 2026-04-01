cmake_minimum_required(VERSION 3.16)

project(pycdc-studio VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network LinguistTools)

set(TS_FILES
    i18n/pycdc_studio_zh_CN.ts
)

set(PROJECT_SOURCES
    main.cpp
    src/app/app_settings.cpp
    src/app/app_settings.h
    src/app/app_context.cpp
    src/app/app_context.h
    src/ai/ai_provider_config.cpp
    src/ai/ai_provider_config.h
    src/ai/openai_compatible_client.cpp
    src/ai/openai_compatible_client.h
    src/ai/prompt_builder.cpp
    src/ai/prompt_builder.h
    src/model/code_object_node.h
    src/model/decompile_result.h
    src/model/ai_reconstruction_result.h
    src/model/project_session.cpp
    src/model/project_session.h
    src/service/pycdc_process_runner.cpp
    src/service/pycdc_process_runner.h
    src/service/pycdas_process_runner.cpp
    src/service/pycdas_process_runner.h
    src/service/pycdas_tree_parser.cpp
    src/service/pycdas_tree_parser.h
    src/service/decompiler_service.cpp
    src/service/decompiler_service.h
    src/service/fallback_service.cpp
    src/service/fallback_service.h
    src/ui/lucide_icon_factory.cpp
    src/ui/lucide_icon_factory.h
    src/ui/main_window.cpp
    src/ui/main_window.h
    src/ui/model_picker_dialog.cpp
    src/ui/model_picker_dialog.h
    src/ui/settings_dialog.cpp
    src/ui/settings_dialog.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(pycdc-studio
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(pycdc-studio SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(pycdc-studio
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(pycdc-studio PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(pycdc-studio PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

set_source_files_properties(resources/styles/app.qss PROPERTIES
    QT_RESOURCE_ALIAS "styles/app.qss"
)

qt_add_resources(pycdc-studio "app_resources"
    PREFIX "/"
    FILES
        resources/styles/app.qss
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_add_translations(pycdc-studio
        TS_FILES ${TS_FILES}
        QM_FILES_OUTPUT_VARIABLE QM_FILES
    )

    foreach(qm_file IN LISTS QM_FILES)
        get_filename_component(qm_name "${qm_file}" NAME)
        set_source_files_properties("${qm_file}" PROPERTIES
            QT_RESOURCE_ALIAS "${qm_name}"
        )
    endforeach()

    qt_add_resources(pycdc-studio "i18n_translations"
        PREFIX "/i18n"
        FILES ${QM_FILES}
    )
endif()

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.pycdc-studio)
endif()
set_target_properties(pycdc-studio PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS pycdc-studio
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(pycdc-studio)
endif()
