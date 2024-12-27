# Tensor::Concord Card Game

## Build Instructions

1. Please make sure to have MinGW installed on your windows devices
2. If the machine does not have `PDCurses` then, refer to [PDCurses-master.zip](https://github.com/wmcbrine/PDCurses/archive/refs/heads/master.zip)
3. Open\Run `MSYS2 MinGW 64-bit` (or `MSYS2 MinGW 32-bit`) and cd into the `wincon` folder and run
```
make -f Makefile WIDE=Y DLL=Y
```
4. If you followed the steps above so far correctly, there should be 2 specific files inside wincon folder called `pdcurses.a` and `pdcurses.dll`

- rename `pdcurses.a` to `libpdcurses.a`
- copy `pdcurses.dll` into `C:\msys64\mingw64\bin`
- copy `libpdcurses.a` into `C:\msys64\mingw64\lib`
- copy `curses.h` and `panel.h` from `PDCurses-master` folder into `C:\msys64\mingw64\include`

> *For your convenience, I've placed those files in `.pdcurses` within this root folder. If you need those specific, important files, please copy them over to those places.*

> [!IMPORTANT]
Please contact me if it does not compile on your machine!

---

## What is Tensor::Concord?

***Tensor::Concord*** is my attempt at an original card game prototype. It is at its core, a simple card game, with a simple twist.

It takes inspiration from most TCG's, meaning its simply a PvE gameplay loop until one side reaches a win condition. Here, it simply is as imagined. A field resides in the middle with 4 spaces on both sides. You, the player can have up to 4 **Champions** on your side. A Champion is a simple mob card that has the stats,

> - Cost
> - Health --> HP
> - Attack --> ATK

### Champions

As you place down your **Champions** from your hand onto your field, they can form tensor nodes, in which when placed with other **Champions**, they can buff each other with increased *HP* or *ATK*. Your enemy can do the same.

### Artifacts

**Artifacts** can help you buff your Champions indirectly, as they have,

> - Cost
> - Buff --> HP / ATK

on the field, they are stackable, and serve to spice up the game minimally.

### Tensor Shards

**Tensor Shards** grant energy directly to the players. They are played free of cost and have no delay, meaning they can be played directly from the hand, and grant you energy. However, they also have a downside, they force the Tensor to increase on the field.

## Tensor

**Tensor** is the energy on the field. It is on default, zero. However, as it increases, by turn, and by playing **Tensor Shards**, it eventually peaks before restarting.

Once it peaks, it forces both players to initiate a game. These are very simple games, that forces a player to take damage.

---

## Minigames

The minigames are small, simple games, that spice up the game minimally.

1. Coin Toss guessing
2. High-Low
3. Simple Roulette
4. Dice Roll
5. Rock-Paper-Scissors

As you can see, they are simple enough to have a binary outcome, with a very slim chance of draws.

## Development

As I planned, this project was meant to have a lot more. But due to time constraints, it will be as is. Originally, it was planned to use SDL2/SFML. However, the cross-platform proved too difficult to implement with how I develop. Later time will prove if I remake this into a GUI game.


Features as well had a lot to be cut as time was not enough to implement it all. In this case, the entire game is an instance of a simple object, `Game game`, where it lives and dies within its lifetime. Here, what remains is just 1 simple game, between the player and a simple game enemy, that is run by simple parameters that do not make the most optimized decision, but does make use of the maximal potential of the cards and field on hand.

### Difficulties

As a person who develops on Linux, Windows proved weird when running GUI components, and having to run instances of X11 graphic drivers also made it difficult to debug. However, after some time, I moved the project onto windows, and reluctantly, developed most of it with the windows API in mind.