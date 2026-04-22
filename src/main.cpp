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
        const auto& board = presenter_.getEngine().getBoard();
        const auto& stats = presenter_.getEngine().getStats();

        auto heuristic = std::make_shared<ManhattanDistance>();
        double heuristicValue = heuristic->calculate(board);

        renderer_.render(board);
        renderer_.updateStats(stats, heuristicValue);
        renderer_.showMessage(statusMessage_);
        renderer_.refresh();

        screen_.PostEvent(ftxui::Event::Custom);
    }

    /**
     * @brief Obsługuje ruch gracza w danym kierunku
     * @param dir Kierunek ruchu
     */
    void handleMove(Direction dir) {
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
        bool success = presenter_.undo();
        statusMessage_ = success ? "Cofnięto ruch" : "Brak ruchów do cofnięcia";
        refreshView();
    }

    /**
     * @brief Obsługuje ponowienie ruchu (Redo)
     */
    void handleRedo() {
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
            switch (*suggestion) {
                case Direction::Up: dirName = "Góra"; break;
                case Direction::Down: dirName = "Dół"; break;
                case Direction::Left: dirName = "Lewo"; break;
                case Direction::Right: dirName = "Prawo"; break;
            }
            statusMessage_ = std::string("Podpowiedź: ") + dirName;
        } else {
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
        presenter_.shuffle();
        statusMessage_ = "Plansza przetasowana";
        refreshView();
    }

    /**
     * @brief Obsługuje reset gry
     */
    void handleReset() {
        presenter_.reset();
        statusMessage_ = "Gra zresetowana";
        refreshView();
    }

    /**
     * @brief Tworzy komponent FTXUI obsługujący zdarzenia klawiatury
     * @return Komponent FTXUI
     */
    ftxui::Component createComponent() {
        auto component = ftxui::Renderer([this] {
            return ftxui::text("N-Puzzle Game");
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

        refreshView();


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
