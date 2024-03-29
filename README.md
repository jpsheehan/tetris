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
- [x] Add a timer to the HUD
- [x] Detect T-Spins for scoring
- [ ] Detect combos for scoring
- [x] Detect perfect clears for scoring
- [x] Detect lock-out (locking down above the play area)
- [ ] Fix up the input (it doesn't feel right)
  - I think it needs to accept your first input, then wait a little bit before being sensitive
  - At the moment it is too sensitive
- [x] Add a way to pause the game (think about blurring the stack)
- [x] Display a Win/Lose screen with the final score.
- [ ] Offer to let the user type their initials here (for high score if neccessary).
- [x] Add a short delay after clearing line(s)

### Fluff
- [x] Add a countdown before starting
- [x] Smooth out the preroll (we have 60 fps, let's use it!)
- [ ] Add some background graphics (maybe this could be based on the level?)
- [x] Add a program icon (and window icon)
- [x] Add a fullscreen mode (press F11)
- [x] Scale the app window when resized
- [ ] Add more information about each mode in the menu
- [ ] Add a sound effect for when rotation fails
- [ ] Add a sound effect for leveling up
- [ ] Add some special effects for when lines are cleared (perhaps even animate the lines clearing)
- [x] Upscale the UI to 640x480
- [ ] Think about using panning based on the piece position.

### Menu Stuff

- [x] Add a menu system
- [x] Add more modes:
  - [x] sprint (40 lines, leaderboard is time-based)
  - [x] marathon (complete level 15, 150 lines, leaderboard is score based)
  - [x] ultra (2 minute timed, leaderboard is score based)
  - [x] endless (leaderboard is score based)
- [x] Add pause-menu option to restart
- [x] Offer confirmations for restart and and abandon
- [ ] Add highscores for each mode
  - [ ] View the highscores before entering the mode
  - [ ] Show each "next" highscore to beat during the game
- [ ] Add options for muting sounds
- [x] Add a retry menu after finishing the game
- [x] Add a debug menu with setups for testing common patterns
  - [x] Test line clears
  - [x] Test perfect clears
  - [x] Test T-Spins
  - [x] Test T-Spin minis


### Bugs

- [x] Fix sounds dropping out from preroll... sometimes??? Usually after pausing.
  - The preroll timer was being skipped because it was counting too fast in some instances.
- [x] We appear to do too much in the input loop for the game (MIGHT NOT BE AN ISSUE ANYMORE)

### CI/CD

- [x] Get GitHub Actions creating build artifacts for download.
- [ ] Investigate using CMake instead. I might be able to get it compiling for all three major operating systems using GHA that way.


## Credits

### Audio

- Sounds Effects: https://freesound.org/people/deadrobotmusic/packs/37032/
- Melody loop 128 bpm: https://freesound.org/people/DaveJf/sounds/663759/