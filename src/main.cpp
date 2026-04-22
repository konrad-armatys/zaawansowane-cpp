#include "PuzzlePresenter.h"
#include "TuiRenderer.h"
#include "ManhattanDistance.h"
#include "MoveAdvisor.h"
#include "TextGameSaver.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/event.hpp>
#include <memory>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>

/**
 * @brief Główna klasa kontrolera TUI łącząca Model z Widokiem
 */
enum class InputMode {
    None,
    Save,
    Load
};

class InteractivePuzzleController {
private:
    PuzzlePresenter presenter_;
    TuiRenderer renderer_;
    std::shared_ptr<MoveAdvisor> advisor_;
    ftxui::ScreenInteractive screen_;
    bool gameRunning_;
    std::string statusMessage_;
    std::string savePath_;
    InputMode inputMode_;
    std::string inputBuffer_;

    /**
     * @brief Aktualizuje widok po zmianie stanu gry
     */
    void refreshView() {
        screen_.PostEvent(ftxui::Event::Custom);
    }

    /**
     * @brief Obsługuje ruch gracza w danym kierunku
     * @param dir Kierunek ruchu
     */
    void handleMove(Direction dir) {

        renderer_.setHintHighlight(std::nullopt);

        bool moved = presenter_.move(dir);
        if (moved) {
            statusMessage_ = "Ruch wykonany";
            if (presenter_.getEngine().isSolved()) {
                statusMessage_ = "Gratulacje! Układanka rozwiązana!";
            }
        } else {
            statusMessage_ = "Nieprawidłowy ruch";
        }
        refreshView();
    }

    /**
     * @brief Obsługuje cofnięcie ruchu (Undo)
     */
    void handleUndo() {
        renderer_.setHintHighlight(std::nullopt);
        bool success = presenter_.undo();
        statusMessage_ = success ? "Cofnięto ruch" : "Brak ruchów do cofnięcia";
        refreshView();
    }

    /**
     * @brief Obsługuje ponowienie ruchu (Redo)
     */
    void handleRedo() {
        renderer_.setHintHighlight(std::nullopt);
        bool success = presenter_.redo();
        statusMessage_ = success ? "Ponowiono ruch" : "Brak ruchów do ponowienia";
        refreshView();
    }

    /**
     * @brief Obsługuje żądanie podpowiedzi
     */
    void handleHint() {
        auto [emptyX, emptyY] = presenter_.getEngine().getEmptyPosition();
        auto suggestion = advisor_->suggestMove(
            presenter_.getEngine().getBoard(), emptyX, emptyY);

        if (suggestion.has_value()) {
            const char* dirName = "";
            int highlightX = emptyX;
            int highlightY = emptyY;


            switch (*suggestion) {
                case Direction::Up:
                    dirName = "Góra";
                    highlightY = emptyY - 1;
                    break;
                case Direction::Down:
                    dirName = "Dół";
                    highlightY = emptyY + 1;
                    break;
                case Direction::Left:
                    dirName = "Lewo";
                    highlightX = emptyX - 1;
                    break;
                case Direction::Right:
                    dirName = "Prawo";
                    highlightX = emptyX + 1;
                    break;
            }


            renderer_.setHintHighlight(std::make_pair(highlightX, highlightY));
            statusMessage_ = std::string("Podpowiedź: ") + dirName + " ⭐";
        } else {
            renderer_.setHintHighlight(std::nullopt);
            statusMessage_ = "Brak dostępnych podpowiedzi";
        }
        refreshView();
    }

    /**
     * @brief Normalizuje ścieżkę - rozszerza ~ na home directory i obsługuje ścieżki absolutne
     * @param path Ścieżka do normalizacji
     * @return Znormalizowana ścieżka
     */
    static std::string normalizePath(const std::string& path) {
        std::string result = path;


        if (!result.empty() && result[0] == '~') {
            const char* home = std::getenv("HOME");
            if (home) {
                result = std::string(home) + result.substr(1);
            }
        }

        try {

            if (std::filesystem::path(result).is_absolute()) {
                return result;
            }
        } catch (...) {

        }


        std::ifstream test(result);
        if (test.good()) {
            test.close();
            return result;
        }


        std::string parentPath = "../" + result;
        std::ifstream parentTest(parentPath);
        if (parentTest.good()) {
            parentTest.close();
            return parentPath;
        }



        return result;
    }

