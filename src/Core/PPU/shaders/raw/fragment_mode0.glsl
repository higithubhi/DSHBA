// BEGIN FragmentShaderMode0Source


// Declarations moved to shared header (fragment.glsl/fragment_helpers.glsl)

vec4 mode0(uint x, uint y) {
    uint DISPCNT = readIOreg(++DISPCNT++);

    uint BGCNT = readIOreg(++BG0CNT++ + (BG << 1));

    vec4 Color;
    if ((DISPCNT & (++DisplayBG0++ << BG)) == 0u) {
        discard;  // background disabled
    }

    Color = regularBGPixel(BGCNT, x, y);

    if (Color.w != 0.0) {
        // priority
        gl_FragDepth = getDepth(BGCNT);
        return Color;
    }
    else {
        discard;
    }
}

// END FragmentShaderMode0Source