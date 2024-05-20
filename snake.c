// Copyright (c) 2020 MattKC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include <windows.h>

#define WINDOW_STYLE        WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_MAXIMIZEBOX
#define WINDOW_WIDTH        640
#define WINDOW_HEIGHT       480
#define TILE_SIZE           40
#define HORIZONTAL_TILES    WINDOW_WIDTH / TILE_SIZE
#define VERTICAL_TILES      WINDOW_HEIGHT / TILE_SIZE
#define MAX_TILE_COUNT      HORIZONTAL_TILES * VERTICAL_TILES
#define START_X             HORIZONTAL_TILES / 2
#define START_Y             VERTICAL_TILES / 2
#define SNAKE_PADDING       TILE_SIZE / 4
#define SNAKE_SIZE          TILE_SIZE - SNAKE_PADDING - SNAKE_PADDING
#define FOOD_PADDING        TILE_SIZE / 8
#define FOOD_SIZE           TILE_SIZE - FOOD_PADDING - FOOD_PADDING

#define GAME_OVER 0
#define GAME_WON 1
#define GAME_PAUSE 2


typedef struct {
    int x;
    int y;
} Position;


Position* snake_pos;
Position food_pos = {TILE_SIZE * 2 % HORIZONTAL_TILES, TILE_SIZE * 2 % VERTICAL_TILES};
size_t player_dir = 0, cur_state = GAME_PAUSE;
size_t snake_len = 1;
BOOL forgiveness = FALSE;
static unsigned long rand_seed = 1;


#define rand(max) (rand_seed = (rand_seed * 1664525U + 1013904223U) % ((unsigned long)(max)))
#define SI_TO_U64(ptr)  (*((union { void* in; u64* out; }){ptr}.out))


BOOL PosEqual(Position p1, Position p2) {
    return SI_TO_U64(&p1) == SI_TO_U64(&p2);
}

BOOL PosExists(Position p) {
    size_t i;
    for (i = 0; i < snake_len; i++) {
        if (PosEqual(snake_pos[i], p)) {
            return TRUE;
        }
    }

    return FALSE;
}

