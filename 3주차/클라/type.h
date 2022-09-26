#pragma once
enum class MOVETYPE
{
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};


enum class PACKET_TYPE
{
    START,
    MOVE,
    END
};

typedef struct move_packet {
    short size ;
    char type;
    char id;
    char packet_type = static_cast<char>(PACKET_TYPE::MOVE);
    int x;
    int y;
}MOVEPACKET;

typedef struct start_packet {
    short size;
    char id;
    char packet_type = static_cast<char>(PACKET_TYPE::START);
}STARTPACKET;

typedef struct end_packet {
    short size;
    char id;
    char packet_type = static_cast<char>(PACKET_TYPE::END);
}ENDPACKET;

