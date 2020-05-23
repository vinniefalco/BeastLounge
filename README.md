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

## Design Goals

* Clean source file organization

* Fast build times

  - Using template heavy, header-only libraries

## CMake

    cmake -G "Visual Studio 16 2019" -A Win32 -B bin -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/msvc.cmake
    cmake -G "Visual Studio 16 2019" -A x64 -B bin64 -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/msvc.cmake
