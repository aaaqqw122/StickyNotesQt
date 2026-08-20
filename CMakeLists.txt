cmake_minimum_required(VERSION 3.5)
project(StickyNotesApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "D:/QT/6.11.1/msvc2022_64/lib/cmake")
find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(note
    src/main.cpp
    src/note_data.cpp
    src/note_data_manager.cpp
    src/note_widget.cpp
    src/note_list_widget.cpp
    src/note_manager.cpp
    src/custom_title_bar.cpp
    src/rich_text_toolbar.cpp
    src/rich_text_actions.cpp
    src/win11_style.cpp
    src/settings_dialog.cpp
)
set_target_properties(note PROPERTIES WIN32_EXECUTABLE TRUE)
target_link_libraries(note PRIVATE Qt6::Widgets)
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources"
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

if(WIN32)
    set(RESOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon/favicon.ico")
    if(EXISTS ${RESOURCE_FILE})
        set_source_files_properties(${RESOURCE_FILE} PROPERTIES
            MACOSX_PLIST_INFO "Icon file=${RESOURCE_FILE}")
        add_custom_command(TARGET note POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                "${RESOURCE_FILE}"
                "$<TARGET_FILE_DIR:note>/resources/icon/favicon.ico")
    endif()
endif()
