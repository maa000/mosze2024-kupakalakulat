#ifndef MAP_H
#define MAP_H

#include <vector>
#include "Room.h"

class Map {
public:
    Map();
    
    // Térkép generálása
    void generateRandomMap();

    // Szobák elérése (opcionális, ha később szükség lenne rá)
    const std::vector<Room>& getRooms() const;

private:
    std::vector<Room> rooms; // A generált szobák listája
};

#endif // MAP_H
