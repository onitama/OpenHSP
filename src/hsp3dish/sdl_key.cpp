#include "sdl_key.h"
#include "../hsp3/hsp3code.h"

// onkey uses the same Windows virtual-key numbers as getkey. SDL key symbols
// supply unshifted characters; text/IME input still goes through SDL_TEXTINPUT.
static int hsp_sdl_virtual_key(SDL_Keycode key)
{
    if (key >= SDLK_a && key <= SDLK_z) return 'A' + key - SDLK_a;
    if (key >= SDLK_0 && key <= SDLK_9) return key;
    if (key >= SDLK_F1 && key <= SDLK_F12) return 112 + key - SDLK_F1;
    if (key >= SDLK_F13 && key <= SDLK_F24) return 124 + key - SDLK_F13;
    if (key >= SDLK_KP_1 && key <= SDLK_KP_9) return 97 + key - SDLK_KP_1;
    switch (key) {
    case SDLK_BACKSPACE: return 8;
    case SDLK_TAB: return 9;
    case SDLK_CLEAR: return 12;
    case SDLK_RETURN: case SDLK_KP_ENTER: return 13;
    case SDLK_LSHIFT: case SDLK_RSHIFT: return 16;
    case SDLK_LCTRL: case SDLK_RCTRL: return 17;
    case SDLK_LALT: case SDLK_RALT: return 18;
    case SDLK_PAUSE: return 19;
    case SDLK_CAPSLOCK: return 20;
    case SDLK_ESCAPE: return 27;
    case SDLK_SPACE: return 32;
    case SDLK_PAGEUP: return 33;
    case SDLK_PAGEDOWN: return 34;
    case SDLK_END: return 35;
    case SDLK_HOME: return 36;
    case SDLK_LEFT: return 37;
    case SDLK_UP: return 38;
    case SDLK_RIGHT: return 39;
    case SDLK_DOWN: return 40;
    case SDLK_PRINTSCREEN: return 44;
    case SDLK_INSERT: return 45;
    case SDLK_DELETE: return 46;
    case SDLK_LGUI: return 91;
    case SDLK_RGUI: return 92;
    case SDLK_APPLICATION: return 93;
    case SDLK_KP_0: return 96;
    case SDLK_KP_MULTIPLY: return 106;
    case SDLK_KP_PLUS: return 107;
    case SDLK_KP_MINUS: return 109;
    case SDLK_KP_PERIOD: return 110;
    case SDLK_KP_DIVIDE: return 111;
    case SDLK_NUMLOCKCLEAR: return 144;
    case SDLK_SCROLLLOCK: return 145;
    case SDLK_SEMICOLON: return 186;
    case SDLK_EQUALS: return 187;
    case SDLK_COMMA: return 188;
    case SDLK_MINUS: return 189;
    case SDLK_PERIOD: return 190;
    case SDLK_SLASH: return 191;
    case SDLK_BACKQUOTE: return 192;
    case SDLK_LEFTBRACKET: return 219;
    case SDLK_BACKSLASH: return 220;
    case SDLK_RIGHTBRACKET: return 221;
    case SDLK_QUOTE: return 222;
    default: return 0;
    }
}

void hsp_sdl_send_key_irq(const SDL_KeyboardEvent& event)
{
    if (!code_isirq(HSPIRQ_ONKEY)) return;
    const int key = hsp_sdl_virtual_key(event.keysym.sym);
    if (key == 0) return;
    int character = event.keysym.sym;
    if (character >= SDLK_a && character <= SDLK_z) character += 'A' - 'a';
    if (character < 0 || character > 127 || character == SDLK_DELETE) character = 0;
    if (event.keysym.sym == SDLK_KP_ENTER) character = 13;
    if (key >= 96 && key <= 105) character = '0' + key - 96;
    switch (event.keysym.sym) {
    case SDLK_KP_MULTIPLY: character = '*'; break;
    case SDLK_KP_PLUS: character = '+'; break;
    case SDLK_KP_MINUS: character = '-'; break;
    case SDLK_KP_PERIOD: character = '.'; break;
    case SDLK_KP_DIVIDE: character = '/'; break;
    default: break;
    }
    // There is no Windows message on SDL: repeat count=1, SDL scancode in
    // bits 16..24, previous-key-state in bit 30. Key-up does not send onkey.
    int details = 1 | (int(event.keysym.scancode) << 16);
    if (event.repeat) details |= (1 << 30);
    code_sendirq(HSPIRQ_ONKEY, character, key, details);
}
