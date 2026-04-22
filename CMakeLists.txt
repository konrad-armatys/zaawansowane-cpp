cmake_minimum_required (VERSION 3.22)

project(ftxui-starter
  LANGUAGES CXX
  VERSION 1.0.0
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
FetchContent_MakeAvailable(ftxui)
# ------------------------------------------------------------------------------

add_executable(ftxui-starter
  src/main.cpp
  src/ManhattanDistance.cpp
  src/HammingDistance.cpp
  src/LookAhead.cpp
  src/UndoRedo.cpp
  src/MoveAdvisor.cpp
  src/BinaryGameSaver.cpp
  src/TextGameSaver.cpp
  src/UndoRedoManager.cpp
  src/PuzzlePresenter.cpp
  src/TuiRenderer.cpp
)
target_include_directories(ftxui-starter PRIVATE include)

target_link_libraries(ftxui-starter
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)

add_executable(test-stats tests/test_stats.cpp)
target_include_directories(test-stats PRIVATE include)

add_executable(test-engine tests/test_engine.cpp)
target_include_directories(test-engine PRIVATE include)

add_executable(test-heuristics
  tests/test_heuristics.cpp
  src/ManhattanDistance.cpp
  src/HammingDistance.cpp
  src/LookAhead.cpp
  src/UndoRedo.cpp
  src/MoveAdvisor.cpp
)
target_include_directories(test-heuristics PRIVATE include)

add_executable(test-moveadvisor
  tests/test_moveadvisor.cpp
  src/ManhattanDistance.cpp
  src/HammingDistance.cpp
  src/MoveAdvisor.cpp
)
target_include_directories(test-moveadvisor PRIVATE include)

add_executable(test-undoredomanager
  tests/test_undoredomanager.cpp
  src/UndoRedoManager.cpp
)
target_include_directories(test-undoredomanager PRIVATE include)

add_executable(debug-moveadvisor
  tests/debug_moveadvisor.cpp
  src/ManhattanDistance.cpp
  src/MoveAdvisor.cpp
)
target_include_directories(debug-moveadvisor PRIVATE include)

add_executable(debug-moveadvisor-verbose
  tests/debug_moveadvisor_verbose.cpp
  src/ManhattanDistance.cpp
)
target_include_directories(debug-moveadvisor-verbose PRIVATE include)

add_executable(find-two-move-board
  tests/find_two_move_board.cpp
  src/ManhattanDistance.cpp
)
target_include_directories(find-two-move-board PRIVATE include)

add_executable(example-heuristics
  examples/example_heuristics.cpp
  src/ManhattanDistance.cpp
  src/HammingDistance.cpp
  src/LookAhead.cpp
  src/UndoRedo.cpp
  src/MoveAdvisor.cpp
)
target_include_directories(example-heuristics PRIVATE include)

add_executable(example-moveadvisor
  examples/example_moveadvisor.cpp
  src/ManhattanDistance.cpp
  src/HammingDistance.cpp
  src/MoveAdvisor.cpp
)
target_include_directories(example-moveadvisor PRIVATE include)

add_executable(example-undoredomanager
  examples/example_undoredomanager.cpp
  src/UndoRedoManager.cpp
)
target_include_directories(example-undoredomanager PRIVATE include)

add_executable(test-gamesaver
  tests/test_gamesaver.cpp
  src/BinaryGameSaver.cpp
  src/TextGameSaver.cpp
  src/UndoRedoManager.cpp
)
target_include_directories(test-gamesaver PRIVATE include)

add_executable(example-gamesaver
  examples/example_gamesaver.cpp
  src/BinaryGameSaver.cpp
  src/TextGameSaver.cpp
  src/UndoRedoManager.cpp
)
target_include_directories(example-gamesaver PRIVATE include)

add_executable(example-textgamesaver
  examples/example_textgamesaver.cpp
  src/BinaryGameSaver.cpp
  src/TextGameSaver.cpp
  src/UndoRedoManager.cpp
)
target_include_directories(example-textgamesaver PRIVATE include)

add_executable(test-textgamesaver
  tests/test_textgamesaver.cpp
  src/TextGameSaver.cpp
  src/UndoRedoManager.cpp
)
target_include_directories(test-textgamesaver PRIVATE include)

add_executable(test-tuirenderer
  tests/test_tuirenderer.cpp
  src/TuiRenderer.cpp
  src/PuzzlePresenter.cpp
  src/ManhattanDistance.cpp
  src/HammingDistance.cpp
  src/LookAhead.cpp
  src/UndoRedo.cpp
  src/MoveAdvisor.cpp
  src/BinaryGameSaver.cpp
  src/TextGameSaver.cpp
  src/UndoRedoManager.cpp
)
target_include_directories(test-tuirenderer PRIVATE include)
target_link_libraries(test-tuirenderer
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)

add_executable(example-puzzlepresenter
  examples/example_puzzlepresenter.cpp
  src/PuzzlePresenter.cpp
  src/BinaryGameSaver.cpp
  src/TextGameSaver.cpp
  src/UndoRedoManager.cpp
  src/ManhattanDistance.cpp
  src/HammingDistance.cpp
  src/LookAhead.cpp
  src/UndoRedo.cpp
  src/MoveAdvisor.cpp
)
target_include_directories(example-puzzlepresenter PRIVATE include)

add_executable(example-tuirenderer
  examples/example_tuirenderer.cpp
  src/TuiRenderer.cpp
  src/PuzzlePresenter.cpp
  src/ManhattanDistance.cpp
  src/HammingDistance.cpp
  src/LookAhead.cpp
  src/UndoRedo.cpp
  src/MoveAdvisor.cpp
  src/BinaryGameSaver.cpp
  src/TextGameSaver.cpp
  src/UndoRedoManager.cpp
)
target_include_directories(example-tuirenderer PRIVATE include)
target_link_libraries(example-tuirenderer
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)

add_executable(example-interactive-tui
  examples/example_interactive_tui.cpp
  src/TuiRenderer.cpp
  src/PuzzlePresenter.cpp
  src/ManhattanDistance.cpp
  src/HammingDistance.cpp
  src/LookAhead.cpp
  src/UndoRedo.cpp
  src/MoveAdvisor.cpp
  src/BinaryGameSaver.cpp
  src/TextGameSaver.cpp
  src/UndoRedoManager.cpp
)
target_include_directories(example-interactive-tui PRIVATE include)
target_link_libraries(example-interactive-tui
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)

if (EMSCRIPTEN)
  string(APPEND CMAKE_CXX_FLAGS " -s USE_PTHREADS")
  string(APPEND CMAKE_EXE_LINKER_FLAGS " -s ASYNCIFY")
  string(APPEND CMAKE_EXE_LINKER_FLAGS " -s PROXY_TO_PTHREAD")

  foreach(file "index.html" "run_webassembly.py")
    configure_file("src/${file}" ${file})
  endforeach(file)
endif()