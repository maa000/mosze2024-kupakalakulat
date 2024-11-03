#include "Map.h"
#include <cstdlib>  // Véletlenszám-generáláshoz
#include <ctime>    // Az idő beállítása a véletlenszám-generáláshoz

Map::Map() {
    // Inicializálja a random seedet (csak egyszer kell meghívni a program elején)
    std::srand(static_cast<unsigned int>(std::time(0)));
}

void Map::generateRandomMap() {
    rooms.clear(); // Törli az előző térképet, ha volt

    // Példa: 10 szoba véletlenszerű típusú szobákkal
    int numRooms = 10;
    for (int i = 0; i < numRooms; ++i) {
        Room::Type roomType;

        // Véletlenszerű szobatípus választása
        int randomType = std::rand() % 4;
        switch (randomType) {
            case 0:
                roomType = Room::Type::EnemyCombat;
                break;
            case 1:
                roomType = Room::Type::Shop;
                break;
            case 2:
                roomType = Room::Type::Heal;
                break;
            case 3:
                roomType = Room::Type::RandomEvent;
                break;
        }

        rooms.emplace_back(roomType); // Szoba hozzáadása a listához
    }
}

const std::vector<Room>& Map::getRooms() const {
    return rooms;
}
