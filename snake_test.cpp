#include <gtest/gtest.h>
#include "snake.h"


TEST(SnakeBehaviour, NextHeadRight) {
    pair<int, int> current = make_pair(rand() % 10, rand() % 10);
    EXPECT_EQ(get_next_head(current, 'r'),make_pair(current.first,current.second+1));
    
}


TEST(SnakeBehaviour, NextHeadLeft) {
  pair<int, int> current = make_pair(rand() % 10, rand() % 10);
  EXPECT_EQ(get_next_head(current, 'l'),make_pair(current.first,current.second-1));
  
}

TEST(SnakeBehaviour, NextHeadUp) {
  pair<int, int> current = make_pair(rand() % 10, rand() % 10);
  EXPECT_EQ(get_next_head(current, 'u'),make_pair(current.first-1,current.second));
}

TEST(SnakeBehaviour, NextHeadDown) {
  pair<int, int> current = make_pair(rand() % 10, rand() % 10);
  EXPECT_EQ(get_next_head(current, 'd'),make_pair(current.first+1,current.second));
  
}

TEST(SnakeBehaviour, RandomCellNotInSnake) {
    std::deque<std::pair<int,int>> snake;
    snake.push_back({0,0});
    snake.push_back({0,1});
    snake.push_back({0,2});

    // generate 100 cells to be safe
    for (int i=0;i<100;i++) {
        auto cell = random_cell_excluding(snake);
        // cell should not be in snake
        EXPECT_EQ(std::find(snake.begin(),snake.end(),cell),snake.end());
        // cell should be within board bounds
        EXPECT_GE(cell.first,0);
        EXPECT_LE(cell.first,9);
        EXPECT_GE(cell.second,0);
        EXPECT_LE(cell.second,9);
    }
}

TEST(SnakeBehaviour, SpeedDecreasesEvery10Foods) {
    int speed=500;
    for(int foods=1;foods<=30;foods++) {
        int newSpeed = compute_new_speed(foods,speed);
        if(foods%10==0 && speed>100) {
            EXPECT_EQ(newSpeed,speed-50);
            speed=newSpeed; // update speed for next loop
        } else {
            EXPECT_EQ(newSpeed,speed);
        }
    }
}

// Optional: test that poison never spawns on snake as well
TEST(SnakeBehaviour, PoisonCellNotInSnake) {
    std::deque<std::pair<int,int>> snake;
    for(int i=0;i<5;i++) snake.push_back({i,i});
    for(int i=0;i<50;i++){
        auto poison = random_cell_excluding(snake);
        EXPECT_EQ(std::find(snake.begin(),snake.end(),poison),snake.end());
    }


/** 
 * g++ -o my_tests snake_test.cpp -lgtest -lgtest_main -pthread;
 * This command is a two-part shell command. Let's break it down.

  The first part is the compilation:
  g++ -o my_tests hello_gtest.cpp -lgtest -lgtest_main -pthread


   * g++: This invokes the GNU C++ compiler.
   * -o my_tests: This tells the compiler to create an executable file named
     my_tests.
   * hello_gtest.cpp: This is the C++ source file containing your tests.
   * -lgtest: This links the Google Test library, which provides the core testing
     framework.
   * -lgtest_main: This links a pre-compiled main function provided by Google
     Test, which saves you from writing your own main() to run the tests.
   * -pthread: This links the POSIX threads library, which is required by Google
     Test for its operation.
 * 
*/