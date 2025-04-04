# ALSD 2 Mini Project

## 📝 Description
A basic word puzzle game prototype where players form valid words from given letters to progress through levels.

### Core Mechanics:
- 10 levels with increasing difficulty
- Words must be 2-6 letters long
- Dictionary validation using a provided word list
- Simple user account system

---

## 📦 Folder Structure
```plaintext
word_game_prototype/
├── main.c                # Main game code
├── words.txt             # Dictionary word list
├── raygui.h              # UI library header
├── raylib.h              # Game library header
├── libraylib.a           # Raylib static library
└── README.md             # This file
```

---

## ⚙️ Installation & Setup
### Prerequisites
- Linux system (tested on Ubuntu)
- Basic build tools (`gcc`, `make`)
- X11 development libraries

### Build Instructions
#### Download required files:
- [Raylib](https://www.raylib.com/)
- [RayGUI](https://github.com/raysan5/raygui)

#### Compile the game:
```bash
gcc main.c -o main -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

#### Run the game:
```bash
./main
```

---

## 🎮 How to Play
### Start Screen
- Register a new account or log in
- Usernames are **case-sensitive**

### Gameplay
- Each level provides a set of letters
- Form words that:
  - Use only the provided letters
  - Match the required lengths
  - Are in the dictionary
- Complete all required words to advance

### Level Progression
- 10 total levels
- Progress saves automatically

---

## 🔧 Technical Details
### Libraries Used:
- **Raylib 4.5** (graphics/input)
- **RayGUI 3.2** (UI components)

### Key Functions:
- Dictionary loading/validation
- Level configuration
- User data persistence

### Limitations:
- No error handling for missing files
- Fixed window size (800x600)
- Basic UI without animations

---

## 📌 Notes
- This is a prototype (**v0.0.0**) for educational purposes.
- No license - for **ALSD 2** course use only.
- The dictionary (`words.txt`) can be replaced with custom word lists.
- All game assets are included in the repository.

  
## **‼️You can build the the project if you don't want to compile it alone, It's Ctrl + Shift + b in Vscode ‼️**

### Developed for:
**Advanced Linux System Development (ALSD) Course**  
**Computer Engineering Department**  
**University of Bejaia**

---


## 🛠️ Prototype by **Hicham IDDIR** - **[S2/2025]**

### 🛑 Report Issues
If you encounter any bugs or errors, please open an issue on this repository or contact the developer.

---
