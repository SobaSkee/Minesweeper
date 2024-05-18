#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "Game.h"
#include "Title.h"
#include <algorithm>
using namespace std;


void setWindow(sf::RenderWindow &window, int rowCount, int colCount) {
    window.create(sf::VideoMode(colCount*32, (rowCount*32)+100), "Minesweeper", sf::Style::Close);
}


void renderLeaderboard(int rowCount, int colCount, Game &gameBoard, string playerName) {
//    cout << "Rendering leaderboard window..." << endl;

    sf::RenderWindow leaderboardWindow;
    leaderboardWindow.create(sf::VideoMode(colCount * 16, (rowCount * 16) + 50), "Leaderboard", sf::Style::Close);
//    for (int i = 0; i < rowCount; i++) {
//        for (int j = 0; j < colCount; j++) {
//            ??
//        }
//    }


    Font font;
    font.loadFromFile("files/font.ttf");
    // load content from leaderboard.txt
    ifstream readFile("files/leaderboard.txt");
    vector<PlayerData> data;
    if (readFile.is_open()) {
        string line;
        while (getline(readFile, line)) {
            stringstream stream(line);
            string timeStr, name;
            if (getline(stream, timeStr, ',')) {
                stringstream timeStream(timeStr);
                string minStr, secStr;
                if (getline(timeStream, minStr, ':') && getline(timeStream, secStr, ':')) {
                    int min = stoi(minStr);
                    int sec = stoi(secStr);
                    getline(stream, name);
                    data.push_back(PlayerData(min, sec, name));
                }
            }
        }
        readFile.close();
    }
    PlayerData currentPlayer;
    currentPlayer.minutes = gameBoard.currentPlayerMin;
    currentPlayer.seconds = gameBoard.currentPlayerSecInt;
    bool hasBeatenAny = false;
    // only add the new player if they are a winner
    if (gameBoard.gameWon) {


        for (auto player: data) {
            if (comparePlayers(currentPlayer, player)) {
                hasBeatenAny = true;
                playerName += "*";
                break;
            }
        }
        if (hasBeatenAny) {
            for (auto& player : data) {
                if (player.name.find('*') != string::npos) {
                    // Remove the asterisk from the player's name
                    player.name.erase(remove(player.name.begin(), player.name.end(), '*'), player.name.end());
                }
            }
        }

        data.push_back(PlayerData(gameBoard.currentPlayerMin, gameBoard.currentPlayerSecInt, playerName));
        // sort the data for top five
        sortTopFive(data);
        // export top five to leaderboard.txt
        gameBoard.exportToLeaderboard(data);
    }

//    // sort the data for top five
//    sortTopFive(data);

    while (leaderboardWindow.isOpen()) {
        sf::Event event;
        while (leaderboardWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                gameBoard.resumeTimer();
                leaderboardWindow.close();

                for (int i = 0; i < rowCount; i++) {
                    for (int j = 0; j < colCount; j++) {
                        gameBoard.tiles[i][j].hideAll();
                    }
                }
            }
        }
        // create title object
        Title leaderboard;
        leaderboard.setTitleTextLB(font, leaderboardWindow);
        leaderboard.setLBData(font, leaderboardWindow, data, currentPlayer);

        leaderboardWindow.clear(sf::Color::Blue);
        // draw everything here
//        leaderboardWindow.draw(leaderboard.backgroundSprite);
        leaderboardWindow.draw(leaderboard.titleText);
        leaderboardWindow.draw(leaderboard.leaderboardText);

        leaderboardWindow.display();
    }
}


int main() {
    // load font object
    sf::Font font;
    font.loadFromFile("files/font.ttf");
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
//    backgroundTexture.loadFromFile("files/images/SobaSkee.png");
//    backgroundSprite.setTexture(backgroundTexture);
    // read contents from cfg file
    ifstream readFile("files/config.cfg");
    if (!readFile) {
        cout << "didnt open" << endl;
    }


    int rowCount, colCount, mineCount;
    string line;

    getline(readFile, line);
    colCount = stoi(line);
//    cout << "col count: " << colCount << endl;
    getline(readFile, line);
    rowCount = stoi(line);
//    cout << "row count: " << rowCount << endl;
    getline(readFile, line);
    mineCount = stoi(line);

    // set window dimensions
    sf::RenderWindow window;
    setWindow(window, rowCount, colCount);


    /* CREATE TEXT THINGS FOR THE WELCOME WINDOW */
    Title title;
    title.setTitleText(font, window);
    title.setNamePromptText(font, window);
    title.setNameInputText(font, window);

    bool gameOpen = false;

//    gameBoard.setTimerToNineFifty(gameBoard);
    Game gameBoard(colCount, rowCount, mineCount);
    // main loop
    while (window.isOpen()) {
        title.handleInput(window, gameOpen);
        string playerName = title.playerName;
        window.clear(sf::Color::Blue); // clear old frame
//        window.draw(backgroundSprite);
        /* DRAW ANYTHING HERE */

        window.draw(title.titleText); // draw title
        window.draw(title.namePromptText); // draw name prompt
        window.draw(title.nameInputText); // draw name input

        window.display(); // tell app that window is done running
        if(gameOpen) {
            sf::RenderWindow gameWindow;
            setWindow(gameWindow,rowCount, colCount);
            gameBoard.timer.restart();
            // main game loop
            while (gameWindow.isOpen()) {
                if (gameBoard.leaderboardOpen) {
                    renderLeaderboard(rowCount, colCount, gameBoard, playerName);
                    gameBoard.leaderboardOpen = false; // Reset the flag
                }
                // Game logic goes here
                sf::Event event;
                while (gameWindow.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        gameWindow.close();
                        gameOpen = false; // set to false when game window is closed
                    }
                }

                gameWindow.clear(sf::Color::White);
                // draw game elements here
                gameBoard.updateTimer();
                gameBoard.updateGameWindow(gameWindow);
                gameBoard.drawCounter(gameWindow, gameBoard.flagCount);
                gameWindow.display();
            }
        }

    }

    return 0;
}
