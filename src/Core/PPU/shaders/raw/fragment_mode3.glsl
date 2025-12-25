// BEGIN FragmentShaderMode3Source


// Declarations moved to shared header (fragment.glsl/fragment_helpers.glsl)


vec4 mode3(uint x, uint y) {
    if (BG != 2u) {
        // only BG2 is drawn
        discard;
    }

    uint DISPCNT = readIOreg(++DISPCNT++);

    if ((DISPCNT & ++DisplayBG2++) == 0u) {
        // background 2 is disabled
        discard;
    }

    uint VRAMAddr = (++VISIBLE_SCREEN_WIDTH++ * y + x) << 1;  // 16bpp

    uint PackedColor = readVRAM16(VRAMAddr);

    vec4 Color = vec4(0.0, 0.0, 0.0, 32.0);  // to be scaled later

    // BGR format
    Color.r = float(PackedColor & 0x1fu);
    PackedColor >>= 5u;
    Color.g = float(PackedColor & 0x1fu);
    PackedColor >>= 5u;
    Color.b = float(PackedColor & 0x1fu);

    uint BGCNT = readIOreg(++BG2CNT++);
    gl_FragDepth = getDepth(BGCNT);

    return Color / 32.0;
}

// END FragmentShaderMode3Source