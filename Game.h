#pragma once
#include <SFML/Graphics.hpp>
#include "Tile.h"
#include "Title.h"
#include <cstdlib>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;
using namespace sf;

struct Game {

//    sf::RenderWindow leaderboardWindow;

    int colCount;
    int rowCount;
    int mineCount;

    int boardWidth;
    int boardHeight;

    int currentPlayerMin;
    int currentPlayerTotalSec;
    int currentPlayerSecInt;

    vector<vector<Tile>> tiles;
    map<string, Texture> textures;
    vector<vector<bool>> tileVals2D;

    // sprites
    Sprite faceS;
    Sprite loseS;
    Sprite winS;
    Sprite debugS;
    Sprite playpauseS;
    Sprite playS;
    Sprite leaderboardS;
    Sprite timerS;

    // used for the digits png
    Sprite digitS;
    Sprite ones;
    Sprite tens;
    Sprite hundreds;
    Sprite thousands;



    // timer
    Clock timer;
    Time pausedTime;
    Time totalElapsedTime;

    // booleans
    bool gameOver;
    bool gameWon;
    bool gamePaused;
    bool timerPaused;
    bool leaderboardOpen;
    bool debugClicked;

    // counters
    int tilesRevealed;
    int flagCount;

    // functions
    // default constructor
    Game(int colCount, int rowCount, int mineCount) {
        this->colCount = colCount;
        this->rowCount = rowCount;
        this->mineCount = mineCount;
        boardWidth = colCount*32;
        boardHeight = (rowCount*32);
        tileVals2D.resize(rowCount, vector<bool>(colCount, false));

        // default values
        gameOver = false;
        gameWon = false;
        gamePaused = false;
        leaderboardOpen = false;
        debugClicked = true;
        tilesRevealed = 0;
        flagCount = 0;

        pausedTime = Time::Zero;
        timerPaused = false;

        currentPlayerMin = 0;
        currentPlayerTotalSec = 0;

        Texture temp;
        // laod numbers
        temp.loadFromFile("files/images/number_1.png");
        textures.emplace("one", temp);
        temp.loadFromFile("files/images/number_2.png");
        textures.emplace("two", temp);
        temp.loadFromFile("files/images/number_3.png");
        textures.emplace("three", temp);
        temp.loadFromFile("files/images/number_4.png");
        textures.emplace("four", temp);
        temp.loadFromFile("files/images/number_5.png");
        textures.emplace("five", temp);
        temp.loadFromFile("files/images/number_6.png");
        textures.emplace("six", temp);
        temp.loadFromFile("files/images/number_7.png");
        textures.emplace("seven", temp);
        temp.loadFromFile("files/images/number_8.png");
        textures.emplace("eight", temp);

        // load faces
        temp.loadFromFile("files/images/face_happy.png");
        textures.emplace("happy", temp);
        temp.loadFromFile("files/images/face_lose.png");
        textures.emplace("lose", temp);
        temp.loadFromFile("files/images/face_win.png");
        textures.emplace("win", temp);

        // load buttons
        temp.loadFromFile("files/images/debug.png");
        textures.emplace("debug", temp);
        temp.loadFromFile("files/images/pause.png");
        textures.emplace("pause", temp);
        temp.loadFromFile("files/images/play.png");
        textures.emplace("play", temp);
        temp.loadFromFile("files/images/mine.png");
        textures.emplace("mine", temp);
        temp.loadFromFile("files/images/leaderboard.png");
        textures.emplace("leaderboard", temp);

        temp.loadFromFile("files/images/digits.png");
        textures.emplace("digits", temp);

        // load tile stuff
        temp.loadFromFile("files/images/tile_hidden.png");
        textures.emplace("hidden", temp);
        temp.loadFromFile("files/images/tile_revealed.png");
        textures.emplace("revealed", temp);
        temp.loadFromFile("files/images/flag.png");
        textures.emplace("flag", temp);

        // set positions and textures for buttons
        faceS.setTexture(textures["happy"]);
        faceS.setPosition(((colCount/2.0)*32)-32, 32*(rowCount+0.5));
        loseS.setTexture(textures["lose"]);
        loseS.setPosition(((colCount/2.0)*32)-32, 32*(rowCount+0.5));
        winS.setTexture(textures["lose"]);
        winS.setPosition(((colCount/2.0)*32)-32, 32*(rowCount+0.5));
        debugS.setTexture(textures["debug"]);
        debugS.setPosition(colCount*32-304, 32*(rowCount+0.5));
        playpauseS.setTexture(textures["pause"]);
        playpauseS.setPosition(colCount*32-240, 32*(rowCount+0.5));
        playS.setTexture(textures["play"]);
        leaderboardS.setTexture(textures["leaderboard"]);
        leaderboardS.setPosition(colCount*32-176, 32*(rowCount+0.5));

        // digits
//        hundreds.setTexture(textures["digits"]);
//        hundreds.setTextureRect(Rect<int>())

        createTiles();
        getMines();
        countNearbyMines();
        setIcons();

    }

