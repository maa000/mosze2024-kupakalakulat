#include "Room.h"

Room::Room(Type type) : type(type) {}

Room::Type Room::getType() const {
    return type;
}
