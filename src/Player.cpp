#include "Player.h"
#include <algorithm>  // std::find miatt szükséges
#include <iostream>

// Konstruktor implementációja, alapértelmezett értékekkel
Player::Player(int h, int m) : health(h), money(m) {}

// Életerő módosítása
void Player::modifyHealth(int amount) {
    health += amount;
    if (health < 0) health = 0;  // Az életerő nem lehet negatív
}

// Pénz módosítása
void Player::modifyMoney(int amount) {
    money += amount;
}

// Tárgy hozzáadása az inventory-hoz
void Player::addItem(int itemID) {
    items.push_back(itemID);
}

// Tárgy eltávolítása (ha megtalálható az inventory-ban)
void Player::removeItem(int itemID) {
    auto it = std::find(items.begin(), items.end(), itemID);
    if (it != items.end()) {
        items.erase(it);
    }
}

// Ellenőrizhetjük, hogy a játékos rendelkezik-e egy adott tárggyal
bool Player::hasItem(int itemID) const {
    return std::find(items.begin(), items.end(), itemID) != items.end();
}

// Az aktuális állapot lekérdezése a mentéshez
int Player::getHealth() const {
    return health;
}

int Player::getMoney() const {
    return money;
}

std::vector<int> Player::getItems() const {
    return items;
}
