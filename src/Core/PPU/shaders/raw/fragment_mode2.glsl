// BEGIN FragmentShaderMode2Source


// Declarations moved to shared header (fragment.glsl/fragment_helpers.glsl)

vec4 mode2(uint x, uint y, vec2 screen_pos) {
    if (BG < 2u) {
        // only BG2 and 3 enabled
        discard;
    }

    uint DISPCNT = readIOreg(++DISPCNT++);

    uint BGCNT = readIOreg(++BG0CNT++ + (BG << 1));

    vec4 Color;
    if ((DISPCNT & (++DisplayBG0++ << BG)) == 0u) {
        discard;  // background disabled
    }

    Color = affineBGPixel(BGCNT, screen_pos);

    if (Color.w != 0.0) {
        // BG priority
        gl_FragDepth = getDepth(BGCNT);
        return Color;
    }
    else {
        discard;
    }
}

// END FragmentShaderMode2Source