    /**
     * @brief Sprawdza czy plik istnieje w dowolnej z katalogu bieżącego lub nadrzędnego
     * @param filename Nazwa pliku do sprawdzenia
     * @return Ścieżka do pliku jeśli istnieje, pusty string w przeciwnym wypadku
     */
    static std::string findSaveFile(const std::string& filename) {
        std::ifstream file(filename);
        if (file.good()) {
            file.close();
            return filename;
        }

        std::string parentPath = "../" + filename;
        std::ifstream parentFile(parentPath);
        if (parentFile.good()) {
            parentFile.close();
            return parentPath;
        }

        return "";
    }

    /**
     * @brief Rozpoczyna tryb inputu do wpisania ścieżki zapisu
     */
    void beginSaveMode() {
        inputMode_ = InputMode::Save;
        inputBuffer_ = savePath_;
        statusMessage_ = "Podaj ścieżkę do pliku zapisu (ESC - anuluj, ENTER - zapisz):";
        refreshView();
    }

    /**
     * @brief Rozpoczyna tryb inputu do wpisania ścieżki wczytania
     */
    void beginLoadMode() {
        inputMode_ = InputMode::Load;
        inputBuffer_ = savePath_;
        statusMessage_ = "Podaj ścieżkę do pliku wczytania (ESC - anuluj, ENTER - wczytaj):";
        refreshView();
    }

    /**
     * @brief Kończy tryb inputu i wykonuje akcję zapisu/wczytania
     */
    void confirmInputMode() {
        if (inputBuffer_.empty()) {
            statusMessage_ = "Błąd: ścieżka nie może być pusta";
            inputMode_ = InputMode::None;
            refreshView();
            return;
        }


        std::string resolvedPath = normalizePath(inputBuffer_);

        try {
            if (inputMode_ == InputMode::Save) {

                try {
                    auto parentPath = std::filesystem::path(resolvedPath).parent_path();
                    if (!parentPath.empty() && !std::filesystem::exists(parentPath)) {
                        std::filesystem::create_directories(parentPath);
                    }
                } catch (...) {

                }

                presenter_.saveGame(resolvedPath);
                savePath_ = resolvedPath;
                statusMessage_ = "Gra zapisana do " + resolvedPath;
            } else if (inputMode_ == InputMode::Load) {
                presenter_.loadGame(resolvedPath);
                savePath_ = resolvedPath;
                statusMessage_ = "Gra wczytana z " + resolvedPath;
            }
        } catch (const std::exception& e) {
            statusMessage_ = std::string("Błąd: ") + e.what();
        }

        inputMode_ = InputMode::None;
        refreshView();
    }

    /**
     * @brief Anuluje tryb inputu
     */
    void cancelInputMode() {
        inputMode_ = InputMode::None;
        inputBuffer_.clear();
        statusMessage_ = "Anulowano";
        refreshView();
    }

    /**
     * @brief Obsługuje tasowanie planszy
     */
    void handleShuffle() {
        renderer_.setHintHighlight(std::nullopt);
        presenter_.shuffle();
        statusMessage_ = "Plansza przetasowana";
        refreshView();
    }

    /**
     * @brief Obsługuje reset gry
     */
    void handleReset() {
        renderer_.setHintHighlight(std::nullopt);
        presenter_.reset();
        statusMessage_ = "Gra zresetowana";
        refreshView();
    }

