#include <gtest/gtest.h>
#include "ui/TileRenderer.h"
#include <ftxui/dom/elements.hpp>

class TileRendererTest : public ::testing::Test {
protected:
    TileRenderer renderer;
};

TEST_F(TileRendererTest, TestTileCorrectness) {

    int boardSize = 3;


    EXPECT_TRUE(renderer.isCorrect(1, 0, 0, boardSize));
    EXPECT_TRUE(renderer.isCorrect(2, 1, 0, boardSize));
    EXPECT_TRUE(renderer.isCorrect(3, 2, 0, boardSize));
    EXPECT_TRUE(renderer.isCorrect(4, 0, 1, boardSize));
    EXPECT_TRUE(renderer.isCorrect(5, 1, 1, boardSize));


    EXPECT_FALSE(renderer.isCorrect(1, 1, 0, boardSize));
    EXPECT_FALSE(renderer.isCorrect(2, 0, 1, boardSize));
    EXPECT_FALSE(renderer.isCorrect(0, 0, 0, boardSize));
}

TEST_F(TileRendererTest, TestTileWidth) {


    auto smallBoard = renderer.render(1, 0, 0, 3);
    auto mediumBoard = renderer.render(1, 0, 0, 10);
    auto largeBoard = renderer.render(1, 0, 0, 25);


    EXPECT_NO_THROW({ smallBoard; });
    EXPECT_NO_THROW({ mediumBoard; });
    EXPECT_NO_THROW({ largeBoard; });
}

TEST_F(TileRendererTest, TestHintPosition) {

    EXPECT_FALSE(renderer.getHintPosition().has_value());


    renderer.setHintPosition(std::make_pair(1, 2));
    auto hintPos = renderer.getHintPosition();
    ASSERT_TRUE(hintPos.has_value());
    EXPECT_EQ(hintPos->first, 1);
    EXPECT_EQ(hintPos->second, 2);


    renderer.setHintPosition(std::nullopt);
    EXPECT_FALSE(renderer.getHintPosition().has_value());
}

TEST_F(TileRendererTest, TestRenderEmptyTile) {

    auto emptyTile = renderer.render(0, 0, 0, 3);
    EXPECT_NO_THROW({ emptyTile; });
}

TEST_F(TileRendererTest, TestRenderCorrectTile) {

    auto correctTile = renderer.render(1, 0, 0, 3);
    EXPECT_NO_THROW({ correctTile; });
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}