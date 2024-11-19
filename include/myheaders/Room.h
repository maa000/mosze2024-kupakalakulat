#ifndef ROOM_H
#define ROOM_H

class Room {
public:
    enum class Type {
        EnemyCombat,
        Shop,
        Heal,
        RandomEvent
    };

    Room(Type type);
    
    Type getType() const;

private:
    Type type;
};

#endif // ROOM_H