    /**
     * @brief Tworzy element UI z aktualnym stanem gry
     * @return Element FTXUI do wyświetlenia
     */
    ftxui::Element createGameElement() {
        const auto& board = presenter_.getEngine().getBoard();
        const auto& stats = presenter_.getEngine().getStats();
        auto heuristic = std::make_shared<ManhattanDistance>();
        double heuristicValue = heuristic->calculate(board);


        renderer_.updateStats(stats, heuristicValue);
        renderer_.showMessage(statusMessage_);


        ftxui::Elements rows;
        const int size = board.getSize();

        for (int y = 0; y < size; ++y) {
            ftxui::Elements row;
            for (int x = 0; x < size; ++x) {
                int value = board.at(x, y);
                ftxui::Element tile;

                if (value == 0) {
                    tile = ftxui::text("   ") | ftxui::border | ftxui::bgcolor(ftxui::Color::Black);
                } else {
                    std::ostringstream oss;
                    oss << std::setw(3) << value;

                    tile = ftxui::text(oss.str()) | ftxui::border;


                    auto hintPos = renderer_.getHintPosition();
                    if (hintPos.has_value() && hintPos->first == x && hintPos->second == y) {
                        tile = tile | ftxui::bgcolor(ftxui::Color::Yellow) | ftxui::color(ftxui::Color::Black) | ftxui::bold;
                    } else if (renderer_.isTileCorrect(value, x, y, size)) {
                        tile = tile | ftxui::bgcolor(ftxui::Color::Green) | ftxui::color(ftxui::Color::Black);
                    } else {
                        tile = tile | ftxui::bgcolor(ftxui::Color::Blue) | ftxui::color(ftxui::Color::White);
                    }
                }

                row.push_back(tile);
            }
            rows.push_back(ftxui::hbox(std::move(row)));
        }

        auto gridElement = ftxui::vbox(std::move(rows)) | ftxui::borderStyled(ftxui::ROUNDED);


        ftxui::Elements statsElements;
        statsElements.push_back(ftxui::text("=== STATYSTYKI ===") | ftxui::bold | ftxui::color(ftxui::Color::Cyan));
        statsElements.push_back(ftxui::separator());
        statsElements.push_back(ftxui::hbox({
            ftxui::text("Ruchy: ") | ftxui::bold,
            ftxui::text(std::to_string(stats.movesCount.get())) | ftxui::color(ftxui::Color::Yellow)
        }));
        statsElements.push_back(ftxui::hbox({
            ftxui::text("Cofnięcia: ") | ftxui::bold,
            ftxui::text(std::to_string(stats.undoCount.get())) | ftxui::color(ftxui::Color::Yellow)
        }));
        statsElements.push_back(ftxui::hbox({
            ftxui::text("Poprawne kafelki: ") | ftxui::bold,
            ftxui::text(std::to_string(stats.correctTiles.get()) + " / " + std::to_string(size * size)) | ftxui::color(ftxui::Color::Green)
        }));

        std::ostringstream heurOss;
        heurOss << std::fixed << std::setprecision(2) << heuristicValue;
        statsElements.push_back(ftxui::hbox({
            ftxui::text("Heurystyka: ") | ftxui::bold,
            ftxui::text(heurOss.str()) | ftxui::color(ftxui::Color::Magenta)
        }));

        auto statsElement = ftxui::vbox(std::move(statsElements)) | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 30);


        ftxui::Elements mainContent;
        mainContent.push_back(ftxui::text("N-PUZZLE GAME") | ftxui::bold | ftxui::color(ftxui::Color::Cyan) | ftxui::center);
        mainContent.push_back(ftxui::separator());

        ftxui::Elements gameArea;
        gameArea.push_back(gridElement | ftxui::flex);
        gameArea.push_back(ftxui::separator());
        gameArea.push_back(statsElement);

        mainContent.push_back(ftxui::hbox(std::move(gameArea)));

        if (!statusMessage_.empty()) {
            mainContent.push_back(
                ftxui::text(statusMessage_) |
                ftxui::bgcolor(ftxui::Color::Blue) |
                ftxui::color(ftxui::Color::White) |
                ftxui::bold
            );
        }


        if (inputMode_ != InputMode::None) {
            mainContent.push_back(ftxui::separator());
            std::string modeLabel = (inputMode_ == InputMode::Save) ? "Zapisz:" : "Wczytaj:";
            mainContent.push_back(
                ftxui::hbox({
                    ftxui::text(modeLabel) | ftxui::bold,
                    ftxui::text(" [") | ftxui::dim,
                    ftxui::text(inputBuffer_) | ftxui::bgcolor(ftxui::Color::White) | ftxui::color(ftxui::Color::Black),
                    ftxui::text("]") | ftxui::dim
                })
            );
        }

