#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <SDL.h> // Az SDL_Rect miatt

class Player {
public:
    int health;              // Játékos életereje
    int money;               // Játékos pénze
    int roomnum;             // Játékos szobaszáma
    SDL_Rect position;       // Játékos pozíciója (hajó koordinátái és mérete)
    std::vector<int> items;  // Játékos tárgyai (inventory)

    // Konstruktor
    Player(int h = 100, int m = 0, int roomnum = 0);

    // Értékek módosítása
    void modifyHealth(int amount);
    void modifyMoney(int amount);
    void modifyRoomNum(int newNum);

    // Inventory kezelése
    void addItem(int itemID);
    void removeItem(int itemID);
    bool hasItem(int itemID) const;

    // Getterek
    int getHealth() const;
    int getMoney() const;
    int getRoomNum() const;
    std::vector<int> getItems() const;
    SDL_Rect getPosition() const;

    // Setter a pozícióhoz
    void setPosition(int x, int y);

    void TestPlayerClass();
};

#endif // PLAYER_H
