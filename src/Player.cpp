#include "Player.h"
#include <algorithm>  // std::find miatt szükséges
#include <iostream>
#include <cassert>

// Konstruktor implementációja, alapértelmezett értékekkel
Player::Player(int h, int m, int roomnum)
    : health(h), money(m), roomnum(roomnum), position({800, 700, 200, 200}) {}

// Életerő módosítása
void Player::modifyHealth(int amount) {
    health += amount;
    if (health < 0) health = 0; // Az életerő nem lehet negatív
}

// Pénz módosítása
void Player::modifyMoney(int amount) {
    money += amount;
}

// Szobaszám módosítása
void Player::modifyRoomNum(int newNum) {
    roomnum = newNum;
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

// Getterek
int Player::getHealth() const {
    return health;
}

int Player::getMoney() const {
    return money;
}

int Player::getRoomNum() const {
    return roomnum;
}

std::vector<int> Player::getItems() const {
    return items;
}

SDL_Rect Player::getPosition() const {
    return position;
}

// Pozíció beállítása
void Player::setPosition(int x, int y) {
    position.x = x;
    position.y = y;
}
void TestPlayerClass() {
    std::cout << "Player osztály tesztelése kezdődik...\n";

    // 1. Konstruktor teszt
    Player testPlayer(100, 50, 1);
    assert(testPlayer.getHealth() == 100);
    assert(testPlayer.getMoney() == 50);
    assert(testPlayer.getRoomNum() == 1);
    std::cout << "Konstruktor teszt sikeres!\n";

    // 2. Health módosítás teszt
    testPlayer.modifyHealth(-20);
    assert(testPlayer.getHealth() == 80);
    testPlayer.modifyHealth(-100);
    assert(testPlayer.getHealth() == 0); // Health nem lehet negatív
    std::cout << "Health módosítás teszt sikeres!\n";

    // 3. Money módosítás teszt
    testPlayer.modifyMoney(25);
    assert(testPlayer.getMoney() == 75);
    testPlayer.modifyMoney(-100);
    assert(testPlayer.getMoney() == -25); // Money lehet negatív
    std::cout << "Money módosítás teszt sikeres!\n";

    // 4. Room módosítás teszt
    testPlayer.modifyRoomNum(3);
    assert(testPlayer.getRoomNum() == 3);
    std::cout << "Room módosítás teszt sikeres!\n";

    // 5. Inventory teszt
    testPlayer.addItem(42);
    assert(testPlayer.hasItem(42) == true);
    testPlayer.removeItem(42);
    assert(testPlayer.hasItem(42) == false);
    std::cout << "Inventory teszt sikeres!\n";

    // 6. Pozíció teszt
    testPlayer.setPosition(200, 150);
    SDL_Rect position = testPlayer.getPosition();
    assert(position.x == 200);
    assert(position.y == 150);
    std::cout << "Pozíció teszt sikeres!\n";

    std::cout << "Minden teszt sikeresen lefutott!\n";
}
