#pragma once
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <iomanip>
using namespace std;
using namespace sf;

struct PlayerData {
    int minutes;
    int seconds;
    string name;
    PlayerData() {
        minutes = 0;
        seconds = 0;
        name = "";
    }
    // Constructor
    PlayerData(int minutes, int seconds, string& name) {
        this->minutes = minutes;
        this->seconds = seconds;
        this->name = name;
        this->name.erase(0, this->name.find_first_not_of(" \t")); // Trim leading whitespace
        this->name.erase(this->name.find_last_not_of(" \t") + 1); // Trim trailing whitespace

    }
};
bool comparePlayers(PlayerData &a, PlayerData &b);
void sortTopFive(vector<PlayerData> &data);

struct Title {
    sf::Text titleText;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Text namePromptText;
    sf::Text nameInputText;
    sf::String playerName;
    Text leaderboardText;

    void setTitleTextLB(Font &font, RenderWindow &window) {
        backgroundTexture.loadFromFile("files/images/GOJOOOOO.png");
        backgroundSprite.setTexture(backgroundTexture);
        titleText.setFont(font);
        titleText.setString("LEADERBOARD");
        titleText.setCharacterSize(20);
        titleText.setFillColor(sf::Color::White);
        titleText.setStyle(sf::Text::Bold | sf::Text::Underlined);
        setText(titleText, window.getSize().x / 2, (window.getSize().y / 2) - 120);
    }

    void setTitleText(Font &font, RenderWindow &window) {
        titleText.setFont(font);
        titleText.setString("Welcome to Minesweeper!");
        titleText.setCharacterSize(24);
        titleText.setFillColor(sf::Color::White);
        titleText.setStyle(sf::Text::Bold | sf::Text::Underlined);
        setText(titleText, window.getSize().x / 2, (window.getSize().y / 2) - 150);
    }

    void setNamePromptText(sf::Font &font, sf::RenderWindow &window) {
        namePromptText.setFont(font);
        namePromptText.setString("Enter your name:");
        namePromptText.setCharacterSize(20);
        namePromptText.setFillColor(sf::Color::White);
        namePromptText.setStyle(sf::Text::Bold);
        setText(namePromptText, window.getSize().x / 2, (window.getSize().y / 2) - 75);
    }

    void setNameInputText(sf::Font &font, sf::RenderWindow &window) {
        nameInputText.setFont(font);
        nameInputText.setString("|");
        nameInputText.setCharacterSize(18);
        nameInputText.setFillColor(sf::Color::Yellow);
        nameInputText.setStyle(sf::Text::Bold);
        float nameInputX = window.getSize().x / 2.0f;
        float nameInputY = window.getSize().y / 2.0f;
        setText(nameInputText, nameInputX, nameInputY - 45);
    }

    void handleInput(sf::RenderWindow &window, bool &gameOpen) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128) {
                    // Backspace
                    if (event.text.unicode == '\b' && playerName.getSize() > 0) {
                        playerName.erase(playerName.getSize() - 1, 1);
                    }
                        // add characters to name
                    else if (isalpha(event.text.unicode)) {
                        if (playerName.getSize() < 10) {
                            // Add name to text object
                            if (playerName.getSize() == 0) {
                                playerName += static_cast<char>(toupper(event.text.unicode));
                            }
                            else {
                                playerName += static_cast<char>(tolower(event.text.unicode));
                            }
                        }
                    }
                    // update name input text
                    nameInputText.setString(playerName + "|");
                    // automatically center user input
                    sf::FloatRect textBounds = nameInputText.getLocalBounds();
                    float newX = window.getSize().x / 2.0f - textBounds.width / 2.0f;
                    nameInputText.setPosition(newX, window.getSize().y / 2.0f - 45);
                }
            }
            // enter key
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                if (playerName.getSize() > 0) {

                    window.close();
                    // open game window
                    gameOpen = true;
                }
            }
        }
    }


    void setText(sf::Text &text, float x, float y){
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width/2.0f,
                       textRect.top + textRect.height/2.0f);
        text.setPosition(sf::Vector2f(x, y));
    }

    void setLBData(Font &font, RenderWindow &window, vector<PlayerData> data, PlayerData &currentPlayer) {
        leaderboardText.setFont(font);
        leaderboardText.setCharacterSize(18);
        leaderboardText.setFillColor(sf::Color::White);
        leaderboardText.setStyle(sf::Text::Bold);
        leaderboardText.setString("");
        for (int i = 0; i < data.size(); i++) {
            int minutes = data[i].minutes;
            int seconds = data[i].seconds;
            string name = data[i].name;

            stringstream stream;
            if (minutes < 10 && seconds < 10) {
                stream << i+1 << ".\t0" << minutes << ":0" << seconds << "\t" << name;
            }
            else if (minutes < 10) {
                stream << i+1 << ".\t0" << minutes << ":" << seconds << "\t" << name;
            }
            else if (seconds < 10) {
                stream << i+1 << ".\t" << minutes << ":0" << seconds << "\t" << name;
            }
            else {
                stream << i+1 << ".\t" << minutes << ":" << seconds << "\t" << name;
            }

            leaderboardText.setString(leaderboardText.getString() + stream.str() + "\n\n");
        }
        setText(leaderboardText, window.getSize().x / 2, (window.getSize().y / 2) + 20);
    }
};
bool comparePlayers(PlayerData &a, PlayerData &b) {
    // compare minutes and sort by ascending
    if (a.minutes != b.minutes) {
        return a.minutes < b.minutes;
    }
    // if minutes are the same compare the seconds
    return a.seconds < b.seconds;
}

void sortTopFive(vector<PlayerData> &data) {
    sort(data.begin(), data.end(), comparePlayers);

    if (data.size() > 5) {
        data.erase(data.begin() + 5, data.end());
    }

}