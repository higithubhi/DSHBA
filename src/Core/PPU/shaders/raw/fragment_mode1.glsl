// BEGIN FragmentShaderMode1Source


// Declarations moved to shared header (fragment.glsl/fragment_helpers.glsl)

vec4 mode1(uint x, uint y, vec2 screen_pos) {
    if (BG == 3u) {
        // BG 3 is not drawn
        discard;
    }

    uint DISPCNT = readIOreg(++DISPCNT++);

    uint BGCNT = readIOreg(++BG0CNT++ + (BG << 1));

    vec4 Color;
    if ((DISPCNT & (++DisplayBG0++ << BG)) == 0u) {
        discard;  // background disabled
    }


    if (BG < 2u) {
        Color = regularBGPixel(BGCNT, x, y);
    }
    else {
        Color = affineBGPixel(BGCNT, screen_pos);
    }

    if (Color.w != 0.0) {
        // background priority
        gl_FragDepth = getDepth(BGCNT);
        return Color;
    }
    else {
        discard;
    }
}

// END FragmentShaderMode1Source