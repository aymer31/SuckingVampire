include(GenerateExportHeader)

add_library(common 
    Common/common.cpp
    Common/common.h
    Common/movment.h
    Common/movment.cpp)
 

target_link_libraries(
    common PUBLIC
    sfml-system sfml-window sfml-graphics
)

target_include_directories(
    common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)   


add_library(game SHARED
    Game/game.cpp
    Game/game.h
 )

target_link_libraries(
    game PUBLIC
    sfml-system sfml-window sfml-graphics common
)

target_include_directories(
    game PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)   

set_target_properties(game PROPERTIES DEFINE_SYMBOL GAME_EXPORTS)
generate_export_header(game
    BASE_NAME        GAME
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/game_export.h
)

add_library(menu SHARED
    Menu/menu.cpp
    Menu/menu.h
 )

target_link_libraries(
    menu PUBLIC
    sfml-system sfml-window sfml-graphics common game
)

target_include_directories(
    menu PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(menu PROPERTIES DEFINE_SYMBOL MENU_EXPORTS)
generate_export_header(menu
    BASE_NAME        MENU
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/menu_export.h
)

add_executable(BaseProject main.cpp "Common/movment.cpp")
target_link_libraries(BaseProject PUBLIC 
sfml-system sfml-window sfml-graphics menu
)
