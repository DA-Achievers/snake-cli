#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <map>
#include <deque>
#include <algorithm>
using namespace std;
using namespace std::this_thread;

char direction='r';
bool paused=false;                    // new pause state
vector<int> highScores;               // top scores in memory only

// helper to generate random cell not occupied by snake or other cell
pair<int,int> random_cell_excluding(const deque<pair<int,int>> &snake,
                                    const pair<int,int> &other = {-1,-1}) {
    pair<int,int> p;
    do {
        p = make_pair(rand()%10, rand()%10);
    } while(find(snake.begin(), snake.end(), p) != snake.end() || p==other);
    return p;
}

void input_handler(){
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    map<char, char> keymap = {
        {'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}
    };
    while (true) {
        char input = getchar();
        if (keymap.find(input) != keymap.end()) {
            direction = keymap[input];
        } else if (input == 'q'){
            exit(0);
        } else if (input == 'p') {
            // toggle pause
            paused = !paused;
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void render_game(int size, deque<pair<int, int>> &snake,
                 pair<int, int> food, pair<int,int> poison, int score){
    for(size_t i=0;i<size;i++){
        for(size_t j=0;j<size;j++){
            if (i == food.first && j == food.second){
                cout << "🍎"; // normal food
            } else if (i == poison.first && j == poison.second){
                cout << "💀"; // poison
            } else if (find(snake.begin(), snake.end(), make_pair(int(i), int(j))) != snake.end()) {
                cout << "🐍";
            } else {
                cout << "⬜";
            }
        }
        cout << endl;
    }
    cout << "Length: " << snake.size()
         << " | Score: " << score
         << (paused ? " | PAUSED (press 'p' to resume)" : "") << endl;
}

pair<int,int> get_next_head(pair<int,int> current, char direction){
    pair<int, int> next;
    if(direction =='r'){
        next = make_pair(current.first,(current.second+1) % 10);
    } else if (direction=='l') {
        next = make_pair(current.first, current.second==0?9:current.second-1);
    } else if(direction =='d'){
        next = make_pair((current.first+1)%10,current.second);
    } else if (direction=='u'){
        next = make_pair(current.first==0?9:current.first-1, current.second);
    }
    return next;
}

void show_high_scores() {
    cout << "\n=== TOP 10 HIGH SCORES ===\n";
    if(highScores.empty()) {
        cout << "No scores yet.\n";
        return;
    }
    for(size_t i=0;i<highScores.size();i++) {
        cout << i+1 << ". " << highScores[i] << endl;
    }
}

void update_high_scores(int score) {
    highScores.push_back(score);
    sort(highScores.begin(), highScores.end(), greater<int>());
    if(highScores.size()>10) highScores.resize(10);
}

void game_play(){
    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0,0));

    pair<int, int> food = random_cell_excluding(snake);
    pair<int, int> poison = random_cell_excluding(snake, food);
    int score=0;
    int steps=0;
    int foodsEaten=0;
    int speedMs=500; // initial speed

    for(pair<int, int> head=make_pair(0,1);; head = get_next_head(head, direction)){
        cout << "\033[H";

        // if paused, just draw current state without moving snake
        if(paused) {
            render_game(10, snake, food, poison, score);
            sleep_for(200ms);
            continue;
        }

        // self collision
        if (find(snake.begin(), snake.end(), head) != snake.end()) {
            system("clear");
            cout << "Game Over - You hit yourself!\nFinal Score: " << score << endl;
            update_high_scores(score);
            show_high_scores();
            exit(0);
        }

        // poison collision
        if (head.first == poison.first && head.second == poison.second){
            system("clear");
            cout << "Game Over - You ate poison!\nFinal Score: " << score << endl;
            update_high_scores(score);
            show_high_scores();
            exit(0);
        }

        // normal food
        if (head.first == food.first && head.second == food.second) {
            // grow snake
            snake.push_back(head);
            score +=10;
            foodsEaten++;

            // speed up every 10 foods eaten
            if(foodsEaten%10==0 && speedMs>100) speedMs -= 50;

            // new food not on snake or poison
            food = random_cell_excluding(snake, poison);
        } else {
            // move snake
            snake.push_back(head);
            snake.pop_front();
        }

        // move poison every few steps
        steps++;
        if (steps % 10 == 0){
            poison = random_cell_excluding(snake, food);
        }

        render_game(10, snake, food, poison, score);
        sleep_for(std::chrono::milliseconds(speedMs));
    }
}