        mainContent.push_back(ftxui::separator());
        mainContent.push_back(
            ftxui::text("Sterowanie: ↑↓←→/WSAD - ruch | U - undo | R - redo | H - hint | V - save | L - load | N - new | T - reset | Q - quit") |
            ftxui::dim | ftxui::center
        );

        return ftxui::vbox(std::move(mainContent));
    }

    /**
     * @brief Tworzy komponent FTXUI obsługujący zdarzenia klawiatury
     * @return Komponent FTXUI
     */
    ftxui::Component createComponent() {
        auto component = ftxui::Renderer([this] {
            return createGameElement();
        });

        component = ftxui::CatchEvent(component, [this](ftxui::Event event) {

            if (inputMode_ != InputMode::None) {
                if (event == ftxui::Event::Escape) {
                    cancelInputMode();
                    return true;
                }
                if (event == ftxui::Event::Return) {
                    confirmInputMode();
                    return true;
                }
                if (event == ftxui::Event::Backspace) {
                    if (!inputBuffer_.empty()) {
                        inputBuffer_.pop_back();
                        refreshView();
                    }
                    return true;
                }
                if (event.is_character()) {
                    inputBuffer_ += event.character()[0];
                    refreshView();
                    return true;
                }
                return false;
            }


            if (event == ftxui::Event::Character('q') || event == ftxui::Event::Character('Q')) {
                gameRunning_ = false;
                screen_.ExitLoopClosure()();
                return true;
            }


            if (event == ftxui::Event::ArrowUp) {
                handleMove(Direction::Up);
                return true;
            }
            if (event == ftxui::Event::ArrowDown) {
                handleMove(Direction::Down);
                return true;
            }
            if (event == ftxui::Event::ArrowLeft) {
                handleMove(Direction::Left);
                return true;
            }
            if (event == ftxui::Event::ArrowRight) {
                handleMove(Direction::Right);
                return true;
            }


            if (event == ftxui::Event::Character('w') || event == ftxui::Event::Character('W')) {
                handleMove(Direction::Up);
                return true;
            }
            if (event == ftxui::Event::Character('s') || event == ftxui::Event::Character('S')) {
                handleMove(Direction::Down);
                return true;
            }
            if (event == ftxui::Event::Character('a') || event == ftxui::Event::Character('A')) {
                handleMove(Direction::Left);
                return true;
            }
            if (event == ftxui::Event::Character('d') || event == ftxui::Event::Character('D')) {
                handleMove(Direction::Right);
                return true;
            }


            if (event == ftxui::Event::Character('u') || event == ftxui::Event::Character('U')) {
                handleUndo();
                return true;
            }
            if (event == ftxui::Event::Character('r') || event == ftxui::Event::Character('R')) {
                handleRedo();
                return true;
            }


            if (event == ftxui::Event::Character('h') || event == ftxui::Event::Character('H')) {
                handleHint();
                return true;
            }


            if (event == ftxui::Event::Character('v') || event == ftxui::Event::Character('V')) {
                beginSaveMode();
                return true;
            }
            if (event == ftxui::Event::Character('l') || event == ftxui::Event::Character('L')) {
                beginLoadMode();
                return true;
            }


            if (event == ftxui::Event::Character('n') || event == ftxui::Event::Character('N')) {
                handleShuffle();
                return true;
            }
            if (event == ftxui::Event::Character('t') || event == ftxui::Event::Character('T')) {
                handleReset();
                return true;
            }

            return false;
        });

        return component;
    }

