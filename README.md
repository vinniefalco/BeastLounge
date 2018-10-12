# BeastLounge

## Description

This is a multi-user gaming server which allows users to register and
play Blackjack against the dealer with up to 5 other players at each
table. The game is run on a server written in C++ using Boost.Beast,
and a client written in JavaScript which runs in the browser and is
delivered by the game server.

The WebSocket protocol is used to communicate between the client and
server. This allows for full-duplex unsolicited messaging. WebSocket
is built-in to JavaScript and a natural choice. Each message contains
in the payload a complete JSON-RPC object as described here:
https://www.jsonrpc.org/specification

A lobby area offers multiple public and private chat rooms similar to
IRC where users can meet, add each other to their friends list,
send direct messages (whispers), and join games together. The lobby
lists games in progress or games starting and gives users the option
to join.

## Authentication API

    - Don't store passwords
    - https://openwall.info/wiki/people/solar/algorithms/challenge-response-authentication

## Persistence

    - Abstract storage interface, domain-specific
    - In-memory implementation (no persistence)
    - sqlite implementation

JSON-RPC commands:

Sent by the client:

    ListRooms

    JoinRoom

    LeaveRoom

    GetRoomInfo   # Show users in the room

    SayToRoom

WhisperUser

Sent by the server:

    JoinRoom (response)

    LeaveRoom (response)

    SayFromRoom

    WhisperFromUser
