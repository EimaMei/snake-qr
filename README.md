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

# Primary changes
To see the differences between the original and this version first-hand, it's best to view [the original file](https://www.mattkc.com/etc/snakeqr/snake.c) and see what's different from a programming perspective. Regardless, here is a list of the main changes between the two:

## Space saving changes
- The input system has been reworked to utilize only 2 variables instead of using an array of valid/invalid inputs. This rework reduces quite a considerable amount of taken up space inside the executable, as the game now does less interanally for the same result. 
- The food starts at a constant position instead of calling the `rand` function at startup.
- All integers are `size_t`.
- A custom `rand` is implemented instead of relying on the C standard version.
- Code to change the snake's color depending on the game state (playing/won/lost) has been reworked to be completely branchless.
- `WM_TIMER`'s `while` loop has been converted to a single `if` statement.
- Removed the `bool moved` variable in `WM_TIMER` and used `goto` instead.
- Removed the `else` statement at `if (snake_len == MAX_TILE_COUNT)`.
- Removed `GetModuleHandleA` and `AdjustWindowRect`.
- The app now uses allocates memory into the stack instead of the heap to allocate `snake_pos`.
- Removed `if (msg.message == WM_QUIT)` in the main loop.
- Removed `TranslateMessage` in the main loop.

## Bug fixes
- The original `snake.c` leaked memory by continuously not doing `DeleteObject` when calling `CreateSolidBrush`. [It's even stated in the official Win32 documentation that you *should* use `DeleteObject` once you're done with the brush](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-createsolidbrush). This bug fix does come with a caveat, that being it takes up a bit of space in the executable for it to be implemented. 

## Visual changes
- The apple is now green.

## Misc changes
- You can now retry the game at anytime by pressing 'R'.
- The game's performance has been improved (less branching, `PosEqual` does one check instead of 2, etc).
- The pause button has been changed from `VK_PAUSE` and `P` to `VK_ENTER`.
- Regular `WNDCLASSA` and `CreateWindowA` are used instead of the expanded versions.
- The game doesn't overallocate `dir_queue` like it did in the original.

# Building
NOTE: `cl.exe` is assumed to be x86, not x64! Compiling it in 64-bit will considerably increase the size of the binary.

## Windows
Just copy the terminal commands from `Makefile` and paste them into the terminal.

## Linux/*nix OSses
This requires downloading `msvc` from [msvc-wine](https://github.com/mstorsjo/msvc-wine). After that, just run `make` and you'll get correct binaries.

# Making the QR code
Once you compiled `snake.exe`, on Unix you can run `make qrcode` to generate `code.png`.
