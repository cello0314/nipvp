# nipvp

A plugin that turns the Co-op mode into a PvP mode for Naruto Shippuden: Ultimate Ninja Impact.

<img width="480" height="272" alt="ULUS10582_00007" src="https://github.com/user-attachments/assets/08cec14a-bc52-4ce2-9326-bee0768e009c" /><br>
<img width="480" height="272" alt="NPJH50435_00022" src="https://github.com/user-attachments/assets/62699d04-bf35-4d19-a7f3-0ad466bee556" /><br>
<img width="480" height="272" alt="NPJH50435_00024" src="https://github.com/user-attachments/assets/8cc05732-f35b-4630-850d-03ddf0767d43" />

## Features
- PvP mode.
- Removes all other enimies.
- Balanced HP and Chakra.
- Replaces spawnpoints in all maps.
- Works on both JP and USA version (`NPJH50435`/`ULUS10582`).
- Works on PSP, Vita and PPSSPP.
## Installation
- Download [nipvp.prx](https://github.com/CelloPoly314/nipvp/releases/latest/download/nipvp.prx) and copy it into `seplugins/` folder in the root of your storage.

  - For PS Vita, it should be `ux0:pspemu/seplugins/` by default.

  
- Write this line into `seplugins/game.txt`:
  ```
  ms0:/seplugins/nipvp.prx 1
  ```
  - If you're using PSP Go internal storage, write `ef0:/seplugins/nipvp.prx 1` instead.

- For PPSSPP, Download [nipvp.zip](https://github.com/CelloPoly314/nipvp/releases/latest/download/nipvp.zip) and extract the folder into `PPSSPP/PSP/PLUGINS/`

## Special Thanks
- ermaccer for his plugin template that make it possible for me to turn my cheat codes into an actual plugin.
- 南德小學生 for basically rewriting the entire plugin.