void SetFood(void) {
    do {
        food_pos.x = rand(HORIZONTAL_TILES);
        food_pos.y = rand(VERTICAL_TILES - 1);
    } while (PosExists(food_pos));
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ctx;
            HDC device = BeginPaint(hwnd, &ctx);
            HBRUSH handle = CreateSolidBrush(RGB(0, 255, 0));

            SelectObject(device, GetStockObject(NULL_PEN));
            SelectObject(device, handle);

            Position food_pos_px;
            food_pos_px.x = food_pos.x * TILE_SIZE + FOOD_PADDING;
            food_pos_px.y = food_pos.y * TILE_SIZE + FOOD_PADDING;

            Rectangle(
                device,
                food_pos_px.x,
                food_pos_px.y,
                food_pos_px.x + FOOD_SIZE,
                food_pos_px.y + FOOD_SIZE
            );
            DeleteObject(handle);

            handle = CreateSolidBrush(RGB(
                (cur_state != GAME_WON) * 255,
                (cur_state > GAME_OVER) * 255,
                (cur_state > GAME_WON) * 255
            ));
            SelectObject(device, handle);


            Position top_left;
            Position bottom_right;
            RECT draw_rect;

            size_t i;
            for (i = 0; i < snake_len; i++) {
                top_left = snake_pos[i];
                bottom_right = snake_pos[i];

                if (i > 0) {
                    top_left.x = min(top_left.x, snake_pos[i - 1].x);
                    top_left.y = min(top_left.y, snake_pos[i - 1].y);
                    bottom_right.x = max(bottom_right.x, snake_pos[i - 1].x);
                    bottom_right.y = max(bottom_right.y, snake_pos[i - 1].y);
                }

                draw_rect.left = top_left.x * TILE_SIZE + SNAKE_PADDING;
                draw_rect.top = top_left.y * TILE_SIZE + SNAKE_PADDING;
                draw_rect.right = (1 + bottom_right.x) * TILE_SIZE - SNAKE_PADDING;
                draw_rect.bottom = (1 + bottom_right.y) * TILE_SIZE - SNAKE_PADDING;

                if (top_left.x == 0 && bottom_right.x == HORIZONTAL_TILES - 1) {
                    Rectangle(
                        device,
                        0,
                        draw_rect.top,
                        TILE_SIZE - SNAKE_PADDING,
                        draw_rect.bottom
                    );

                    draw_rect.left = WINDOW_WIDTH - TILE_SIZE + SNAKE_PADDING;
                    draw_rect.right = WINDOW_WIDTH;
                }
                else if (top_left.y == 0 && bottom_right.y == VERTICAL_TILES - 2) {
                    Rectangle(
                        device,
                        draw_rect.left,
                        0,
                        draw_rect.right,
                        TILE_SIZE - SNAKE_PADDING
                    );

                    draw_rect.top = WINDOW_HEIGHT - TILE_SIZE + SNAKE_PADDING;
                    draw_rect.bottom = WINDOW_HEIGHT;
                }

                Rectangle(
                    device,
                    draw_rect.left,
                    draw_rect.top,
                    draw_rect.right,
                    draw_rect.bottom
                );
            }

            DeleteObject(handle);
            EndPaint(hwnd, &ctx);
            break;
        }
        case WM_KEYDOWN: {
            if (cur_state < GAME_PAUSE) {
                goto check_retry;
            }

            switch (wParam) {
                case VK_LEFT:
                case VK_UP:
                case VK_RIGHT:
                case VK_DOWN:
                    player_dir = wParam;
                    goto end;
                case VK_RETURN:
                    player_dir = 0;
                    cur_state = GAME_PAUSE;
                    goto end;
                default: goto end;
            }

check_retry:
            if (wParam == 'R') {
                snake_pos->x = START_X;
                snake_pos->y = START_Y;

                SetFood();
                snake_len = 1;

                player_dir = 0;
                cur_state = GAME_PAUSE;
            }
            break;
        }
        case WM_TIMER: {
            if (player_dir != cur_state
                && player_dir + 2 != cur_state
                && player_dir - 2 != cur_state) {
                cur_state = player_dir;
            }

            Position new_pos = snake_pos[0];

            switch (cur_state) {
                case VK_LEFT:
                    new_pos.x--;
                    break;
                case VK_RIGHT:
                    new_pos.x++;
                    break;
                case VK_UP:
                    new_pos.y--;
                    break;
                case VK_DOWN:
                    new_pos.y++;
                    break;
                default:
                    break;
            }

            if (new_pos.x < 0) {
                new_pos.x = HORIZONTAL_TILES - 1;
            }
            else if (new_pos.x > HORIZONTAL_TILES - 1) {
                new_pos.x = 0;
            }
            else if (new_pos.y < 0) {
                new_pos.y = VERTICAL_TILES - 2;
            }
            else if (new_pos.y > VERTICAL_TILES - 2) {
                new_pos.y = 0;
            }

            snake_len--;
            BOOL collided = PosExists(new_pos);
            snake_len++;

            if (collided) {
                if (forgiveness) {
                    player_dir = 0;
                    cur_state = GAME_OVER;
                }
                else {
                    forgiveness = TRUE;
                    goto end;
                }
            }
            else {
                BOOL ate_food = PosEqual(new_pos, food_pos);
                snake_len += ate_food;
                forgiveness = FALSE;

                size_t i;
                for (i = snake_len - 1; i > 0; i--) {
                    snake_pos[i] = snake_pos[i - 1];
                }
                snake_pos[0] = new_pos;

                if (ate_food) {
                    if (snake_len == MAX_TILE_COUNT) {
                        food_pos.x = -1;
                        player_dir = 0;
                        cur_state = GAME_WON;
                    }
                    SetFood();
                }
            }
            InvalidateRect(hwnd, 0, 1);
            break;
        }
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
    }

end:
    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}


int main(void) {
    GetSystemTimeAsFileTime((void*)&rand_seed);

    Position snake_pos_stack[MAX_TILE_COUNT];
    snake_pos = snake_pos_stack;
    snake_pos->x = START_X;
    snake_pos->y = START_Y;

    WNDCLASSA wc;
    wc.lpfnWndProc = WindowProc;
    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpszClassName = "SNAKE";
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = 0;
    wc.hIcon = 0;
    wc.lpszMenuName = 0;
    wc.hbrBackground = GetStockObject(BLACK_BRUSH);
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA(
        "SNAKE",
        "SNAKE",
        WINDOW_STYLE,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0,
        0,
        NULL,
        NULL);

    SetTimer(hwnd, 0, 150, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }

    return 0;
}
