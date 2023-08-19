# Snake QR
<p align="center">
    <img src="images/code.png?raw=true" alt="QR code" style="width:60%">
    <br/>A simple and pure win32 Snake game, without any compressor trickery, just under 2,953 bytes!<br/>
    <br/>
</p>

# Contenxt/Credit
The original source code was taken from MattKC's "Can you fit a whole game into a QR code?" video. If you haven't watched the video prior, [watch it here](https://www.youtube.com/watch?v=ExwqNreocpg&ab_channel=MattKC) for context and what not.

# Main changes compared to the MattKC version
- No crinker/compressor is used. Just pure C and compiler shenanigans.
- A few visual and gameplay fixes to make it much closer to the [HTML version](https://www.mattkc.com/etc/snakeqr/snake-minified.html) that was shown in the video.
- You can pause the game now by pressing the enter key instead of the pause or 'P' key.
- You can retry the game by pressing the 'R' key.

# Main coding changes
You can view the original [snake.c](https://www.mattkc.com/etc/snakeqr/snake.c) and compare between this and the new one, however here are compiled lists of the changes via categories:

## Space saving changes
- The input system is reworked to utilize only 2 variables (`player_dir` and the previous valid input, `cur_state`) instead of using an array of valid/invalid inputs. This rework improves the space by a lot as the program does less for the same amount.
- The food starts at the X and Y position of (`TILE_SIZE` * 2 % `HORIZONTAL_TILES`), (`TILE_SIZE` * 2 % `VERTICAL_TILES`) instead of it being random during startup.
- All integers are `size_t`.
- A custom `rand` is implemented instead of relying on the C standard version.
- The line of code to set the color of the snake depending on the game state (playing/paused/end/won) has been reworked to work completely branchless, saving a few bytes.
- `WM_KEYDOWN` is faster and does less by the vertue of not having to do `% MAX_DIR_QUEUE` and incrementing `dir_queue_sz`.
- When getting the valdi input inside `WM_TIMER`, all it does is a simple `if` statement instead of using the old (and to be frank, confusing and weird) `while` system of the original.
- Removed the `bool moved` variable in `WM_TIMER` and used `goto` instead.
- Removed the `else` statement at `if (snake_len == MAX_TILE_COUNT)`.
- Removed `GetModuleHandleA` and `AdjustWindowRect`.
- The program uses the stack now to allocate `snake_pos`, not the heap (meaning `GetProcessHeap`, 2x `HeapAlloc` and `HeapFree` are removed).
- Because of `dir_queue` not existing, a few bytes are also saved.
- Removed `if (msg.message == WM_QUIT)` in the main loop.
- Removed `TranslateMessage` in the main loop.

## Bug fixes
- The original `snake.c` leaked memory by continuously not doing `DeleteObject` when executing `CreateSolidBrush`. It's even stated in the documentation that you *should* use `DeleteObject` once you're done with the brush. However doing this requires *quite* a bit of space, however it is possible after some tinkering.

## Visual changes
- The apple is now always set to be green.

## Misc changes
- You can now retry the game at anytime by pressing 'R'.
- Performance has been generally improved (less branching, `PosEqual` does one check instead of 2, etc).
- The pause button has been changed from `VK_PAUSE` and `P` to `VK_ENTER`.
- Regular `WNDCLASSA` and `CreateWindowA` are used instead of the expanded versions.
- The game doesn't overallocate `dir_queue` like it did in the original:

# Building
NOTE: `cl.exe` is assumed to be x86, not x64! Compiling it in 64-bit will considerably increase the size of the binary.

## Windows
Just copy the terminal commands from `Makefile` and paste them into the terminal.

## Linux/*nix OSses
This requires downloading `msvc` from [msvc-wine](https://github.com/mstorsjo/msvc-wine). After that, just run `make` and you'll get correct binaries.

# Making the QR code
Once you compiled `snake.exe`, on Unix you can run `make qrcode` to generate `code.png`.
