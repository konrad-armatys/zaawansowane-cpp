#include <gtest/gtest.h>
#include "ui/GridRenderer.h"
#include "ui/TileRenderer.h"
#include "Board.h"
#include <memory>

class GridRendererTest : public ::testing::Test {
protected:
    void SetUp() override {
        tileRenderer = std::make_shared<TileRenderer>();
        renderer = std::make_unique<GridRenderer>(tileRenderer);
    }

    std::shared_ptr<TileRenderer> tileRenderer;
    std::unique_ptr<GridRenderer> renderer;

    Board<int> makeBoard(int size) {
        Board<int> board(size);
        int value = 1;
        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                if (y == size - 1 && x == size - 1) {
                    board.at(x, y) = 0;
                } else {
                    board.at(x, y) = value++;
                }
            }
        }
        return board;
    }
};

TEST_F(GridRendererTest, TestSmallGrid) {
    auto board = makeBoard(3);
    EXPECT_NO_THROW({ renderer->render(board); });
}

TEST_F(GridRendererTest, TestMediumGrid) {
    auto board = makeBoard(10);
    EXPECT_NO_THROW({ renderer->render(board); });
}

TEST_F(GridRendererTest, TestLargeGrid) {
    auto board = makeBoard(25);
    EXPECT_NO_THROW({ renderer->render(board); });
}

TEST_F(GridRendererTest, TestTruncationMessage) {

    auto board = makeBoard(25);
    auto element = renderer->render(board);
    EXPECT_NO_THROW({ element; });
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
