cmake_minimum_required (VERSION 3.22)

project(ftxui-starter
  LANGUAGES CXX
  VERSION 1.0.0
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

# --- Fetch FTXUI --------------------------------------------------------------
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/arthursonzogni/ftxui.git
  GIT_TAG        v6.1.9
  GIT_PROGRESS   TRUE
  GIT_SHALLOW    TRUE
  EXCLUDE_FROM_ALL
)
FetchContent_Declare(googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
  GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(ftxui googletest)
# ------------------------------------------------------------------------------

# --- Common libraries ---------------------------------------------------------
set(COMMON_INCLUDE_DIR include)

# Core game logic (undo/redo, saving)
add_library(core_logic OBJECT
  src/UndoRedo.cpp
  src/UndoRedoManager.cpp
  src/BinaryGameSaver.cpp
  src/TextGameSaver.cpp
)
target_include_directories(core_logic PRIVATE ${COMMON_INCLUDE_DIR})

# TUI components (static library to properly link FTXUI)
add_library(tui_lib STATIC
  src/TuiRenderer.cpp
  src/PuzzlePresenter.cpp
  src/PathResolver.cpp
  src/InputModeHandler.cpp
  src/ui/TileRenderer.cpp
  src/ui/GridRenderer.cpp
  src/ui/StatsPanelRenderer.cpp
  src/ui/MessagePanelRenderer.cpp
  src/ui/ControlsPanelRenderer.cpp
  src/ui/GameLayoutComposer.cpp
)
target_include_directories(tui_lib
  PRIVATE ${COMMON_INCLUDE_DIR}
  PUBLIC include
)
target_link_libraries(tui_lib
  PUBLIC ftxui::screen
  PUBLIC ftxui::dom
  PUBLIC ftxui::component
)

# Helper macro for FTXUI linkage
macro(link_ftxui target)
  target_link_libraries(${target}
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
  )
endmacro()
# ------------------------------------------------------------------------------

# --- Main executable ---------------------------------------------------------
add_executable(ftxui-starter
  src/main.cpp
  $<TARGET_OBJECTS:core_logic>
)
target_include_directories(ftxui-starter PRIVATE ${COMMON_INCLUDE_DIR})
target_link_libraries(ftxui-starter PRIVATE tui_lib)

# --- Tests (no TUI) -----------------------------------------------------------
add_executable(test-stats tests/test_stats.cpp)
target_include_directories(test-stats PRIVATE ${COMMON_INCLUDE_DIR})
add_test(NAME TestStats COMMAND test-stats)

add_executable(test-engine tests/test_engine.cpp)
target_include_directories(test-engine PRIVATE ${COMMON_INCLUDE_DIR})
add_test(NAME TestEngine COMMAND test-engine)

add_executable(test-heuristics tests/test_heuristics.cpp $<TARGET_OBJECTS:core_logic>)
target_include_directories(test-heuristics PRIVATE ${COMMON_INCLUDE_DIR})
add_test(NAME TestHeuristics COMMAND test-heuristics)

add_executable(test-moveadvisor tests/test_moveadvisor.cpp)
target_include_directories(test-moveadvisor PRIVATE ${COMMON_INCLUDE_DIR})
add_test(NAME TestMoveAdvisor COMMAND test-moveadvisor)

add_executable(test-undoredomanager tests/test_undoredomanager.cpp $<TARGET_OBJECTS:core_logic>)
target_include_directories(test-undoredomanager PRIVATE ${COMMON_INCLUDE_DIR})
add_test(NAME TestUndoRedoManager COMMAND test-undoredomanager)

add_executable(test-gamesaver tests/test_gamesaver.cpp $<TARGET_OBJECTS:core_logic>)
target_include_directories(test-gamesaver PRIVATE ${COMMON_INCLUDE_DIR})
add_test(NAME TestGameSaver COMMAND test-gamesaver)

add_executable(test-textgamesaver tests/test_textgamesaver.cpp $<TARGET_OBJECTS:core_logic>)
target_include_directories(test-textgamesaver PRIVATE ${COMMON_INCLUDE_DIR})
add_test(NAME TestTextGameSaver COMMAND test-textgamesaver)

add_executable(test-tuirenderer tests/test_tuirenderer.cpp $<TARGET_OBJECTS:core_logic>)
target_include_directories(test-tuirenderer PRIVATE ${COMMON_INCLUDE_DIR})
target_link_libraries(test-tuirenderer PRIVATE tui_lib)
add_test(NAME TestTuiRenderer COMMAND test-tuirenderer)

add_executable(test-ui-components tests/test_ui_components.cpp $<TARGET_OBJECTS:core_logic>)
target_include_directories(test-ui-components PRIVATE ${COMMON_INCLUDE_DIR})
target_link_libraries(test-ui-components PRIVATE tui_lib)
add_test(NAME TestUIComponents COMMAND test-ui-components)

add_executable(test-tile-renderer tests/test_tile_renderer.cpp $<TARGET_OBJECTS:core_logic>)
target_include_directories(test-tile-renderer PRIVATE ${COMMON_INCLUDE_DIR})
target_link_libraries(test-tile-renderer
  PRIVATE tui_lib
  PRIVATE gtest_main
  PRIVATE gmock
)
add_test(NAME TestTileRenderer COMMAND test-tile-renderer)

add_executable(test-grid-renderer tests/test_grid_renderer.cpp $<TARGET_OBJECTS:core_logic>)
target_include_directories(test-grid-renderer PRIVATE ${COMMON_INCLUDE_DIR})
target_link_libraries(test-grid-renderer
  PRIVATE tui_lib
  PRIVATE gtest_main
  PRIVATE gmock
)
add_test(NAME TestGridRenderer COMMAND test-grid-renderer)

add_executable(test-game-layout-composer tests/test_game_layout_composer.cpp $<TARGET_OBJECTS:core_logic>)
target_include_directories(test-game-layout-composer PRIVATE ${COMMON_INCLUDE_DIR})
target_link_libraries(test-game-layout-composer
  PRIVATE tui_lib
  PRIVATE gtest_main
  PRIVATE gmock
)
add_test(NAME TestGameLayoutComposer COMMAND test-game-layout-composer)

# --- Debug targets -----------------------------------------------------------
add_executable(debug-moveadvisor tests/debug_moveadvisor.cpp)
target_include_directories(debug-moveadvisor PRIVATE ${COMMON_INCLUDE_DIR})

add_executable(debug-moveadvisor-verbose tests/debug_moveadvisor_verbose.cpp)
target_include_directories(debug-moveadvisor-verbose PRIVATE ${COMMON_INCLUDE_DIR})

add_executable(find-two-move-board tests/find_two_move_board.cpp)
target_include_directories(find-two-move-board PRIVATE ${COMMON_INCLUDE_DIR})

if (EMSCRIPTEN)
  string(APPEND CMAKE_CXX_FLAGS " -s USE_PTHREADS")
  string(APPEND CMAKE_EXE_LINKER_FLAGS " -s ASYNCIFY")
  string(APPEND CMAKE_EXE_LINKER_FLAGS " -s PROXY_TO_PTHREAD")

  foreach(file "index.html" "run_webassembly.py")
    configure_file("src/${file}" ${file})
  endforeach(file)
endif()