# 🐍 Snake Game in C.

A modern twist on the classic Snake game, built using C and the [raylib](https://www.raylib.com/) game library. This version features:

- Smooth grid-based movement
- Sound effects (`eat.wav`, `gameover.wav`)
- Level progression and speed scaling
- Persistent high score storage using a JSON file
- Pause/resume and reset controls
- Clean, grid-style interface and visuals

---

## 🚀 Features

- ✅ Keyboard controls (`WASD` or `Arrow Keys`)
- ✅ Growing snake logic
- ✅ Food spawning
- ✅ Self and wall collision detection
- ✅ Level-up every 5 points (speed increases)
- ✅ Score and high score tracking
- ✅ Persistent high score saved in `highscore.json`
- ✅ Pause with `P`, reset with `R` on game over

---

## 🎮 Controls

| Key       | Action             |
|-----------|--------------------|
| W / ↑     | Move up            |
| S / ↓     | Move down          |
| A / ←     | Move left          |
| D / →     | Move right         |
| P         | Pause/Unpause     |
| R         | Restart (if game over) |
| ESC       | Quit game          |

---

---

## 🧱 Requirements

- C Compiler (e.g., GCC or Clang)
- [raylib](https://github.com/raysan5/raylib) library installed
- `eat.wav`, `gameover.wav` sound files in the same directory
- macOS, Linux or Windows (tested on macOS M3)

---

## ⚙️ Build & Run

### 🛠 macOS / Linux:

```bash
gcc snake.c -o snake_game \
  -I/opt/homebrew/include \
  -L/opt/homebrew/lib \
  -lraylib \
  -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo
./snake_game
