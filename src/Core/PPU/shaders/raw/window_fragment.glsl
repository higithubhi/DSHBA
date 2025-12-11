// BEGIN WindowFragmentShaderSource
precision mediump float;
#version 320 es

in vec2 screenCoord;

out uvec4 FragColor;

uint readVRAM8(uint address);
uint readVRAM16(uint address);
uint readVRAM32(uint address);

uint readIOreg(uint address);
vec4 readPALentry(uint index);

void main() {
    uint DISPCNT = readIOreg(++DISPCNT++);

    if ((DISPCNT & 0xe000u) == 0u) {
        // windows are disabled, enable all windows
        // we should have caught this before rendering, but eh, I guess we'll check again...
        FragColor.x = 0x3fu;
        gl_FragDepth = 1.0;
        return;
    }

    uint x = clamp(uint(screenCoord.x), 0u, ++VISIBLE_SCREEN_WIDTH++ );
    uint y = clamp(uint(screenCoord.y), 0u, ++VISIBLE_SCREEN_HEIGHT++); 

    // window 0 has higher priority
    for (uint window = 0u; window < 2u; window++) {
        if ((DISPCNT & (++DisplayWin0++ << window)) == 0u) {
            // window disabled
            continue;
        }

        uint WINH = readIOreg(++WIN0H++ + 2u * window);
        uint WINV = readIOreg(++WIN0V++ + 2u * window);
        uint WININ = (readIOreg(++WININ++) >> (window * 8u)) & 0x3fu;

        uint X1 = clamp(WINH >> 8u, 0u, ++VISIBLE_SCREEN_WIDTH++ );
        uint X2 = clamp(WINH & 0xffu, 0u, ++VISIBLE_SCREEN_WIDTH++ );
        uint Y1 = clamp(WINV >> 8u, 0u, ++VISIBLE_SCREEN_HEIGHT++);
        uint Y2 = clamp(WINV & 0xffu, 0u, ++VISIBLE_SCREEN_HEIGHT++);

        if (Y1 <= Y2) {
            // no vert wrap and out of bounds, continue
            if (y < Y1 || y >= Y2) {
                continue;
            }
        }
        else {
            // vert wrap and "in bounds":
            if ((y < Y1) && (y >= Y2)) {
                continue;
            }
        }

        if (X1 <= X2) {
            // no hor wrap
            if (x >= X1 && x < X2) {
                // pixel in WININ
                FragColor.x = WININ;
                gl_FragDepth = 0.0;
                return;
            }
        }
        else {
            // hor wrap
            if (x < X2 || x >= X1) {
                // pixel in WININ
                FragColor.x = WININ;
                gl_FragDepth = 0.0;
                return;
            }
        }
    }

    FragColor.x = readIOreg(++WINOUT++) & 0x3fu;  // WINOUT
    gl_FragDepth = 1.0;
}

// END WindowFragmentShaderSource