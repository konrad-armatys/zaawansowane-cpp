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

/**
 * @brief Główna klasa kontrolera TUI łącząca Model z Widokiem
 */
class InteractivePuzzleController {
private:
    PuzzlePresenter presenter_;
    TuiRenderer renderer_;
    std::shared_ptr<MoveAdvisor> advisor_;
    ftxui::ScreenInteractive screen_;
    bool gameRunning_;
    std::string statusMessage_;

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
     * @brief Obsługuje zapis gry
     */
    void handleSave() {
        try {
            presenter_.saveGame("puzzle_save.txt");
            statusMessage_ = "Gra zapisana do puzzle_save.txt";
        } catch (const std::exception& e) {
            statusMessage_ = std::string("Błąd zapisu: ") + e.what();
        }
        refreshView();
    }

    /**
     * @brief Obsługuje wczytanie gry
     */
    void handleLoad() {
        try {
            presenter_.loadGame("puzzle_save.txt");
            statusMessage_ = "Gra wczytana z puzzle_save.txt";
        } catch (const std::exception& e) {
            statusMessage_ = std::string("Błąd wczytania: ") + e.what();
        }
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

        auto gridElement = ftxui::vbox(std::move(rows)) | ftxui::border | ftxui::borderStyled(ftxui::ROUNDED);


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

        auto statsElement = ftxui::vbox(std::move(statsElements)) | ftxui::border | ftxui::borderStyled(ftxui::ROUNDED) | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 30);


        ftxui::Elements mainContent;
        mainContent.push_back(ftxui::text("N-PUZZLE GAME") | ftxui::bold | ftxui::color(ftxui::Color::Cyan) | ftxui::center);
        mainContent.push_back(ftxui::separator());

        ftxui::Elements gameArea;
        gameArea.push_back(gridElement | ftxui::flex);
        gameArea.push_back(ftxui::separator());
        gameArea.push_back(statsElement);

        mainContent.push_back(ftxui::hbox(std::move(gameArea)));

        if (!statusMessage_.empty()) {
            mainContent.push_back(ftxui::separator());
            mainContent.push_back(
                ftxui::text(statusMessage_) |
                ftxui::border |
                ftxui::borderStyled(ftxui::ROUNDED) |
                ftxui::bgcolor(ftxui::Color::Blue) |
                ftxui::color(ftxui::Color::White) |
                ftxui::bold
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
                handleSave();
                return true;
            }
            if (event == ftxui::Event::Character('l') || event == ftxui::Event::Character('L')) {
                handleLoad();
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
     */
    explicit InteractivePuzzleController(int boardSize)
        : presenter_(boardSize, std::make_unique<TextGameSaver>()),
          renderer_(boardSize, std::make_unique<ManhattanDistance>()),
          advisor_(std::make_shared<MoveAdvisor>(std::make_shared<ManhattanDistance>())),
          screen_(ftxui::ScreenInteractive::Fullscreen()),
          gameRunning_(true),
          statusMessage_("Witaj w N-Puzzle! Użyj strzałek lub WSAD do gry")
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


        presenter_.shuffle();
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

        constexpr int BOARD_SIZE = 4;

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
        std::cout << "\nNaciśnij Enter aby rozpocząć...";
        std::cin.get();

        InteractivePuzzleController controller(BOARD_SIZE);
        controller.run();

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << std::endl;
        return 1;
    }
}