    void createTiles() {
        float x = 0;
        float y = 0;
        for (int i = 0; i < rowCount; i++) {
            vector<Tile> row;
            for (int j = 0; j < colCount; j++) {
                row.push_back(Tile(textures, x, y));
                x += 32.f;
            }
            tiles.push_back(row);
            x = 0;
            y += 32.f;
        }


        // determine neighboring tiles
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                // x directions of tile
                for (int xd = -1; xd < 2; xd++) {
                    // y directions of tile
                    for (int yd = -1; yd < 2; yd++) {
                        int newRowI = i + xd;
                        int newColJ = j + yd;
                        // check if it's not the current position and smaller than edges
                        if (newRowI >= 0 && newRowI < rowCount && newColJ >= 0
                            && newColJ < colCount && !(xd == 0 && yd == 0)) {
                            tiles[i][j].addNeighbor(&tiles[newRowI][newColJ]);
                        }
                    }
                }
            }
        }
        // debug stuff
//        Tile& firstTile = tiles[10][5];
//        for (Tile* neighbor : firstTile.neighbors) {
//            cout << "(" << neighbor->rowIndex << ", " << neighbor->colIndex << ")" << endl;
//        }
        gameOver = false;
    }

    void handleFaceClick(RenderWindow &window) {
        if (Mouse::isButtonPressed(Mouse::Left)) {
            Vector2i position = Mouse::getPosition(window);
            int faceY = 32 * (rowCount + 0.5);
            int faceX = ((colCount / 2.0) * 32) - 32;
            if (position.y >= faceY && position.y < faceY + 60) {
                if (position.x >= faceX && position.x < faceX + 60) {
                    // reset the game board if the player clicks on the happy face button
                    resetBoard(window);
                }
            }
        }
    }

    void handlePausePlayClick(RenderWindow& window) {
        static Clock clickClock;
        static const Time clickDelay = milliseconds(200); // adjust the delay as needed
        bool currentLeftMousePressed = Mouse::isButtonPressed(Mouse::Left);

        if (currentLeftMousePressed && clickClock.getElapsedTime() >= clickDelay) {
            Vector2i position = Mouse::getPosition(window);
            float buttonY = 32 * (rowCount + 0.5);
            float buttonX = colCount * 32 - 240;

            if (position.y >= buttonY && position.y < buttonY + 60 &&
                position.x >= buttonX && position.x < buttonX + 60) {
                clickClock.restart();
                if (!gamePaused) {
                    pauseTimer();
                }
                else {
                    resumeTimer();
                }
                updatePausePlay();
            }
        }
    }


    void exportToLeaderboard(vector<PlayerData> &data) {
        std::ofstream leaderboardFile("files/leaderboard.txt", std::ios_base::trunc);
        for (auto player: data) {
            string minutesStr = to_string(player.minutes);
            string secondsStr = to_string(player.seconds);
            if (player.minutes < 10) {
                minutesStr = "0" + minutesStr;
            }
            if (player.seconds < 10) {
                secondsStr = "0" + secondsStr;
            }
            string name = player.name;
            leaderboardFile << minutesStr << ":" << secondsStr << ", " << player.name << endl;
        }
        leaderboardFile.close();
    }


    void handleLeaderboardClick(RenderWindow& window) {
        if (Mouse::isButtonPressed(Mouse::Left)) {
            Vector2i position = Mouse::getPosition(window);
            float buttonY = 32 * (rowCount + 0.5);
            float buttonX = colCount * 32 - 176;

            if (position.y >= buttonY && position.y < buttonY + 60 &&
                position.x >= buttonX && position.x < buttonX + 60) {
                if (leaderboardOpen) {
                    for (int i = 0; i < rowCount; i++) {
                        for (int j = 0; j < colCount; j++) {
                            tiles[i][j].revealAll();
                        }
                    }
                }
                pauseTimer();
                updateTimer();
                leaderboardOpen = true;
            }
        }
    }

    void handleRightClick(RenderWindow &window) {
        Vector2i position = Mouse::getPosition(window);
        if (position.y >= 0 && position.y < boardHeight) {
            if (position.x >= 0 && position.x < boardWidth) {
                int x = position.y / 32;
                int y = position.x / 32;
                tiles[x][y].toggleFlag(flagCount, mineCount);
                cout << "current flag count: " << flagCount << endl;
            }
        }
    }

    void updateGameWindow(RenderWindow &window) {

        static bool prevLeftMousePressed = false;
        static bool prevRightMousePressed = false;
        static Clock clickClock;
        static const Time clickDelay = milliseconds(200);

        bool currentLeftMousePressed = Mouse::isButtonPressed(Mouse::Left);
        bool currentRightMousePressed = Mouse::isButtonPressed(Mouse::Right);

        if (!gameOver && !gameWon && !gamePaused) {
            currentPlayerMin = (totalElapsedTime.asSeconds()) / 60;
            currentPlayerTotalSec = totalElapsedTime.asSeconds();
            currentPlayerSecInt = currentPlayerTotalSec % 60;
            // check for left mouse click
            if (currentLeftMousePressed && !prevLeftMousePressed && clickClock.getElapsedTime() >= clickDelay) {
                prevLeftMousePressed = true;
                clickClock.restart();

                Vector2i position = Mouse::getPosition(window);
                if (position.y >= 0 && position.y < boardHeight) {
                    if (position.x >= 0 && position.x < boardWidth) {
                        int x = position.y / 32;
                        int y = position.x / 32;
                        tiles[x][y].revealIcon(tilesRevealed, gameOver);
                        // check if position is a mine
                        if (tiles[x][y].isMine && !tiles[x][y].isFlagged) {
                            gameOver = true;
                        }

                        cout << tilesRevealed << endl;
                        if (checkGameWon()) {
                            gameWon = true;
                        }
                    }
                }
                // check for debug press
                int debugY = 32 * (rowCount + 0.5);
                int debugX = colCount * 32 - 304;
                if (position.y >= debugY && position.y < debugY + 60) {
                    if (position.x >= debugX && position.x < debugX + 60) {
                        debug();
                        debugClicked = !debugClicked;
                    }
                }
                handlePausePlayClick(window);
                handleLeaderboardClick(window);

            }
            else if (!currentLeftMousePressed) {
                prevLeftMousePressed = false;
            }

            if (currentRightMousePressed && !prevRightMousePressed && clickClock.getElapsedTime() >= clickDelay) {
                prevRightMousePressed = true;
                clickClock.restart();
                handleRightClick(window);
            }
            else if (!currentRightMousePressed) {
                prevRightMousePressed = false;
            }
        }


        if (gameWon) {
            // flag all mines
            for (int i = 0; i < rowCount; i++) {
                for (int j = 0; j < colCount; j++) {
                    if (tiles[i][j].isMine) {
                        tiles[i][j].revealFlag();
                    }
                }
            }
            handleFaceClick(window);
            timer.restart();
        }

        if (gameOver) {
            // handle face click if the game is over or won
            handleFaceClick(window);
            timer.restart();
        }
        if (gamePaused) {
            handlePausePlayClick(window);
//            handleLeaderboardClick(window);

        }
        // reset board in general if user clicks on face
        handleFaceClick(window);
        handleLeaderboardClick(window);
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                window.draw(tiles[i][j].baseLayer);
                window.draw(tiles[i][j].icon);
                window.draw(tiles[i][j].upperLayer);
                window.draw(tiles[i][j].flag);
            }
        }
        updateFaces();
        window.draw(faceS);
        window.draw(debugS);
        window.draw(playpauseS);
        window.draw(leaderboardS);
        window.draw(timerS);
        window.draw(thousands);
        window.draw(hundreds);
        window.draw(tens);
        window.draw(ones);

    }


    void getMines() {
        vector<pair<int, int>> mineCoords(mineCount);
        int randomCol;
        int randomRow;
        bool available;

        for (int i = 0; i < mineCount; i++) {
            available = false;
            while (!available) {
                randomCol = rand() % colCount;
                randomRow = rand() % rowCount;
                if (!tileVals2D[randomRow][randomCol]) {
                    mineCoords[i] = make_pair(randomRow, randomCol);
                    tileVals2D[randomRow][randomCol] = true;
                    available = true;
                }
            }
        }
        int falseCount = 0;
        int trueCount = 0;
        bool val = true;
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                if (val == tileVals2D[i][j]) {
                    trueCount++;
                }
                else {
                    falseCount++;
                }
            }
        }
