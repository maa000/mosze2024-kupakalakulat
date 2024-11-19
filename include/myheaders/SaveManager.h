#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "Player.h"

using json = nlohmann::json;

class SaveManager {
public:
    json activeSave;
    std::string filename;  // A fájlnév tárolására szolgáló változó

    // Konstruktor, amely fájlnevet vár
    SaveManager(const std::string& file) : filename(file) {}

    // Játék mentése a megadott fájlba
    void saveGame(int roomIndex, const Player& player) {
        activeSave["roomIndex"] = roomIndex;
        activeSave["player"]["health"] = player.health;
        activeSave["player"]["money"] = player.money;
        activeSave["player"]["items"] = player.items;

        std::ofstream saveFile(filename);  // Használjuk a fájlnév adattagot
        if (saveFile.is_open()) {
            saveFile << activeSave.dump(4);
        }
    }

    // Játék betöltése a megadott fájlból
    bool loadGame(int& roomIndex, Player& player) {
        std::ifstream saveFile(filename);  // Használjuk a fájlnév adattagot
        if (!saveFile.is_open()) return false;

        json loadedData;
        saveFile >> loadedData;

        roomIndex = loadedData["roomIndex"];
        player.health = loadedData["player"]["health"];
        player.money = loadedData["player"]["money"];
        player.items = loadedData["player"]["items"].get<std::vector<int>>();

        activeSave = loadedData;
        return true;
    }
};

#endif // SAVEMANAGER_H
