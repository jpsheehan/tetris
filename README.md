# Tetris

A simple implementation of the popular falling block game.

![a screenshot of tetris](./docs/screenshot.png)

## Prerequisites

### Debian

```bash
sudo apt install liballegro5-dev make
```

### Windows

- Visual Studio. All dependencies should be automatically restored as needed by NuGet.

## Tasks

### Gameplay

- [x] Restore the "remaining lines" text
- [x] Add a "lines cleared" text
- [x] Add a locking delay for soft locking
- [ ] Add a timer to the HUD
- [ ] Detect T-Spins for scoring
- [ ] Detect combos for scoring
- [x] Detect lock-out (locking down above the play area)
- [ ] Fix up the input (it doesn't feel right)
  - I think it needs to accept your first input, then wait a little bit before being sensitive
  - At the moment it is just sensitive
- [x] Add a way to pause the game (think about blurring the stack)

### Fluff
- [x] Add a countdown before starting
- [ ] Add some background graphics (maybe this could be based on the level?)
- [ ] Add a sound effect for when rotation fails
- [ ] Add a sound effect for leveling up
- [ ] Add some special effects for when lines are cleared (perhaps even animate the lines clearing)

### Menu Stuff

- [x] Add a menu system
- [ ] Add more modes:
  - [ ] sprint
  - [ ] marathon
  - [ ] ultra
  - [ ] endless
- [ ] Add highscores for each mode
- [ ] Add options for muting sounds

### CI/CD

- [x] Get GitHub Actions creating build artifacts for download.
- [ ] Investigate using CMake instead. I might be able to get it compiling for all three major operating systems using GHA that way.


## Credits

### Audio

- Sounds Effects: https://freesound.org/people/deadrobotmusic/packs/37032/
- Melody loop 128 bpm: https://freesound.org/people/DaveJf/sounds/663759/