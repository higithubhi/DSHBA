// BEGIN FragmentShaderMode4Source


// Declarations moved to shared header (fragment.glsl/fragment_helpers.glsl)

vec4 mode4(uint x, uint y) {
    if (BG != 2u) {
        // only BG2 is drawn
        discard;
    }

    uint DISPCNT = readIOreg(++DISPCNT++);

    if ((DISPCNT & ++DisplayBG2++) == 0u) {
        // background 2 is disabled
        discard;
    }

    // offset is specified in DISPCNT
    uint Offset = 0u;
    if ((DISPCNT & ++DPFrameSelect++) != 0u) {
        // offset
        Offset = 0xa000u;
    }

    uint VRAMAddr = (++VISIBLE_SCREEN_WIDTH++ * y + x);
    VRAMAddr += Offset;
    uint PaletteIndex = readVRAM8(VRAMAddr);

    vec4 Color = readPALentry(PaletteIndex);
    uint BGCNT = readIOreg(++BG2CNT++);
    gl_FragDepth = getDepth(BGCNT);;

    // We already converted to BGR when buffering data
    return vec4(Color.rgb, 1.0);
}

// END FragmentShaderMode4Source