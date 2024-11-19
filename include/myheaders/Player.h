#ifndef PLAYER_H
#define PLAYER_H

#include <vector>

class Player {
public:
    int health;
    int money;
    std::vector<int> items;

    // Konstruktor
    Player(int h = 100, int m = 0);

    // Funkciók az osztályban
    void modifyHealth(int amount);
    void modifyMoney(int amount);
    void addItem(int itemID);
    void removeItem(int itemID);
    bool hasItem(int itemID) const;

    // Getterek az aktuális állapothoz (pl. mentéshez)
    int getHealth() const;
    int getMoney() const;
    std::vector<int> getItems() const;
};

#endif // PLAYER_H
