# Tensor::Concord Card Game

## Demo

[Click here to view video](https://drive.google.com/file/d/19z3b7filhDP9yixxzw6PsRcmYFdyU7xe/view?usp=sharing)

---

- 00:00 - Compile
- 00:30 - Game Start
- 00:41 - Game 1 [LOSE]
- 00:45 - Help Menu exploring
- 03:05 - Tensor Peak [Minigame 1]
- 05:18 - Using Artifact
- 05:40 - Tensor Peak [Minigame 2]
- 06:00 - Minigame practice menu exploring
- 07:35 - Game 2 [WIN]
- 09:03 - Tensor Peak [Minigame 3]
- 10:10 - Using Artifact
- 11:11 - WIN

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

**Tensor** is the energy on the field. It is on default, zero. However, as it increases, by turn, and by playing **Tensor Shards**, it eventually peaks before restarting. Its peak is initially at 3, before it peaks. After its peak, it resets to zero then its maximum is 4. This goes until its maximum is 7.

Once it peaks, it forces both players to initiate a minigame. These are very simple games, that forces a player to take damage.

---

## Minigames

The minigames are small, simple games, that spice up the game minimally.

1. Coin Toss guessing - player guesses the outcome of a coin toss
2. High-Low - two cards, one revealed. Player guesses if next card is higher or lower
3. Simple Roulette - Player picks on simple wheel space to spin on
4. Dice Roll - Player guesses outcome of a dice roll
5. Rock-Paper-Scissors - Player plays against a simple enemy on Rock Paper Scissors

As you can see, they are simple enough to have a binary outcome.

---

## Synergy

Synergy comes as a feature of the faction system. There are four factions in this game, along with roles. Roles as of yet do not have any function, merely just to differentiate between Champions.

1. Techno
2. Cyber
3. Exec
4. Virtu-Machina

They all grant different buffs in synergy when they are present on the field. There are 3 different levels of synergy. It is as follows: two cards of the same faction create a level 1 synergy. This goes until all four cards on the field, that is the maximum space on the player's side of the field contains the same faction, in which this is a level 3 synergy, the max.

The buffs are as follows:

1. Techno --> ATK buff (+1/+2/+3)
2. Cyber --> HP buff (+1/+2/+3)
3. Exec --> Cost reduction (to other exec faction cards in hand of -1/-2/-3)
4. Virtu-Machina --> Energy buff (+1/+2/+3 per turn synergy is active) and +1 to tensor gauge per turn

Synergy buffs are active so as long as the number of cards of their factions are on the field. Here, once you put on a new card, it will check for synergy to activate. However, whenever a champion is destroyed, the check for recalculating synergy levels only come at the end of the turn.

### Perfect Synergy - Tensor Concordia

When all four factions are present on the field simultaneously, a special synergy called "Tensor Concordia" activates, granting all champions +3/+3 stats. This represents the perfect harmony of all factions working together.

### Roles

While currently not affecting gameplay directly, champions are divided into four roles that represent their background in the cyberpunk world:
- Merc: Mercenaries and hired guns
- Nomad: Independent wanderers and scouts
- Corpo: Corporate agents and executives
- Mage: Tech-wizards and data-manipulators

### Card Types Detail

#### Champions
Champions have base stats that can be enhanced through:
- Faction synergies
- Artifact buffs
- Tensor Concordia
- Initial stats range: Cost(1-3), ATK(1-3), HP(2-3)

#### Artifacts
Artifacts provide permanent buffs:
- Even-numbered effects buff HP
- Odd-numbered effects buff ATK
- Cost equals the buff amount
- Cannot be removed once applied

#### Tensor Shards
- Always cost 0 energy
- Grant 1-2 energy instantly
- Increase tensor gauge by 1
- Can chain multiple in one turn

### Tensor Gauge Mechanics

The tensor gauge has several important properties:
- Starts with maximum of 3
- Increases maximum by 1 after each peak (up to 7)
- Triggers a random minigame at peak
- Can be increased by:
  - Playing Tensor Shards
  - Virtu-Machina faction synergy
  - Natural turn progression

### Game Balance

The game is designed around key balance elements:
- Four card spaces enforce strategic placement
- Energy management between card costs and tensor gains
- Risk/reward of increasing tensor gauge
- Faction synergy vs mixed strategy decisions

---

## Development

As I planned, this project was meant to have a lot more. But due to time constraints, it will be as is. Originally, it was planned to use SDL2/SFML. However, the cross-platform proved too difficult to implement with how I develop. Later time will prove if I remake this into a GUI game.


Features as well had a lot to be cut as time was not enough to implement it all. In this case, the entire game is an instance of a simple object, `Game game`, where it lives and dies within its lifetime. Here, what remains is just 1 simple game, between the player and a simple game enemy, that is run by simple parameters that do not make the most optimized decision, but does make use of the maximal potential of the cards and field on hand.

### Difficulties

As a person who develops on Linux, Windows proved weird when running GUI components, and having to run instances of X11 graphic drivers also made it difficult to debug. However, after some time, I moved the project onto windows, and reluctantly, developed most of it with the windows API in mind.

<!--TO DO:
1. Perfect Concord
2. roles - Merc, Nomad, Corpo, Mage
-->
