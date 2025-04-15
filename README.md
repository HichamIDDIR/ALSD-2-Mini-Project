# ALSD 2 Mini Project

## 📝 Description
A basic word puzzle game prototype where players form valid words from given letters to progress through levels.


**IF YOU ARE ON WINDOWS JUST RUN  THE .exe FILE, OR THE .bat file to build and run.**

### Core Mechanics:
- 10 levels with increasing difficulty
- Words must be 2-6 letters long
- Dictionary validation using a provided word list
- Simple user account system

---

## 📦 Folder Structure
```plaintext
LSD-2-Mini-Project-/
├── linux/                              # Linux-specific files
│   ├── .vscode/                        # VSCode configuration
│   ├── raygui-master/                  # Raygui source directory
│   ├── raylib-5.5_linux_amd64/         # Raylib binaries directory
│   ├── librarylib.a                    # Raylib static library
│   ├── main                            # Compiled Linux executable
│   ├── main.c                          # Main game code
│   ├── raygui.h                        # UI library header
│   ├── raylib.h                        # Game library header
│   └── words.txt                       # Dictionary word list
├── Windows/                            # Windows-specific files (updated)
│   ├── compiler/                       # MinGW-w64 GCC 13.2.0 (from winlibs)
│   │   └── bin/
│   │       ├── gcc.exe                 # C compiler
│   │       ├── g++.exe                 # C++ compiler (if needed)
│   │       ├── ar.exe                  # Archiver
│   │       └── libwinpthread-1.dll     # Required DLL
│   │   └── ...                         # Other necessary files (lib/, include/, etc.)
│   ├── include/                        # Raylib + RayGUI headers
│   │   ├── raylib.h
│   │   └── raygui.h
│   ├── libs/                           # Static libraries
│   │   └── libraylib.a
│   ├── src/                            # Game source code
│   │   └── main.c
│   ├── words.txt                       # Dictionary file
│   ├── game.exe                        # Compiled output
│   ├── build.bat                       # One-click build script
│   └── users/                          # Created at runtime
│       └── *.dat                       # User save files
├── .git/                               # Git repository data
├── .gitignore                          # Git ignore file
└── README.md                           # Project documentation

```

---

## ⚙️ Installation & Setup

### Prerequisites:
- Linux system (tested on Fedora)
- Basic build tools: `gcc`, `make`
- X11 development libraries  
  _Install required packages (Fedora):_
```bash
sudo dnf install gcc make libX11-devel
```
- in windows everything is portable



#### Compile & Run the game:
##### For linux :
```bash
cd ALSD-2-Mini-Project\
gcc -o game main.c libraylib.a -lm -lpthread -ldl -lGL -lX11
./game
```
##### For windows : 
```plaintext
Just run the build.bat file.
```


---

## 🎮 How to Play
- **User Setup:** Register a new account or log in (usernames are case-sensitive)
- **Gameplay:** Each level provides a set of letters to form words that:
  - Use only the provided letters
  - Match the required lengths (2-6 letters)
  - Are in the dictionary  
  Complete all required words to advance through 10 levels. Progress saves automatically.

---

## 🔧 Technical Details
- **Libraries:** Raylib 4.5 (graphics/input), RayGUI 3.2 (UI components)
- **Core Functions:** Dictionary loading/validation, level configuration, and user data persistence  
  _Note: The dictionary file (`words.txt`) may take a long time to load due to its size._
- **Limitations:** No error handling for missing files, fixed window size (800x600), basic UI without animations

---

## 📌 Notes:
- Prototype (v0.0.1) for educational purposes.
- No license - for **ALSD 2 (Algorithm et Structure des Données)** course use only.
- The dictionary (`words.txt`) can be replaced with custom word lists.
- All game assets are included in the repository.

### Developed for:
- Algorithm et Structure des Données (ALSD) Course
- Computer Engineering Department
- University of Bejaia

---

## 🛑 Report Issues
If you encounter any bugs or errors, please open an issue on this repository or contact the developer.
