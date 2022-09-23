#pragma once


enum class MOVETYPE
{
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};
MOVEPACKET player_move;

enum class PACKET_TYPE
{
    START,
    MOVE,
    END
};

typedef struct move_packet {
    short size = sizeof(move_packet);
    char move_type;
    char id;
    char packet_type = static_cast<char>(PACKET_TYPE::MOVE);
    int x;
    int y;
}MOVEPACKET;

typedef struct move_packet {
    short size = sizeof(move_packet);
    char id;
    char packet_type = static_cast<char>(PACKET_TYPE::START);
}STARTPACKET;

typedef struct move_packet {
    short size = sizeof(move_packet);
    char id;
    char packet_type = static_cast<char>(PACKET_TYPE::END);
}ENDPACKET;

