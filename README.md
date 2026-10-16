# 🚗 Firewall Racer

A simple 2D car game written in **C** using the [raylib](https://www.raylib.com/) graphics library. Drive down a 3-lane road, dodge cyber threats, and collect security patches to protect your car.

Made as a first-semester **Bachelor of Cyber Security** college project.

## 📸 Screenshots

| Start menu | Gameplay | Game over |
|:---:|:---:|:---:|
| ![Start menu](menu.png) | ![Gameplay](screenshots/gameplay.png) | ![Game over](screenshots/gameover.png) |

## 🎮 About the game

You drive a blue car down a road with three lanes. Cyber threats fall from the top of the screen. Switch lanes to dodge them. Every threat you dodge gives you 1 point, and the game gets faster as your score grows. If you hit a threat, the game is over.

| Item | Looks like | What it does |
|---|---|---|
| **VIR** (virus) | Red box | Hit it and the game ends |
| **MAL** (malware) | Purple box | Hit it and the game ends |
| **PHI** (phishing) | Orange box | Hit it and the game ends |
| **PAT** (security patch) | Green circle | Gives you a shield that blocks one hit |

### Features
- Start menu, game screen and game over screen
- 3 lanes with smooth car movement
- Random falling obstacles
- Score, speed level and high score
- Shield power-up (a blue ring appears around the car)
- Speed increases every 5 points, up to a limit
- Car and obstacles are drawn with simple shapes, so no image files are needed

## 🕹️ Controls

| Key | Action |
|---|---|
| `A` or `←` | Move left |
| `D` or `→` | Move right |
| `ENTER` | Start the game |
| `R` | Retry after game over |
| `Q` | Back to the menu |
| `ESC` | Exit |

## 📁 Project files

```
firewall-racer/
├── firewall_racer_raylib.c   # the whole game
├── screenshots/              # images used in this README
└── README.md
```

## 🛠️ Requirements

- Windows 10 or 11
- Visual Studio (2022 or later) with the **Desktop development with C++** workload
- The **raylib** NuGet package (installed in the steps below)

## ▶️ How to clone and run

### 1. Clone the repository

Using Git:

```bash
git clone https://github.com/Anisha-8/firewall-racer.git
```

No Git? On the repository page click **Code → Download ZIP**, then unzip it.

### 2. Create a Visual Studio project

1. Open Visual Studio and choose **Create a new project**.
2. Select **Console App** with the **C++** language tag (not the C# one), then click **Next**.
3. Name the project `FirewallRacer` and click **Create**.

### 3. Add the game file

1. In **Solution Explorer**, right-click the default `.cpp` file, choose **Remove**, then **Delete**.
2. Right-click **Source Files** and choose **Add → Existing Item**.
3. Select `firewall_racer_raylib.c` from the folder you cloned.

The file ends in `.c`, so Visual Studio compiles it as C.

### 4. Install raylib

1. Right-click the project and choose **Manage NuGet Packages**.
2. Open the **Browse** tab, search for **raylib** and click **Install**.

### 5. Run

1. Set the platform at the top of Visual Studio to **x64**.
2. Press **Ctrl + F5** to build and run.

The game window will open on the start menu. Press `ENTER` to play.

## 🧩 How the code is organised

| Function | What it does |
|---|---|
| `main()` | Opens the window and runs the game loop: update, then draw |
| `initGame()` | Resets score, speed, shield and obstacles for a new game |
| `handleInput()` | Reads `A` / `D` (or the arrow keys) and changes lane |
| `updateObstacles()` | Slides the car, moves items down and spawns new ones |
| `checkCollision()` | Handles hits, patches and the shield |
| `drawScreen()` | Draws the road, obstacles, car and score bar |
| `drawCar()` | Draws the car from simple shapes |
| `showGameOver()` | Shows the game over screen with score and high score |

## ⚙️ Easy things to change

| To change | Edit |
|---|---|
| Starting speed | `START_SPEED` at the top of the file |
| Maximum speed | `MAX_SPEED` |
| Number of items on screen | `MAX_OBS` |
| How often patches appear | `t = GetRandomValue(0, 5);` in `updateObstacles()` |
| Points per dodge | `score++;` in `updateObstacles()` |
| Colours | The colour names in `drawCar()` and `drawScreen()` |

## ❓ Troubleshooting

| Problem | Fix |
|---|---|
| `Cannot open include file 'raylib.h'` | Install the **raylib** NuGet package (step 4) |
| Linker errors | Set the platform to **x64** |
| `Program.cs` or a C# project appears | You created a C# project. Create a **C++** Console App instead |
| The window closes straight away | Run with **Ctrl + F5**, not F5 |

## 👤 Author

[Anisha-8](https://github.com/Anisha-8)
