//
// Created by kesta on 4/8/2024.
//

#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

struct Tile {
    // sprites
    Sprite tile;
    Sprite icon;
    Sprite baseLayer;
    Sprite upperLayer;
    Sprite flag;

    // variables
    bool isMine;
    bool isFlagged;
    bool debugState;
    bool revealed;
    int minesNearby;



    vector<Tile*> neighbors;

    Tile(map<string, Texture> &textures, float x, float y) {
        baseLayer.setPosition(x, y);
        baseLayer.setTexture(textures["revealed"]);
        upperLayer.setPosition(x, y);
        upperLayer.setTexture(textures["hidden"]);
        flag.setPosition(x, y);
        flag.setTexture(textures["flag"]);
        icon.setPosition(x, y);

        // set initial visibility of flags
        flag.setColor(Color(255, 255, 255, 0));


        isMine = false;
        debugState = true;
        revealed = false;
        isFlagged = false;

    }

    void setMineStatus(bool mineStatus) {
        isMine = mineStatus;
//        cout << "current tile mine status: " << isMine << endl;
    }

    void changeIcon(map<string, Texture> &textures) {
        if (isMine) {
            icon.setTexture(textures["mine"]);
        }
        else {
            switch (minesNearby) {
                case 1:
                    icon.setTexture(textures["one"]);
                    break;
                case 2:
                    icon.setTexture(textures["two"]);
                    break;
                case 3:
                    icon.setTexture(textures["three"]);
                    break;
                case 4:
                    icon.setTexture(textures["four"]);
                    break;
                case 5:
                    icon.setTexture(textures["five"]);
                    break;
                case 6:
                    icon.setTexture(textures["six"]);
                    break;
                case 7:
                    icon.setTexture(textures["seven"]);
                    break;
                case 8:
                    icon.setTexture(textures["eight"]);
                    break;
                default:
                    break;
            }
        }
    }

    void addNeighbor(Tile* neighbor) {
        neighbors.push_back(neighbor);
    }

    void revealIcon(int &tilesRevealed, bool gameOver){
        // check if tile has not been revealed before, if so increment
        if (!revealed && !isFlagged && !isMine && !gameOver) {
            upperLayer.setColor(Color(255, 255, 255, 0));
            revealed = true;
            tilesRevealed++;

            if (minesNearby == 0) {
                for (Tile *neighbor : neighbors) {
                    if (!neighbor->isFlagged) {
                        neighbor->revealIcon(tilesRevealed, gameOver);
                    }
                }
            }
        }
        if (isMine && !isFlagged) {
            upperLayer.setColor(Color(255, 255, 255, 0));
            revealed = true;
        }
        // otherwise do nothing
    }

    // used for game end flagging all the mines
    void revealFlag() {
        if (!revealed) {
            upperLayer.setColor(Color(255, 255, 255, 0));
            flag.setColor(Color(255, 255, 255, 255));
//            isFlagged = true;
        }
    }


    void toggleDebug() {
        if (isMine) {
            if (debugState) {
                debugState = false;
                upperLayer.setColor(Color(255, 255, 255, 0));
            }
            else if (!debugState) {
                debugState = true;
                upperLayer.setColor(Color(255, 255, 255, 255));
            }
        }
    }

    // use for hiding after when game is paused
    void hideAll() {
        upperLayer.setColor(Color(255, 255, 255, 255));
        icon.setColor(Color(255, 255, 255, 255));
        if (revealed) {
            upperLayer.setColor(Color(255, 255, 255, 0));
        }
    }
    // used to reset back to original state
    void revealAll() {
        upperLayer.setColor(Color(255, 255, 255, 0));
        icon.setColor(Color(255, 255, 255, 0));
    }

    void toggleFlag(int &flagCount, int mineCount) {
        if (!revealed) {
            if (isFlagged) {
                flag.setColor(Color(255, 255, 255, 0));
                isFlagged = false;
                flagCount--;
            }
            else {
//                if (flagCount < mineCount){
//                    flag.setColor(Color(255, 255, 255, 255));
//                    isFlagged = true;
//                    flagCount++;
//                }
                flag.setColor(Color(255, 255, 255, 255));
                isFlagged = true;
                flagCount++;
            }
        }
    }


};