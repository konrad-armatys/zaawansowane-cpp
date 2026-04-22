#include <gtest/gtest.h>
#include "ui/GameLayoutComposer.h"
#include "ui/TileRenderer.h"
#include "ui/GridRenderer.h"
#include "ui/StatsPanelRenderer.h"
#include "ui/MessagePanelRenderer.h"
#include "ui/ControlsPanelRenderer.h"
#include "PuzzlePresenter.h"
#include "Board.h"
#include "GameStats.h"
#include <memory>

class GameLayoutComposerTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto tileRenderer = std::make_shared<TileRenderer>();
        auto gridRenderer = std::make_shared<GridRenderer>(tileRenderer);
        auto statsRenderer = std::make_shared<StatsPanelRenderer>();
        auto messageRenderer = std::make_shared<MessagePanelRenderer>();
        auto controlsRenderer = std::make_shared<ControlsPanelRenderer>();

        composer = std::make_unique<GameLayoutComposer>(
            gridRenderer, statsRenderer, messageRenderer, controlsRenderer
        );

        board = std::make_unique<Board<int>>(3);
        stats = std::make_unique<GameStats>(3);
        int value = 1;
        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {
                if (y == 2 && x == 2) {
                    board->at(x, y) = 0;
                } else {
                    board->at(x, y) = value++;
                }
            }
        }
    }

    std::unique_ptr<GameLayoutComposer> composer;
    std::unique_ptr<Board<int>> board;
    std::unique_ptr<GameStats> stats;

    PuzzleViewState makeState(const std::string& message = "",
                               InputMode mode = InputMode::None,
                               const std::string& buffer = "") {
        return PuzzleViewState{
            *board,
            *stats,
            message,
            mode,
            buffer,
            0.0,
            std::nullopt
        };
    }
};

TEST_F(GameLayoutComposerTest, TestBasicComposition) {
    auto state = makeState();
    EXPECT_NO_THROW({ composer->compose(state); });
}

TEST_F(GameLayoutComposerTest, TestWithMessage) {
    auto state = makeState("Ruch wykonany!");
    EXPECT_NO_THROW({ composer->compose(state); });
}

TEST_F(GameLayoutComposerTest, TestWithInput) {
    auto state = makeState("", InputMode::Save, "save.dat");
    EXPECT_NO_THROW({ composer->compose(state); });
}

TEST_F(GameLayoutComposerTest, TestEmptyMessage) {
    auto state = makeState("");
    EXPECT_NO_THROW({ composer->compose(state); });
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