//        cout << "Number of false tiles: " << falseCount << endl;
//        cout << "Number of true tiles: " << trueCount << endl;

        setMineStatuses();

    }

    // set mine status for each tile
    void setMineStatuses() {
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                tiles[i][j].setMineStatus(tileVals2D[i][j]);
            }
        }
    }

    void setIcons() {
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                //            cout << "mine status: " << tiles[i][j].ismine << endl;
                tiles[i][j].changeIcon(textures);
            }
        }
    }

    void countNearbyMines() {
        // iterate over all tiles
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                Tile &currentTile = tiles[i][j];
                int minesNearby = 0;
                for (Tile *neighbor: currentTile.neighbors) {
                    if (neighbor->isMine) {
                        minesNearby++;
                    }
                }
                currentTile.minesNearby = minesNearby;

//                cout << "Number of mines nearby mine (" << i << ", " << j << ") is: " << currentTile.minesNearby << endl;
//
//                for (Tile* neighbor : currentTile.neighbors) {
//                    cout << "(" << neighbor->rowIndex << ", " << neighbor->colIndex << ")" << ", neighbor mine status: " << neighbor->isMine << endl;
//                }
            }
        }
    }

    void updateFaces(){
        if (gameOver) {
            faceS.setTexture(textures["lose"]);
            for (int i = 0; i < rowCount; i++) {
                for (int j = 0; j < colCount; j++) {
                    if (tiles[i][j].isMine) {
                        tiles[i][j].revealIcon(tilesRevealed, gameOver);
                    }
                    //            cout << "mine status: " << tiles[i][j].ismine << endl;
                }
            }
        }
        if(gameWon) {
            faceS.setTexture(textures["win"]);
            for (int i = 0; i < rowCount; i++) {
                for (int j = 0; j < colCount; j++) {
                    if (tiles[i][j].isMine) {
                        tiles[i][j].revealFlag();
                    }
                }
            }
        }
    }


    void updatePausePlay(){
        if (gamePaused) {
            for (int i = 0; i < rowCount; i++) {
                for (int j = 0; j < colCount; j++) {
                    tiles[i][j].hideAll();
                }
            }
            playpauseS.setTexture(textures["pause"]);
            gamePaused = false;
        }
        else if (!gamePaused) {
            // reset board to original state
            for (int i = 0; i < rowCount; i++) {
                for (int j = 0; j < colCount; j++) {
                    tiles[i][j].revealAll();
                }
            }
            playpauseS.setTexture(textures["play"]);
            gamePaused = true;
        }

    }

    void resetBoard(RenderWindow &window) {
        gameOver = false;
        gameWon = false;
        gamePaused = false;
        leaderboardOpen = false;
        flagCount = 0;
        pausedTime = Time::Zero;
        timer.restart();
        faceS.setTexture(textures["happy"]);
        playpauseS.setTexture(textures["pause"]);
        // clear old tiles and generate new ones
        tiles.clear();
        createTiles();

        // clear old mines and generate new ones
        tileVals2D.clear();
        tileVals2D.resize(rowCount, vector<bool>(colCount, false));
        getMines();


        // update icons
        countNearbyMines();
        setIcons();

        // update counters
        tilesRevealed = 0;

        // refresh window
        updateGameWindow(window);
    }

    bool checkGameWon() {
        int numMines = (colCount*rowCount) - tilesRevealed;


        if (numMines == mineCount) {
            gameWon = true;
//            cout << "Game won" << endl;
//            cout << tilesRevealed << endl;

            return true;
        }
        return false;
    }

    void debug() {
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                tiles[i][j].toggleDebug();
            }
        }
    }

    void drawCounter(RenderWindow &window, int flagCount) {
        // calculate counter and convert to string

        int counter = mineCount - flagCount;
//        cout << "counter: " << counter << endl;
        string counterStr;
        if (flagCount > mineCount) {
            counterStr = "-";
            counter *= -1;
        }

        counterStr += to_string(counter);
        // convert flagCount to a string
        string flagCountStr = to_string(flagCount);

        // starting positions of digits
        int startX = 33;
        int  startY = 32 * (rowCount + 0.5) + 16;


        // fill with 0's if less than 3 in size
        while (counterStr.length() < 3) {
            counterStr = "0" + counterStr;
        }

        for (char digitChar : counterStr) {
            int digit;
            if (digitChar == '-') {
                digit = 10;
            }
            else {
                digit = digitChar - '0';
            }
            digitS.setTexture(textures["digits"]);
            digitS.setTextureRect(IntRect(digit * 21, 0, 21, 32));
            digitS.setPosition(startX, startY);
            window.draw(digitS);
            startX += 21.f;
        }
    }

    // timer stuff
    void pauseTimer() {
        if (!timerPaused) {
            pausedTime += timer.getElapsedTime();
            timerPaused = true;
        }
    }
    void resumeTimer() {
        if (timerPaused) {
            Time elapsed = timer.restart();
            timerPaused = false;
        }
    }

    void updateTimer() {
        int minutes, seconds;
        totalElapsedTime = pausedTime + timer.getElapsedTime();
//        cout << "Total Elapsed time: " << (int)totalElapsedTime.asSeconds() << endl;
        // when game is not paused
        if (!gamePaused) {
            int totalSeconds = totalElapsedTime.asSeconds();
            minutes = totalSeconds / 60;
            seconds = totalSeconds % 60;
        }
        // when game is paused just display the current paused time
        else {
            int totalPausedSeconds = pausedTime.asSeconds();
            minutes = totalPausedSeconds / 60;
            seconds = totalPausedSeconds % 60;
        }


        updateTimerSprites(minutes, seconds);
    }


    void updateTimerSprites(int minutes, int seconds) {

        // minutes position
        int minutesX = (colCount * 32) - 97;
        int minutesY = 32 * (rowCount + 0.5) + 16;


        // seconds position
        int secondsX = (colCount * 32) - 54;
        int secondsY = 32 * (rowCount + 0.5) + 16;
        int thousandsInt = minutes / 10;
        int hundredsInt = minutes;

        if (minutes == 10) {
            hundredsInt = 0;
        }
        else if (minutes > 10) {
            hundredsInt = minutes % 10;
        }

        ones.setTexture(textures["digits"]);
        ones.setTextureRect(IntRect((seconds % 10) * 21, 0, 21, 32));
        ones.setPosition(secondsX+21, secondsY);

        tens.setTexture(textures["digits"]);
        tens.setTextureRect(IntRect((seconds / 10) * 21, 0, 21, 32));
        tens.setPosition(secondsX, secondsY);

        hundreds.setTexture(textures["digits"]);
        hundreds.setTextureRect(IntRect(hundredsInt * 21, 0, 21, 32));
        hundreds.setPosition(minutesX+21, minutesY);

        thousands.setTexture(textures["digits"]);
        thousands.setTextureRect(IntRect(thousandsInt * 21, 0, 21, 32));
        thousands.setPosition(minutesX, minutesY);
    }


};