public:
    /**
     * @brief Konstruktor kontrolera
     * @param boardSize Rozmiar planszy (N x N)
     * @param autoLoadSave Jeśli true i plik puzzle_save.txt istnieje, wczytaj grę zamiast tworzyć nową
     * @param savePath Ścieżka do pliku zapisu
     */
    explicit InteractivePuzzleController(int boardSize, bool autoLoadSave = false, const std::string& savePath = "puzzle_save.txt")
        : presenter_(boardSize, std::make_unique<TextGameSaver>()),
          renderer_(boardSize, std::make_unique<ManhattanDistance>()),
          advisor_(std::make_shared<MoveAdvisor>(std::make_shared<ManhattanDistance>())),
          screen_(ftxui::ScreenInteractive::Fullscreen()),
          gameRunning_(true),
          statusMessage_("Witaj w N-Puzzle! Użyj strzałek lub WSAD do gry"),
          savePath_(savePath),
          inputMode_(InputMode::None),
          inputBuffer_("")
    {

        auto& stats = presenter_.getEngine().getStats();
        stats.movesCount.setOnChange([this](int) {
            screen_.PostEvent(ftxui::Event::Custom);
        });
        stats.undoCount.setOnChange([this](int) {
            screen_.PostEvent(ftxui::Event::Custom);
        });
        stats.correctTiles.setOnChange([this](int) {
            screen_.PostEvent(ftxui::Event::Custom);
        });


        if (autoLoadSave) {
            try {
                presenter_.loadGame(savePath_);
                statusMessage_ = "Gra wczytana z " + savePath_;
            } catch (...) {

                presenter_.shuffle();
            }
        } else {

            presenter_.shuffle();
        }
    }

    /**
     * @brief Uruchamia główną pętlę gry
     */
    void run() {
        auto component = createComponent();
        screen_.Loop(component);


        std::cout << "\nDziękujemy za grę!\n";
        std::cout << "Wykonanych ruchów: " << presenter_.getEngine().getStats().movesCount << "\n";
    }
};

/**
 * @brief Funkcja główna programu
 * @return Kod wyjścia (0 = sukces)
 */
int main() {
    try {
        std::cout << "=== N-Puzzle Game ===" << std::endl;
        std::cout << "Sterowanie:" << std::endl;
        std::cout << "  Strzałki / WSAD - Ruch" << std::endl;
        std::cout << "  U - Undo" << std::endl;
        std::cout << "  R - Redo" << std::endl;
        std::cout << "  H - Podpowiedź (Hint)" << std::endl;
        std::cout << "  V - Zapisz (saVe)" << std::endl;
        std::cout << "  L - Wczytaj (Load)" << std::endl;
        std::cout << "  N - Nowa gra (shuffle)" << std::endl;
        std::cout << "  T - Reset (do rozwiązanej)" << std::endl;
        std::cout << "  Q - Wyjście" << std::endl;

        int boardSize = 4;
        bool autoLoadGame = false;


        std::string savePath;
        std::ifstream saveFile("puzzle_save.txt");
        if (!saveFile.good()) {
            saveFile.close();
            std::ifstream parentSaveFile("../puzzle_save.txt");
            if (parentSaveFile.good()) {
                savePath = "../puzzle_save.txt";
                parentSaveFile.close();
            } else {
                parentSaveFile.close();
            }
        } else {
            savePath = "puzzle_save.txt";
            saveFile.close();
        }

        if (!savePath.empty()) {
            std::ifstream file(savePath);
            if (file.good()) {
                std::string firstLine;
                if (std::getline(file, firstLine)) {
                    try {
                        boardSize = std::stoi(firstLine);
                        autoLoadGame = true;
                        std::cout << "\nZnaleziono plik zapisu (" << savePath << ") o rozmiarze planszy " << boardSize << "x" << boardSize << ".\n";
                    } catch (...) {
                        std::cout << "\nNie udało się odczytać rozmiaru z pliku zapisu. Używanie domyślnego: 4\n";
                    }
                }
                file.close();
            }
        } else {

            std::cout << "\nPodaj rozmiar planszy (domyślnie 4, zakres 2-8): ";
            std::string input;
            std::getline(std::cin, input);

            if (!input.empty()) {
                try {
                    int parsed = std::stoi(input);
                    if (parsed >= 2 && parsed <= 100) {
                        boardSize = parsed;
                    } else {
                        std::cout << "Rozmiar poza zakresem. Używanie domyślnego: 4\n";
                    }
                } catch (const std::exception&) {
                    std::cout << "Nieprawidłowe wejście. Używanie domyślnego: 4\n";
                }
            }
        }

        std::cout << "Uruchamianie gry z planszą " << boardSize << "x" << boardSize << "...\n";


        std::string controllerSavePath = savePath.empty() ? "puzzle_save.txt" : savePath;
        InteractivePuzzleController controller(boardSize, autoLoadGame, controllerSavePath);
        controller.run();

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << std::endl;
        return 1;
    }
}
