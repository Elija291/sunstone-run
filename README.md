# Sunstone Run
A native full-color C/SDL2 endless runner for Kali or Debian. This standalone desktop game uses a perspective road and an original pixel-art explorer. Run across Palm River, Amber Ruins and Jade Pass. Every obstacle row has an open lane. Speed increases with distance.

## Launch
From this folder run `make play`. The build uses a C compiler, Make and the SDL2 development package. If needed, install them with `sudo apt install build-essential libsdl2-dev`. Use `make test` to run the simulation checks.

## Controls
- Enter starts a run or retries after a crash.
- Left and Right (or A and D) change lanes.
- Up, W or Space jumps crates and broken paving.
- Down or S slides under low vine gates.
- P pauses or resumes. Esc exits.

Collect spinning gold tokens by crossing their lane. The HUD tracks metres and gold independently. Best distance lasts for the current session. The game has no external asset downloads.
