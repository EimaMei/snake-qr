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
- You can pause the game now by pressing the enter key instead of the pause or win32 0x50 key.

# Main coding changes
You can view the original [snake.c](https://www.mattkc.com/etc/snakeqr/snake.c) and compare between this and the new one, however here are compiled lists of the changes via categories:

## Space saving changes
- A custom `rand` is implemented instead of relying on the C standard version.
- "SNAKE" is stored as an array of 5 characters, not as a pointer.
- The food starts at the X and Y position of (`TILE_SIZE` * 2 % `HORIZONTAL_TILES`), (`TILE_SIZE` * 2 % `VERTICAL_TILES`) instead of it being random during startup.
- For most global variables that don't need to be signed are now `size_t` instead of `int`.
- Removed the `bool moved` variable in `WM_TIMER` and used `goto` instead.
- Removed the `else` statement at `if (snake_len == MAX_TILE_COUNT)`.
- Removed `GetModuleHandleA` and `AdjustWindowRect`.
- Used the stack to allocate `snake_pos` and `dir_queue`, not the heap (meaning `GetProcessHeap`, 2x `HeapAlloc` and `HeapFree` are removed).
- Removed `if (msg.message == WM_QUIT)` in the main loop.

## Bug fixes
NOTE: These bugs aren't present in the HTML version, only in `snake.c`.
- During startup, `rand_seed` is set by `GetSystemTimeAsFileTime((void*)&rand_seed)`, equivalent to doing `srand(time(0))`. This guarantees that the food position will be at least somewhat randomized during each new session of gameplay, which isn't the case in `snake.c`. However this increase the size of the executable a bit.
- `SetFood` sometimes generates the Y position to be at the very last vertical tile (tile 12, pos Y being 480), making the food barely visible apart from its top. This was fixed by limiting the maximum count to 11.
- When the head of the snake goes from top to bottom (Y pos 0 to 480), it is now able to snake around at the very bottom, not even visible anymore. You can also just stay in the bottom for long you want. This is fixed by making the collision detection more tight and teleporting the snake a tile more down when crossing the screen.

## Visual changes
- The apple is now always set to be green.

## Misc changes
- Performance has been generally improved (less branching, `PosEqual` does one check instead of 2).
- The pause button has been changed from `VK_PAUSE` and `0x50` to `VK_ENTER`.
- Regular `WNDCLASSA` and `CreateWindowA` are used instead of the expanded versions.
- The game doesn't overallocate `dir_queue` like it did in the original:
```c
int* dir_queue;
// original snake.c (NOTE: sizeof(Position) == 8, sizeof(*dir_queue) == 4)
dir_queue = HeapAlloc(heap, 0, sizeof(struct Position)*MAX_DIR_QUEUE);

// new snake.c
int dir_queue_stack[MAX_DIR_QUEUE];
// ...
dir_queue = dir_queue_stack;
```

# Building
NOTE: `cl.exe` is assumed to be x86, not x64! Compiling it in 64-bit will considerably increase the size of the binary.

## Windows
Just copy the terminal commands from `Makefile` and paste them into the terminal.

## Linux/*nix OSses
This requires downloading `msvc` from [msvc-wine](https://github.com/mstorsjo/msvc-wine). After that, just run `make` and you'll get correct binaries.

# Making the QR code
Once you compiled `snake.exe`, on Unix you can run `make qrcode` to generate `code.png`.
