# Snake QR
<p align="center">
    <img src="images/code.png?raw=true" alt="QR code" style="width:60%">
    <br/>A simple and pure win32 Snake game, without any compressor trickery, just under 2,953 bytes!<br/>
    <br/>
</p>

# Context/Credit
The original source code was taken from MattKC's ["Can you fit a whole game into a QR code?" video](https://www.youtube.com/watch?v=ExwqNreocpg&ab_channel=MattKC) where he attempted to port Snake into a single QR code using the Win32 API in C. Despite his efforts he failed and decided to use a compressor to complete the challange, which felt very lame and lazy to me – so much so that I took a glance at the source code to see if there were any optimizations to make. my suspicions were proven correct as I was not only able to fit the program into the QR code treshold without changing the fundamental logic, but I also implememted some fixes and improvements compared to the original.

# Most notable changes
- No crinker/compressor is used to reduce the executable size. Only Win32 C code and some MSVC compiler flag shenanigans.
- A few visual and gameplay fixes to make it much closer to the [HTML version](https://www.mattkc.com/etc/snakeqr/snake-minified.html) that was shown in the video.
- You can pause the game now by pressing the enter key instead of the 'pause' or 'P' key.
- You can retry the game by pressing the 'R' key.

# Code/smaller changes
To see all of the differences it's best to view [the original](https://www.mattkc.com/etc/snakeqr/snake.c) and this repository's snake.c side by side and compare the minor adjustments that add up. Here is a compiled list of the changes:

## Space saving changes
- The input system has been reworked to utilize only 2 variables instead of using an array of valid/invalid inputs. This reduces quite a considerable amount of space inside the executable, as the game now does a whole less for the same result. 
- The food starts at a constant position instead of calling the `rand` function at startup.
- All integers are `size_t` (register-sized instructions are usually preffered in most cases).
- A custom `rand` is implemented instead of relying on the C standard version.
- Code to change the snake's color depending on the game state (playing/won/lost) has been reworked to be completely branchless.
- `WM_TIMER`'s `while` loop has been converted to a single `if` statement.
- Removed the `bool moved` variable in `WM_TIMER` and used `goto` instead.
- Removed the `else` statement at `if (snake_len == MAX_TILE_COUNT)`.
- Removed `GetModuleHandleA` and `AdjustWindowRect`.
- The app now uses stack memory instead of the heap to hold `snake_pos` (this reduces the amount of OS calls needed to be called).
- Removed `if (msg.message == WM_QUIT)` in the main loop.
- Removed `TranslateMessage` in the main loop.

## Bug fixes
- The original `snake.c` leaked memory by continuously not calling `DeleteObject` when calling `CreateSolidBrush` despite [official documentation advising to cleanup after use](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-createsolidbrush). The only caveat is that this unfortunately comes with the cost of additional binary space.

## Visual changes
- The apple is now green.

## Misc changes
- You can now retry the game at anytime by pressing 'R'.
- The game's performance has been improved (less branching, `PosEqual` does one check instead of 2, etc).
- The pause button has been changed from `VK_PAUSE` and `P` to `VK_ENTER`.
- Regular `WNDCLASSA` and `CreateWindowA` are used instead of the expanded versions.
- The game doesn't overallocate `dir_queue` like it did in the original.

# Building
NOTE: `cl.exe` is assumed to be x86, not x64. Compiling it in 64-bit will considerably increase the size of the binary.

## Windows
Just copy the terminal commands from `Makefile` and paste them into the terminal.

## Linux/*nix OSses
This requires downloading `msvc` from [msvc-wine](https://github.com/mstorsjo/msvc-wine). After that, just run `make` and you'll get correct binaries.

# Making the QR code
Once you compiled `snake.exe`, on Unix you can run `make qrcode` to generate `code.png`.
