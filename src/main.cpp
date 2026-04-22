#include "PuzzlePresenter.h"
#include "TuiRenderer.h"
#include "ManhattanDistance.h"
#include "MoveAdvisor.h"
#include "TextGameSaver.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/event.hpp>
#include <iostream>
#include <memory>
#include <string>

/**
 * @brief Cienka powłoka kontrolera MVP - mapuje zdarzenia klawiatury na akcje prezentera
 */
class InteractivePuzzleController {
private:
    PuzzlePresenter presenter_;
    TuiRenderer renderer_;
    std::shared_ptr<MoveAdvisor<int>> advisor_;
    ftxui::ScreenInteractive screen_;
    bool gameRunning_;

    /**
     * @brief Tworzy komponent FTXUI z mapowaniem zdarzeń klawiatury na akcje prezentera
     * @return Komponent FTXUI gotowy do osadzenia w głównej pętli
     */
    ftxui::Component createComponent() {
        auto component = ftxui::Renderer([this] {
            return renderer_.createGameElement(presenter_);
        });

        return ftxui::CatchEvent(component, [this](ftxui::Event event) {
            if (presenter_.isInInputMode()) {
                if (event == ftxui::Event::Escape)    { presenter_.cancelInputMode();  return true; }
                if (event == ftxui::Event::Return)    { presenter_.confirmInputMode(); return true; }
                if (event == ftxui::Event::Backspace) { presenter_.inputBackspace();   return true; }
                if (event.is_character())             { presenter_.handleCharacter(event.character()[0]); return true; }
                return false;
            }

            if (event == ftxui::Event::Character('q') || event == ftxui::Event::Character('Q')) {
                gameRunning_ = false;
                screen_.ExitLoopClosure()();
                return true;
            }

            if (event == ftxui::Event::ArrowUp    || event == ftxui::Event::Character('w') || event == ftxui::Event::Character('W')) { presenter_.move(Direction::Up);    return true; }
            if (event == ftxui::Event::ArrowDown  || event == ftxui::Event::Character('s') || event == ftxui::Event::Character('S')) { presenter_.move(Direction::Down);  return true; }
            if (event == ftxui::Event::ArrowLeft  || event == ftxui::Event::Character('a') || event == ftxui::Event::Character('A')) { presenter_.move(Direction::Left);  return true; }
            if (event == ftxui::Event::ArrowRight || event == ftxui::Event::Character('d') || event == ftxui::Event::Character('D')) { presenter_.move(Direction::Right); return true; }

            if (event == ftxui::Event::Character('u') || event == ftxui::Event::Character('U')) { presenter_.undo();           return true; }
            if (event == ftxui::Event::Character('r') || event == ftxui::Event::Character('R')) { presenter_.redo();           return true; }
            if (event == ftxui::Event::Character('h') || event == ftxui::Event::Character('H')) { presenter_.hint(*advisor_);  return true; }
            if (event == ftxui::Event::Character('v') || event == ftxui::Event::Character('V')) { presenter_.beginSaveMode();  return true; }
            if (event == ftxui::Event::Character('l') || event == ftxui::Event::Character('L')) { presenter_.beginLoadMode();  return true; }
            if (event == ftxui::Event::Character('n') || event == ftxui::Event::Character('N')) { presenter_.shuffle();        return true; }
            if (event == ftxui::Event::Character('t') || event == ftxui::Event::Character('T')) { presenter_.reset();          return true; }

            return false;
        });
    }

public:
    /**
     * @brief Konstruktor kontrolera
     * @param boardSize Rozmiar planszy (N x N)
     * @param autoLoadSave Jeśli true, spróbuj wczytać istniejący zapis z savePath
     * @param savePath Początkowa ścieżka do pliku zapisu
     */
    explicit InteractivePuzzleController(int boardSize,
                                         bool autoLoadSave = false,
                                         const std::string& savePath = "puzzle_save.txt")
        : presenter_(boardSize, std::make_unique<TextGameSaver>(), std::make_unique<ManhattanDistance<int>>(), savePath),
          renderer_(boardSize, std::make_unique<ManhattanDistance<int>>()),
          advisor_(std::make_shared<MoveAdvisor<int>>(std::make_shared<ManhattanDistance<int>>())),
          screen_(ftxui::ScreenInteractive::Fullscreen()),
          gameRunning_(true)
    {
        presenter_.setOnChange([this] { screen_.PostEvent(ftxui::Event::Custom); });
        presenter_.bootstrap(autoLoadSave);
    }

    /**
     * @brief Uruchamia główną pętlę gry
     */
    void run() {
        auto component = createComponent();
        screen_.Loop(component);
        std::cout << "\nDziękujemy za grę!\n"
                  << "Wykonanych ruchów: " << presenter_.getEngine().getStats().movesCount << "\n";
    }
};

/**
 * @brief Funkcja główna programu
 * @return Kod wyjścia (0 = sukces)
 */
int main() {
    try {
        std::cout << "=== N-Puzzle Game ===\n"
                  << "Sterowanie: Strzałki/WSAD - ruch | U - undo | R - redo | H - hint | V - save | L - load | N - new | Q - quit\n";

        auto detected = PuzzlePresenter::detectSaveFile("puzzle_save.txt");
        bool autoLoadGame = detected.has_value();
        std::string savePath = detected.value_or("puzzle_save.txt");
        int boardSize = 4;

        std::cout << "\nPodaj rozmiar planszy (domyślnie 4, zakres 2-16): ";
        std::string input;
        std::getline(std::cin, input);
        if (!input.empty()) {
            try {
                int parsed = std::stoi(input);
                if (parsed >= 2 && parsed <= 16) boardSize = parsed;
                else std::cout << "Rozmiar poza zakresem. Używanie domyślnego: 4\n";
            } catch (const std::exception&) {
                std::cout << "Nieprawidłowe wejście. Używanie domyślnego: 4\n";
            }
        }

        std::cout << "Uruchamianie gry z planszą " << boardSize << "x" << boardSize << "...\n";
        InteractivePuzzleController controller(boardSize, autoLoadGame, savePath);
        controller.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << std::endl;
        return 1;
    }
}
