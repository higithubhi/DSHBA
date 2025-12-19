#ifndef GC__SHADER_H
#define GC__SHADER_H

// BlitFragmentShaderSource (from blit_fragment.glsl, lines 1 to 32)
const char* BlitFragmentShaderSource = 
"#version 320 es\n                                                                                  "    // l:1
"precision mediump float;\n                                                                         "    // l:2
"\n                                                                                                 "    // l:3
"in vec2 texCoord;\n                                                                                "    // l:4
"\n                                                                                                 "    // l:5
"uniform sampler2D TopLayer;\n                                                                      "    // l:6
"uniform sampler2D BottomLayer;\n                                                                   "    // l:7
"\n                                                                                                 "    // l:8
"out vec4 FragColor;\n                                                                              "    // l:9
"\n                                                                                                 "    // l:10
"void main() {\n                                                                                    "    // l:11
"    vec4 top = texture(TopLayer, texCoord);\n                                                      "    // l:12
"    vec4 bottom = texture(BottomLayer, texCoord);\n                                                "    // l:13
"\n                                                                                                 "    // l:14
"    // default: pick top\n                                                                         "    // l:15
"    FragColor = vec4(\n                                                                            "    // l:16
"        top.rgb, 1.0\n                                                                             "    // l:17
"    );\n                                                                                           "    // l:18
"\n                                                                                                 "    // l:19
"    if ((bottom.a != 0.0) && (bottom.a <= 0.5)) {\n                                                "    // l:20
"        if (top.a >= 0.5) {\n                                                                      "    // l:21
"            float topASNorm = (top.a * 2.0) - 1.0;\n                                               "    // l:22
"            float bottomASNorm = (bottom.a * 2.0) - 1.0;\n                                         "    // l:23
"            vec3 blendedColor = top.rgb * topASNorm - 2.0 * bottom.rgb * (bottomASNorm + 0.25);\n  "    // l:24
"            FragColor = vec4(\n                                                                    "    // l:25
"                blendedColor, 1.0\n                                                                "    // l:26
"            );\n                                                                                   "    // l:27
"        }\n                                                                                        "    // l:28
"    }\n                                                                                            "    // l:29
"}\n                                                                                                "    // l:30
"\n                                                                                                 "    // l:31
;


// BlitVertexShaderSource (from blit_vertex.glsl, lines 1 to 20)
const char* BlitVertexShaderSource = 
"#version 320 es\n                                                                                  "    // l:1
"precision mediump float;\n                                                                         "    // l:2
"\n                                                                                                 "    // l:3
"layout (location = 0) in vec2 position;\n                                                          "    // l:4
"layout (location = 1) in vec2 inTexCoord;\n                                                        "    // l:5
"\n                                                                                                 "    // l:6
"out vec2 texCoord;  // needed for fragment_helpers\n                                               "    // l:7
"\n                                                                                                 "    // l:8
"void main() {\n                                                                                    "    // l:9
"    texCoord = inTexCoord;\n                                                                       "    // l:10
"\n                                                                                                 "    // l:11
"    gl_Position = vec4(\n                                                                          "    // l:12
"        position.x,\n                                                                              "    // l:13
"        position.y,\n                                                                              "    // l:14
"        0,\n                                                                                       "    // l:15
"        1\n                                                                                        "    // l:16
"    );\n                                                                                           "    // l:17
"}\n                                                                                                "    // l:18
"\n                                                                                                 "    // l:19
;


// FragmentShaderSource (from fragment.glsl, lines 1 to 301)
const char* FragmentShaderSource = 
"#version 320 es\n                                                                                  "    // l:1
"precision mediump float;\n                                                                         "    // l:2
"\n                                                                                                 "    // l:3
"in vec2 screenCoord;\n                                                                             "    // l:4
"\n                                                                                                 "    // l:5
"out vec4 FragColor;\n                                                                              "    // l:6
"uniform uint ReferenceLine2[160u];\n                                                               "    // l:7
"uniform uint ReferenceLine3[160u];\n                                                               "    // l:8
"\n                                                                                                 "    // l:9
"// BG 0 - 3 or 4 for backdrop\n                                                                    "    // l:10
"uniform uint BG;\n                                                                                 "    // l:11
"\n                                                                                                 "    // l:12
"vec4 ColorCorrect(vec4 color);\n                                                                   "    // l:13
"void CheckBottom(uint layer, uint window);\n                                                       "    // l:14
"vec4 AlphaCorrect(vec4 color, uint layer, uint window);\n                                          "    // l:15
"\n                                                                                                 "    // l:16
"uint readVRAM8(uint address);\n                                                                    "    // l:17
"uint readVRAM16(uint address);\n                                                                   "    // l:18
"\n                                                                                                 "    // l:19
"uint readVRAM32(uint address);\n                                                                   "    // l:20
"uint readIOreg(uint address);\n                                                                    "    // l:21
"vec4 readPALentry(uint index);\n                                                                   "    // l:22
"\n                                                                                                 "    // l:23
"uint getWindow(uint x, uint y);\n                                                                  "    // l:24
"\n                                                                                                 "    // l:25
"float getDepth(uint BGCNT) {\n                                                                     "    // l:26
"    return ((2.0 * float(BGCNT & 3u)) / 8.0) + (float(1u + BG) / 32.0);\n                          "    // l:27
"}\n                                                                                                "    // l:28
"\n                                                                                                 "    // l:29
"uint VRAMIndex(uint Tx, uint Ty, uint Size) {\n                                                    "    // l:30
"    uint temp = ((Ty & 0x1fu) << 6);\n                                                             "    // l:31
"    temp |= temp | ((Tx & 0x1fu) << 1);\n                                                          "    // l:32
"    switch (Size) {\n                                                                              "    // l:33
"        case 0u:  // 32x32\n                                                                       "    // l:34
"            break;\n                                                                               "    // l:35
"        case 1u:  // 64x32\n                                                                       "    // l:36
"            if ((Tx & 0x20u) != 0u) {\n                                                            "    // l:37
"                temp |= 0x800u;\n                                                                  "    // l:38
"            }\n                                                                                    "    // l:39
"            break;\n                                                                               "    // l:40
"        case 2u:  // 32x64\n                                                                       "    // l:41
"            if ((Ty & 0x20u) != 0u) {\n                                                            "    // l:42
"                temp |= 0x800u;\n                                                                  "    // l:43
"            }\n                                                                                    "    // l:44
"            break;\n                                                                               "    // l:45
"        case 3u:  // 64x64\n                                                                       "    // l:46
"            if ((Ty & 0x20u) != 0u) {\n                                                            "    // l:47
"                temp |= 0x1000u;\n                                                                 "    // l:48
"            }\n                                                                                    "    // l:49
"            if ((Tx & 0x20u) != 0u) {\n                                                            "    // l:50
"                temp |= 0x800u;\n                                                                  "    // l:51
"            }\n                                                                                    "    // l:52
"            break;\n                                                                               "    // l:53
"        default:\n                                                                                 "    // l:54
"            // invalid, should not happen\n                                                        "    // l:55
"            return 0u;\n                                                                           "    // l:56
"    }\n                                                                                            "    // l:57
"    return temp;\n                                                                                 "    // l:58
"}\n                                                                                                "    // l:59
"\n                                                                                                 "    // l:60
"vec4 regularScreenEntryPixel(uint dx, uint dy, uint ScreenEntry, uint CBB, bool ColorMode) {\n     "    // l:61
"    uint PaletteBank = ScreenEntry >> 12;  // 16 bits, we need top 4\n                             "    // l:62
"    if ((ScreenEntry & 0x0800u) != 0u) {\n                                                         "    // l:63
"        // VFlip\n                                                                                 "    // l:64
"        dy = 7u - dy;\n                                                                            "    // l:65
"    }\n                                                                                            "    // l:66
"\n                                                                                                 "    // l:67
"    if ((ScreenEntry & 0x0400u) != 0u) {\n                                                         "    // l:68
"        // HFlip\n                                                                                 "    // l:69
"        dx = 7u - dx;\n                                                                            "    // l:70
"    }\n                                                                                            "    // l:71
"\n                                                                                                 "    // l:72
"    uint TID     = ScreenEntry & 0x3ffu;\n                                                         "    // l:73
"    uint Address = CBB << 14;\n                                                                    "    // l:74
"\n                                                                                                 "    // l:75
"    if (!ColorMode) {\n                                                                            "    // l:76
"        // 4bpp\n                                                                                  "    // l:77
"        Address += TID << 5; // beginning of tile\n                                                "    // l:78
"        Address += dy << 2;  // beginning of sliver\n                                              "    // l:79
"\n                                                                                                 "    // l:80
"        Address += dx >> 1;  // offset into sliver\n                                               "    // l:81
"        uint VRAMEntry = readVRAM8(Address);\n                                                     "    // l:82
"        if ((dx & 1u) != 0u) {\n                                                                   "    // l:83
"            VRAMEntry >>= 4;     // odd x, upper nibble\n                                          "    // l:84
"        }\n                                                                                        "    // l:85
"        else {\n                                                                                   "    // l:86
"            VRAMEntry &= 0xfu;  // even x, lower nibble\n                                          "    // l:87
"        }\n                                                                                        "    // l:88
"\n                                                                                                 "    // l:89
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:90
"            return vec4(readPALentry((PaletteBank << 4) + VRAMEntry).rgb, 1.0);\n                  "    // l:91
"        }\n                                                                                        "    // l:92
"    }\n                                                                                            "    // l:93
"    else {\n                                                                                       "    // l:94
"        // 8bpp\n                                                                                  "    // l:95
"        Address += TID << 6; // beginning of tile\n                                                "    // l:96
"        Address += dy << 3;  // beginning of sliver\n                                              "    // l:97
"\n                                                                                                 "    // l:98
"        Address += dx;       // offset into sliver\n                                               "    // l:99
"        uint VRAMEntry = readVRAM8(Address);\n                                                     "    // l:100
"\n                                                                                                 "    // l:101
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:102
"            return vec4(readPALentry(VRAMEntry).rgb, 1.0);\n                                       "    // l:103
"        }\n                                                                                        "    // l:104
"    }\n                                                                                            "    // l:105
"\n                                                                                                 "    // l:106
"    // transparent\n                                                                               "    // l:107
"    discard;\n                                                                                     "    // l:108
"}\n                                                                                                "    // l:109
"\n                                                                                                 "    // l:110
"vec4 regularBGPixel(uint BGCNT, uint x, uint y) {\n                                                "    // l:111
"    uint HOFS, VOFS;\n                                                                             "    // l:112
"    uint CBB, SBB, Size;\n                                                                         "    // l:113
"    bool ColorMode;\n                                                                              "    // l:114
"\n                                                                                                 "    // l:115
"    HOFS  = readIOreg(0x10u + (BG << 2)) & 0x1ffu;\n                                               "    // l:116
"    VOFS  = readIOreg(0x12u + (BG << 2)) & 0x1ffu;\n                                               "    // l:117
"\n                                                                                                 "    // l:118
"    CBB       = (BGCNT >> 2) & 3u;\n                                                               "    // l:119
"    ColorMode = (BGCNT & 0x0080u) == 0x0080u;  // 0: 4bpp, 1: 8bpp\n                               "    // l:120
"    SBB       = (BGCNT >> 8) & 0x1fu;\n                                                            "    // l:121
"    Size      = (BGCNT >> 14) & 3u;\n                                                              "    // l:122
"\n                                                                                                 "    // l:123
"    uint x_eff = (x + HOFS) & 0xffffu;\n                                                           "    // l:124
"    uint y_eff = (y + VOFS) & 0xffffu;\n                                                           "    // l:125
"\n                                                                                                 "    // l:126
"    // mosaic effect\n                                                                             "    // l:127
"    if ((BGCNT & 0x0040u) != 0u) {\n                                                               "    // l:128
"        uint MOSAIC = readIOreg(0x4cu);\n                                                          "    // l:129
"        x_eff -= x_eff % ((MOSAIC & 0xfu) + 1u);\n                                                 "    // l:130
"        y_eff -= y_eff % (((MOSAIC & 0xf0u) >> 4) + 1u);\n                                         "    // l:131
"    }\n                                                                                            "    // l:132
"\n                                                                                                 "    // l:133
"    uint ScreenEntryIndex = VRAMIndex(x_eff >> 3u, y_eff >> 3u, Size);\n                           "    // l:134
"    ScreenEntryIndex += (SBB << 11u);\n                                                            "    // l:135
"    uint ScreenEntry = readVRAM16(ScreenEntryIndex);  // always halfword aligned\n                 "    // l:136
"\n                                                                                                 "    // l:137
"    return regularScreenEntryPixel(x_eff & 7u, y_eff & 7u, ScreenEntry, CBB, ColorMode);\n         "    // l:138
"}\n                                                                                                "    // l:139
"\n                                                                                                 "    // l:140
"const uint AffineBGSizeTable[4] = uint[](\n                                                        "    // l:141
"    128u, 256u, 512u, 1024u\n                                                                      "    // l:142
");\n                                                                                               "    // l:143
"\n                                                                                                 "    // l:144
"vec4 affineBGPixel(uint BGCNT, vec2 screen_pos) {\n                                                "    // l:145
"    uint x = uint(screen_pos.x);\n                                                                 "    // l:146
"    uint y = uint(screen_pos.y);\n                                                                 "    // l:147
"\n                                                                                                 "    // l:148
"    uint ReferenceLine;\n                                                                          "    // l:149
"    uint BGX_raw, BGY_raw;\n                                                                       "    // l:150
"    int PA, PB, PC, PD;\n                                                                          "    // l:151
"    if (BG == 2u) {\n                                                                              "    // l:152
"        ReferenceLine = ReferenceLine2[y];\n                                                       "    // l:153
"\n                                                                                                 "    // l:154
"        BGX_raw  = readIOreg(0x28u);\n                                                             "    // l:155
"        BGX_raw |= readIOreg(0x2au) << 16;\n                                                       "    // l:156
"        BGY_raw  = readIOreg(0x2cu);\n                                                             "    // l:157
"        BGY_raw |= readIOreg(0x2eu) << 16;\n                                                       "    // l:158
"        PA = int(readIOreg(0x20u)) << 16;\n                                                        "    // l:159
"        PB = int(readIOreg(0x22u)) << 16;\n                                                        "    // l:160
"        PC = int(readIOreg(0x24u)) << 16;\n                                                        "    // l:161
"        PD = int(readIOreg(0x26u)) << 16;\n                                                        "    // l:162
"    }\n                                                                                            "    // l:163
"    else {\n                                                                                       "    // l:164
"        ReferenceLine = ReferenceLine3[y];\n                                                       "    // l:165
"\n                                                                                                 "    // l:166
"        BGX_raw  = readIOreg(0x38u);\n                                                             "    // l:167
"        BGX_raw |= readIOreg(0x3au) << 16;\n                                                       "    // l:168
"        BGY_raw  = readIOreg(0x3cu);\n                                                             "    // l:169
"        BGY_raw |= readIOreg(0x3eu) << 16;\n                                                       "    // l:170
"        PA = int(readIOreg(0x30u)) << 16;\n                                                        "    // l:171
"        PB = int(readIOreg(0x32u)) << 16;\n                                                        "    // l:172
"        PC = int(readIOreg(0x34u)) << 16;\n                                                        "    // l:173
"        PD = int(readIOreg(0x36u)) << 16;\n                                                        "    // l:174
"    }\n                                                                                            "    // l:175
"\n                                                                                                 "    // l:176
"    // convert to signed\n                                                                         "    // l:177
"    int BGX = int(BGX_raw) << 4;\n                                                                 "    // l:178
"    int BGY = int(BGY_raw) << 4;\n                                                                 "    // l:179
"    BGX >>= 4;\n                                                                                   "    // l:180
"    BGY >>= 4;\n                                                                                   "    // l:181
"\n                                                                                                 "    // l:182
"    // was already shifted left\n                                                                  "    // l:183
"    PA >>= 16;\n                                                                                   "    // l:184
"    PB >>= 16;\n                                                                                   "    // l:185
"    PC >>= 16;\n                                                                                   "    // l:186
"    PD >>= 16;\n                                                                                   "    // l:187
"\n                                                                                                 "    // l:188
"    uint CBB, SBB, Size;\n                                                                         "    // l:189
"    bool ColorMode;\n                                                                              "    // l:190
"\n                                                                                                 "    // l:191
"    CBB       = (BGCNT >> 2) & 3u;\n                                                               "    // l:192
"    SBB       = (BGCNT >> 8) & 0x1fu;\n                                                            "    // l:193
"    Size      = AffineBGSizeTable[(BGCNT >> 14) & 3u];\n                                           "    // l:194
"\n                                                                                                 "    // l:195
"    mat2x2 RotationScaling = mat2x2(\n                                                             "    // l:196
"        float(PA), float(PC),  // first column\n                                                   "    // l:197
"        float(PB), float(PD)   // second column\n                                                  "    // l:198
"    );\n                                                                                           "    // l:199
"\n                                                                                                 "    // l:200
"    vec2 pos  = screen_pos - vec2(0, ReferenceLine);\n                                             "    // l:201
"    int x_eff = int(float(BGX) + dot(vec2(PA, PB), pos));\n                                        "    // l:202
"    int y_eff = int(float(BGY) + dot(vec2(PC, PD), pos));\n                                        "    // l:203
"\n                                                                                                 "    // l:204
"    // correct for fixed point math\n                                                              "    // l:205
"    x_eff >>= 8;\n                                                                                 "    // l:206
"    y_eff >>= 8;\n                                                                                 "    // l:207
"\n                                                                                                 "    // l:208
"    if ((x_eff < 0) || (x_eff > int(Size)) || (y_eff < 0) || (y_eff > int(Size))) {\n              "    // l:209
"        if ((BGCNT & 0x2000u) == 0u) {\n                                                           "    // l:210
"            // no display area overflow\n                                                          "    // l:211
"            discard;\n                                                                             "    // l:212
"        }\n                                                                                        "    // l:213
"\n                                                                                                 "    // l:214
"        // wrapping\n                                                                              "    // l:215
"        x_eff &= int(Size) - 1;\n                                                                  "    // l:216
"        y_eff &= int(Size) - 1;\n                                                                  "    // l:217
"    }\n                                                                                            "    // l:218
"\n                                                                                                 "    // l:219
"    // mosaic effect\n                                                                             "    // l:220
"    if ((BGCNT & 0x0040u) != 0u) {\n                                                               "    // l:221
"        uint MOSAIC = readIOreg(0x4cu);\n                                                          "    // l:222
"        x_eff -= x_eff % int((MOSAIC & 0xfu) + 1u);\n                                              "    // l:223
"        y_eff -= y_eff % int(((MOSAIC & 0xf0u) >> 4u) + 1u);\n                                     "    // l:224
"    }\n                                                                                            "    // l:225
"\n                                                                                                 "    // l:226
"    uint TIDAddress = (SBB << 11u);  // base\n                                                     "    // l:227
"    TIDAddress += ((uint(y_eff) >> 3) * (Size >> 3)) | (uint(x_eff) >> 3);  // tile\n              "    // l:228
"    uint TID = readVRAM8(TIDAddress);\n                                                            "    // l:229
"\n                                                                                                 "    // l:230
"    uint PixelAddress = (CBB << 14) | (TID << 6) | ((uint(y_eff) & 7u) << 3) | (uint(x_eff) & 7u);\n"    // l:231
"    uint VRAMEntry = readVRAM8(PixelAddress);\n                                                    "    // l:232
"\n                                                                                                 "    // l:233
"    // transparent\n                                                                               "    // l:234
"    if (VRAMEntry == 0u) {\n                                                                       "    // l:235
"        discard;\n                                                                                 "    // l:236
"    }\n                                                                                            "    // l:237
"\n                                                                                                 "    // l:238
"    return vec4(readPALentry(VRAMEntry).rgb, 1.0);\n                                               "    // l:239
"}\n                                                                                                "    // l:240
"\n                                                                                                 "    // l:241
"vec4 mode0(uint, uint);\n                                                                          "    // l:242
"vec4 mode1(uint, uint, vec2);\n                                                                    "    // l:243
"vec4 mode2(uint, uint, vec2);\n                                                                    "    // l:244
"vec4 mode3(uint, uint);\n                                                                          "    // l:245
"vec4 mode4(uint, uint);\n                                                                          "    // l:246
"\n                                                                                                 "    // l:247
"void main() {\n                                                                                    "    // l:248
"    uint x = uint(screenCoord.x);\n                                                                "    // l:249
"    uint y = uint(screenCoord.y);\n                                                                "    // l:250
"\n                                                                                                 "    // l:251
"    uint window = getWindow(x, y);\n                                                               "    // l:252
"    uint BLDCNT = readIOreg(0x50u);\n                                                              "    // l:253
"\n                                                                                                 "    // l:254
"    if (BG >= 4u) {\n                                                                              "    // l:255
"        CheckBottom(5u, window);\n                                                                 "    // l:256
"\n                                                                                                 "    // l:257
"        // backdrop, highest frag depth\n                                                          "    // l:258
"        gl_FragDepth = 1.0;\n                                                                      "    // l:259
"        FragColor = ColorCorrect(vec4(readPALentry(0u).rgb, 1.0));\n                               "    // l:260
"        FragColor = AlphaCorrect(FragColor, 5u, window);\n                                         "    // l:261
"        return;\n                                                                                  "    // l:262
"    }\n                                                                                            "    // l:263
"\n                                                                                                 "    // l:264
"    // check if we are rendering on the bottom layer, and if we even need to render this fragment\n"    // l:265
"    CheckBottom(BG, window);\n                                                                     "    // l:266
"\n                                                                                                 "    // l:267
"    // disabled by window\n                                                                        "    // l:268
"    if ((window & (1u << BG)) == 0u) {\n                                                           "    // l:269
"        discard;\n                                                                                 "    // l:270
"    }\n                                                                                            "    // l:271
"\n                                                                                                 "    // l:272
"    uint DISPCNT = readIOreg(0u);\n                                                                "    // l:273
"\n                                                                                                 "    // l:274
"    vec4 outColor;\n                                                                               "    // l:275
"    switch(DISPCNT & 7u) {\n                                                                       "    // l:276
"        case 0u:\n                                                                                 "    // l:277
"            outColor = mode0(x, y);\n                                                              "    // l:278
"            break;\n                                                                               "    // l:279
"        case 1u:\n                                                                                 "    // l:280
"            outColor = mode1(x, y, screenCoord);\n                                                 "    // l:281
"            break;\n                                                                               "    // l:282
"        case 2u:\n                                                                                 "    // l:283
"            outColor = mode2(x, y, screenCoord);\n                                                 "    // l:284
"            break;\n                                                                               "    // l:285
"        case 3u:\n                                                                                 "    // l:286
"            outColor = mode3(x, y);\n                                                              "    // l:287
"            break;\n                                                                               "    // l:288
"        case 4u:\n                                                                                 "    // l:289
"            outColor = mode4(x, y);\n                                                              "    // l:290
"            break;\n                                                                               "    // l:291
"        default:\n                                                                                 "    // l:292
"            outColor = vec4(float(x) / float(240u), float(y) / float(160u), 1.0, 1.0);\n           "    // l:293
"            break;\n                                                                               "    // l:294
"    }\n                                                                                            "    // l:295
"\n                                                                                                 "    // l:296
"    FragColor = ColorCorrect(outColor);\n                                                          "    // l:297
"    FragColor = AlphaCorrect(FragColor, BG, window);\n                                             "    // l:298
"}\n                                                                                                "    // l:299
"\n                                                                                                 "    // l:300
;


// FragmentHelperSource (from fragment_helpers.glsl, lines 1 to 164)
const char* FragmentHelperSource = 
"precision mediump float;\n                                                                         "    // l:1
"precision mediump usampler2D;\n                                                                    "    // l:2
"precision mediump isampler2D;\n                                                                    "    // l:3
"\n                                                                                                 "    // l:4
"/* GENERAL */\n                                                                                    "    // l:5
"in vec2 OnScreenPos;\n                                                                             "    // l:6
"\n                                                                                                 "    // l:7
"uniform usampler2D PAL;\n                                                                          "    // l:8
"uniform usampler2D VRAM;\n                                                                         "    // l:9
"uniform usampler2D IO;\n                                                                           "    // l:10
"uniform isampler2D OAM;\n                                                                          "    // l:11
"uniform usampler2D Window;\n                                                                       "    // l:12
"\n                                                                                                 "    // l:13
"uniform bool Bottom;\n                                                                             "    // l:14
"\n                                                                                                 "    // l:15
"uniform uint PALBufferIndex[160u];\n                                                               "    // l:16
"\n                                                                                                 "    // l:17
"uint readIOreg(uint address);\n                                                                    "    // l:18
"\n                                                                                                 "    // l:19
"// algorithm from https://byuu.net/video/color-emulation/\n                                        "    // l:20
"const float lcdGamma = 4.0;\n                                                                      "    // l:21
"const float outGamma = 2.2;\n                                                                      "    // l:22
"const mat3x3 CorrectionMatrix = mat3x3(\n                                                          "    // l:23
"        255.0,  10.0,  50.0,\n                                                                     "    // l:24
"         50.0, 230.0,  10.0,\n                                                                     "    // l:25
"          0.0,  30.0, 220.0\n                                                                      "    // l:26
") / 255.0;\n                                                                                       "    // l:27
"\n                                                                                                 "    // l:28
"vec4 ColorCorrect(vec4 color) {\n                                                                  "    // l:29
"    vec3 lrgb = pow(color.rgb, vec3(lcdGamma));\n                                                  "    // l:30
"    vec3 rgb = pow(CorrectionMatrix * lrgb, vec3(1.0 / outGamma)) * (255.0 / 280.0);\n             "    // l:31
"    return vec4(rgb, color.a);\n                                                                   "    // l:32
"}\n                                                                                                "    // l:33
"\n                                                                                                 "    // l:34
"void CheckBottom(uint layer, uint window) {\n                                                      "    // l:35
"    if (Bottom) {\n                                                                                "    // l:36
"        uint BLDCNT = readIOreg(0x50u);\n                                                          "    // l:37
"        if (((BLDCNT & 0x00c0u) != 0x0040u)) {\n                                                   "    // l:38
"            // not interesting, not normal alpha blending\n                                        "    // l:39
"            discard;\n                                                                             "    // l:40
"        }\n                                                                                        "    // l:41
"\n                                                                                                 "    // l:42
"        if ((window & 0x20u) == 0u) {\n                                                            "    // l:43
"            // blending disabled in window, don't render on bottom frame\n                         "    // l:44
"            discard;\n                                                                             "    // l:45
"        }\n                                                                                        "    // l:46
"    }\n                                                                                            "    // l:47
"}\n                                                                                                "    // l:48
"\n                                                                                                 "    // l:49
"vec4 AlphaCorrect(vec4 color, uint layer, uint window) {\n                                         "    // l:50
"    // BG0-3, 4 for Obj, 5 for BD\n                                                                "    // l:51
"    if ((window & 0x20u) == 0u) {\n                                                                "    // l:52
"        // blending disabled in window\n                                                           "    // l:53
"        return vec4(color.rgb, -1);\n                                                              "    // l:54
"    }\n                                                                                            "    // l:55
"\n                                                                                                 "    // l:56
"    uint BLDCNT = readIOreg(0x50u);\n                                                              "    // l:57
"    uint BLDY = clamp(readIOreg(0x54u) & 0x1fu, 0u, 16u);\n                                        "    // l:58
"\n                                                                                                 "    // l:59
"    switch (BLDCNT & 0x00c0u) {\n                                                                  "    // l:60
"        case 0x0000u:\n                                                                            "    // l:61
"            // blending disabled\n                                                                 "    // l:62
"            return vec4(color.rgb, -1);\n                                                          "    // l:63
"        case 0x0040u:\n                                                                            "    // l:64
"            // normal blending, do this after (most complicated)\n                                 "    // l:65
"            break;\n                                                                               "    // l:66
"        case 0x0080u:\n                                                                            "    // l:67
"        {\n                                                                                        "    // l:68
"            // blend A with white\n                                                                "    // l:69
"            if ((BLDCNT & (1u << layer)) != 0u) {\n                                                "    // l:70
"                // layer is top layer\n                                                            "    // l:71
"                return vec4(mix(color.rgb, vec3(1), float(BLDY) / 16.0), -1.0);\n                  "    // l:72
"            }\n                                                                                    "    // l:73
"            // bottom layer, not blended\n                                                         "    // l:74
"            return vec4(color.rgb, -1);\n                                                          "    // l:75
"        }\n                                                                                        "    // l:76
"        case 0x00c0u:\n                                                                            "    // l:77
"        {\n                                                                                        "    // l:78
"            // blend A with black\n                                                                "    // l:79
"            if ((BLDCNT & (1u << layer)) != 0u) {\n                                                "    // l:80
"                // layer is top layer\n                                                            "    // l:81
"                return vec4(mix(color.rgb, vec3(0), float(BLDY) / 16.0), -1.0);\n                  "    // l:82
"            }\n                                                                                    "    // l:83
"            // bottom layer, not blended\n                                                         "    // l:84
"            return vec4(color.rgb, -1);\n                                                          "    // l:85
"        }\n                                                                                        "    // l:86
"    }\n                                                                                            "    // l:87
"\n                                                                                                 "    // l:88
"    // value was not normal blending / fade\n                                                      "    // l:89
"    uint BLDALPHA = readIOreg(0x52u);\n                                                            "    // l:90
"    uint BLD_EVA = clamp(BLDALPHA & 0x1fu, 0u, 16u);\n                                             "    // l:91
"    uint BLD_EVB = clamp((BLDALPHA >> 8u) & 0x1fu, 0u, 16u);\n                                     "    // l:92
"\n                                                                                                 "    // l:93
"    if ((BLDCNT & (1u << layer)) != 0u) {\n                                                        "    // l:94
"        // top layer\n                                                                             "    // l:95
"        if (!Bottom) {\n                                                                           "    // l:96
"            return vec4(color.rgb, float(BLD_EVA) / 16.0);\n                                       "    // l:97
"        }\n                                                                                        "    // l:98
"        else {\n                                                                                   "    // l:99
"            discard;\n                                                                             "    // l:100
"        }\n                                                                                        "    // l:101
"    }\n                                                                                            "    // l:102
"    // bottom layer\n                                                                              "    // l:103
"    if ((BLDCNT & (0x100u << layer)) != 0u) {\n                                                    "    // l:104
"        // set alpha value to -half of the actual value\n                                          "    // l:105
"        // -1 means: final color\n                                                                 "    // l:106
"        // negative: bottom\n                                                                      "    // l:107
"        // positive: top\n                                                                         "    // l:108
"        return vec4(color.rgb, -0.25 - (float(BLD_EVB) / 32.0));\n                                 "    // l:109
"    }\n                                                                                            "    // l:110
"\n                                                                                                 "    // l:111
"    // neither\n                                                                                   "    // l:112
"    return vec4(color.rgb, -1);\n                                                                  "    // l:113
"}\n                                                                                                "    // l:114
"\n                                                                                                 "    // l:115
"uint readVRAM8(uint address) {\n                                                                   "    // l:116
"    return texelFetch(\n                                                                           "    // l:117
"        VRAM, ivec2(address & 0x7fu, address >> 7u), 0\n                                           "    // l:118
"    ).x;\n                                                                                         "    // l:119
"}\n                                                                                                "    // l:120
"\n                                                                                                 "    // l:121
"uint readVRAM16(uint address) {\n                                                                  "    // l:122
"    address &= ~1u;\n                                                                              "    // l:123
"    uint lsb = readVRAM8(address);\n                                                               "    // l:124
"    return lsb | (readVRAM8(address + 1u) << 8u);\n                                                "    // l:125
"}\n                                                                                                "    // l:126
"\n                                                                                                 "    // l:127
"uint readVRAM32(uint address) {\n                                                                  "    // l:128
"    address &= ~3u;\n                                                                              "    // l:129
"    uint lsh = readVRAM16(address);\n                                                              "    // l:130
"    return lsh | (readVRAM16(address + 2u) << 16u);\n                                              "    // l:131
"}\n                                                                                                "    // l:132
"\n                                                                                                 "    // l:133
"uint readIOreg(uint address) {\n                                                                   "    // l:134
"    return texelFetch(\n                                                                           "    // l:135
"        IO, ivec2(int(address >> 1u), int(OnScreenPos.y)), 0\n                                     "    // l:136
"    ).x;\n                                                                                         "    // l:137
"}\n                                                                                                "    // l:138
"\n                                                                                                 "    // l:139
"ivec4 readOAMentry(uint index) {\n                                                                 "    // l:140
"    return texelFetch(\n                                                                           "    // l:141
"        OAM, ivec2(int(index), 0), 0\n                                                             "    // l:142
"    );\n                                                                                           "    // l:143
"}\n                                                                                                "    // l:144
"\n                                                                                                 "    // l:145
"vec3 decodeBGR555(uint v) {\n                                                                      "    // l:146
"    uint r =  v        & 31u;\n                                                                    "    // l:147
"    uint g = (v >> 5u) & 31u;\n                                                                    "    // l:148
"    uint b = (v >> 10u)& 31u;\n                                                                    "    // l:149
"    return vec3(r, g, b) / 31.0;\n                                                                 "    // l:150
"}\n                                                                                                "    // l:151
"\n                                                                                                 "    // l:152
"vec4 readPALentry(uint index) {\n                                                                  "    // l:153
"    uint v = texelFetch(PAL, ivec2(int(index), int(PALBufferIndex[uint(OnScreenPos.y)])), 0).r;\n  "    // l:154
"    return vec4(decodeBGR555(v), 1.0);\n                                                           "    // l:155
"}\n                                                                                                "    // l:156
"\n                                                                                                 "    // l:157
"uint getWindow(uint x, uint y) {\n                                                                 "    // l:158
"    return texelFetch(\n                                                                           "    // l:159
"        Window, ivec2(x, int(160u) - int(y)), 0\n                                                  "    // l:160
"    ).r;\n                                                                                         "    // l:161
"}\n                                                                                                "    // l:162
"\n                                                                                                 "    // l:163
;


// FragmentShaderMode0Source (from fragment_mode0.glsl, lines 2 to 34)
const char* FragmentShaderMode0Source = 
"uint readVRAM8(uint address);\n                                                                    "    // l:1
"uint readVRAM16(uint address);\n                                                                   "    // l:2
"uint readVRAM32(uint address);\n                                                                   "    // l:3
"\n                                                                                                 "    // l:4
"uint readIOreg(uint address);\n                                                                    "    // l:5
"vec4 readPALentry(uint index);\n                                                                   "    // l:6
"\n                                                                                                 "    // l:7
"vec4 regularBGPixel(uint BGCNT, uint x, uint y);\n                                                 "    // l:8
"float getDepth(uint BGCNT);\n                                                                      "    // l:9
"\n                                                                                                 "    // l:10
"vec4 mode0(uint x, uint y) {\n                                                                     "    // l:11
"    uint DISPCNT = readIOreg(0x00u);\n                                                             "    // l:12
"\n                                                                                                 "    // l:13
"    uint BGCNT = readIOreg(0x08u + (BG << 1));\n                                                   "    // l:14
"\n                                                                                                 "    // l:15
"    vec4 Color;\n                                                                                  "    // l:16
"    if ((DISPCNT & (0x0100u << BG)) == 0u) {\n                                                     "    // l:17
"        discard;  // background disabled\n                                                         "    // l:18
"    }\n                                                                                            "    // l:19
"\n                                                                                                 "    // l:20
"    Color = regularBGPixel(BGCNT, x, y);\n                                                         "    // l:21
"\n                                                                                                 "    // l:22
"    if (Color.w != 0.0) {\n                                                                        "    // l:23
"        // priority\n                                                                              "    // l:24
"        gl_FragDepth = getDepth(BGCNT);\n                                                          "    // l:25
"        return Color;\n                                                                            "    // l:26
"    }\n                                                                                            "    // l:27
"    else {\n                                                                                       "    // l:28
"        discard;\n                                                                                 "    // l:29
"    }\n                                                                                            "    // l:30
"}\n                                                                                                "    // l:31
"\n                                                                                                 "    // l:32
;


// FragmentShaderMode1Source (from fragment_mode1.glsl, lines 5 to 49)
const char* FragmentShaderMode1Source = 
"uint readVRAM8(uint address);\n                                                                    "    // l:1
"uint readVRAM16(uint address);\n                                                                   "    // l:2
"uint readVRAM32(uint address);\n                                                                   "    // l:3
"\n                                                                                                 "    // l:4
"uint readIOreg(uint address);\n                                                                    "    // l:5
"vec4 readPALentry(uint index);\n                                                                   "    // l:6
"\n                                                                                                 "    // l:7
"vec4 regularBGPixel(uint BGCNT, uint x, uint y);\n                                                 "    // l:8
"vec4 affineBGPixel(uint BGCNT, vec2 screen_pos);\n                                                 "    // l:9
"float getDepth(uint BGCNT);\n                                                                      "    // l:10
"\n                                                                                                 "    // l:11
"vec4 mode1(uint x, uint y, vec2 screen_pos) {\n                                                    "    // l:12
"    if (BG == 3u) {\n                                                                              "    // l:13
"        // BG 3 is not drawn\n                                                                     "    // l:14
"        discard;\n                                                                                 "    // l:15
"    }\n                                                                                            "    // l:16
"\n                                                                                                 "    // l:17
"    uint DISPCNT = readIOreg(0x00u);\n                                                             "    // l:18
"\n                                                                                                 "    // l:19
"    uint BGCNT = readIOreg(0x08u + (BG << 1));\n                                                   "    // l:20
"\n                                                                                                 "    // l:21
"    vec4 Color;\n                                                                                  "    // l:22
"    if ((DISPCNT & (0x0100u << BG)) == 0u) {\n                                                     "    // l:23
"        discard;  // background disabled\n                                                         "    // l:24
"    }\n                                                                                            "    // l:25
"\n                                                                                                 "    // l:26
"\n                                                                                                 "    // l:27
"    if (BG < 2u) {\n                                                                               "    // l:28
"        Color = regularBGPixel(BGCNT, x, y);\n                                                     "    // l:29
"    }\n                                                                                            "    // l:30
"    else {\n                                                                                       "    // l:31
"        Color = affineBGPixel(BGCNT, screen_pos);\n                                                "    // l:32
"    }\n                                                                                            "    // l:33
"\n                                                                                                 "    // l:34
"    if (Color.w != 0.0) {\n                                                                        "    // l:35
"        // background priority\n                                                                   "    // l:36
"        gl_FragDepth = getDepth(BGCNT);\n                                                          "    // l:37
"        return Color;\n                                                                            "    // l:38
"    }\n                                                                                            "    // l:39
"    else {\n                                                                                       "    // l:40
"        discard;\n                                                                                 "    // l:41
"    }\n                                                                                            "    // l:42
"}\n                                                                                                "    // l:43
"\n                                                                                                 "    // l:44
;


// FragmentShaderMode2Source (from fragment_mode2.glsl, lines 3 to 41)
const char* FragmentShaderMode2Source = 
"uint readVRAM8(uint address);\n                                                                    "    // l:1
"uint readVRAM16(uint address);\n                                                                   "    // l:2
"uint readVRAM32(uint address);\n                                                                   "    // l:3
"\n                                                                                                 "    // l:4
"uint readIOreg(uint address);\n                                                                    "    // l:5
"vec4 readPALentry(uint index);\n                                                                   "    // l:6
"\n                                                                                                 "    // l:7
"vec4 regularBGPixel(uint BGCNT, uint x, uint y);\n                                                 "    // l:8
"vec4 affineBGPixel(uint BGCNT, vec2 screen_pos);\n                                                 "    // l:9
"float getDepth(uint BGCNT);\n                                                                      "    // l:10
"\n                                                                                                 "    // l:11
"vec4 mode2(uint x, uint y, vec2 screen_pos) {\n                                                    "    // l:12
"    if (BG < 2u) {\n                                                                               "    // l:13
"        // only BG2 and 3 enabled\n                                                                "    // l:14
"        discard;\n                                                                                 "    // l:15
"    }\n                                                                                            "    // l:16
"\n                                                                                                 "    // l:17
"    uint DISPCNT = readIOreg(0x00u);\n                                                             "    // l:18
"\n                                                                                                 "    // l:19
"    uint BGCNT = readIOreg(0x08u + (BG << 1));\n                                                   "    // l:20
"\n                                                                                                 "    // l:21
"    vec4 Color;\n                                                                                  "    // l:22
"    if ((DISPCNT & (0x0100u << BG)) == 0u) {\n                                                     "    // l:23
"        discard;  // background disabled\n                                                         "    // l:24
"    }\n                                                                                            "    // l:25
"\n                                                                                                 "    // l:26
"    Color = affineBGPixel(BGCNT, screen_pos);\n                                                    "    // l:27
"\n                                                                                                 "    // l:28
"    if (Color.w != 0.0) {\n                                                                        "    // l:29
"        // BG priority\n                                                                           "    // l:30
"        gl_FragDepth = getDepth(BGCNT);\n                                                          "    // l:31
"        return Color;\n                                                                            "    // l:32
"    }\n                                                                                            "    // l:33
"    else {\n                                                                                       "    // l:34
"        discard;\n                                                                                 "    // l:35
"    }\n                                                                                            "    // l:36
"}\n                                                                                                "    // l:37
"\n                                                                                                 "    // l:38
;


// FragmentShaderMode3Source (from fragment_mode3.glsl, lines 2 to 43)
const char* FragmentShaderMode3Source = 
"uint readVRAM8(uint address);\n                                                                    "    // l:1
"uint readVRAM16(uint address);\n                                                                   "    // l:2
"uint readVRAM32(uint address);\n                                                                   "    // l:3
"\n                                                                                                 "    // l:4
"uint readIOreg(uint address);\n                                                                    "    // l:5
"vec4 readPALentry(uint index);\n                                                                   "    // l:6
"float getDepth(uint BGCNT);\n                                                                      "    // l:7
"\n                                                                                                 "    // l:8
"\n                                                                                                 "    // l:9
"vec4 mode3(uint x, uint y) {\n                                                                     "    // l:10
"    if (BG != 2u) {\n                                                                              "    // l:11
"        // only BG2 is drawn\n                                                                     "    // l:12
"        discard;\n                                                                                 "    // l:13
"    }\n                                                                                            "    // l:14
"\n                                                                                                 "    // l:15
"    uint DISPCNT = readIOreg(0x00u);\n                                                             "    // l:16
"\n                                                                                                 "    // l:17
"    if ((DISPCNT & 0x0400u) == 0u) {\n                                                             "    // l:18
"        // background 2 is disabled\n                                                              "    // l:19
"        discard;\n                                                                                 "    // l:20
"    }\n                                                                                            "    // l:21
"\n                                                                                                 "    // l:22
"    uint VRAMAddr = (240u * y + x) << 1;  // 16bpp\n                                               "    // l:23
"\n                                                                                                 "    // l:24
"    uint PackedColor = readVRAM16(VRAMAddr);\n                                                     "    // l:25
"\n                                                                                                 "    // l:26
"    vec4 Color = vec4(0.0, 0.0, 0.0, 32.0);  // to be scaled later\n                               "    // l:27
"\n                                                                                                 "    // l:28
"    // BGR format\n                                                                                "    // l:29
"    Color.r = float(PackedColor & 0x1fu);\n                                                        "    // l:30
"    PackedColor >>= 5u;\n                                                                          "    // l:31
"    Color.g = float(PackedColor & 0x1fu);\n                                                        "    // l:32
"    PackedColor >>= 5u;\n                                                                          "    // l:33
"    Color.b = float(PackedColor & 0x1fu);\n                                                        "    // l:34
"\n                                                                                                 "    // l:35
"    uint BGCNT = readIOreg(0x0cu);\n                                                               "    // l:36
"    gl_FragDepth = getDepth(BGCNT);\n                                                              "    // l:37
"\n                                                                                                 "    // l:38
"    return Color / 32.0;\n                                                                         "    // l:39
"}\n                                                                                                "    // l:40
"\n                                                                                                 "    // l:41
;


// FragmentShaderMode4Source (from fragment_mode4.glsl, lines 2 to 42)
const char* FragmentShaderMode4Source = 
"uint readVRAM8(uint address);\n                                                                    "    // l:1
"uint readVRAM16(uint address);\n                                                                   "    // l:2
"uint readVRAM32(uint address);\n                                                                   "    // l:3
"\n                                                                                                 "    // l:4
"uint readIOreg(uint address);\n                                                                    "    // l:5
"vec4 readPALentry(uint index);\n                                                                   "    // l:6
"float getDepth(uint BGCNT);\n                                                                      "    // l:7
"\n                                                                                                 "    // l:8
"vec4 mode4(uint x, uint y) {\n                                                                     "    // l:9
"    if (BG != 2u) {\n                                                                              "    // l:10
"        // only BG2 is drawn\n                                                                     "    // l:11
"        discard;\n                                                                                 "    // l:12
"    }\n                                                                                            "    // l:13
"\n                                                                                                 "    // l:14
"    uint DISPCNT = readIOreg(0x00u);\n                                                             "    // l:15
"\n                                                                                                 "    // l:16
"    if ((DISPCNT & 0x0400u) == 0u) {\n                                                             "    // l:17
"        // background 2 is disabled\n                                                              "    // l:18
"        discard;\n                                                                                 "    // l:19
"    }\n                                                                                            "    // l:20
"\n                                                                                                 "    // l:21
"    // offset is specified in DISPCNT\n                                                            "    // l:22
"    uint Offset = 0u;\n                                                                            "    // l:23
"    if ((DISPCNT & 0x0010u) != 0u) {\n                                                             "    // l:24
"        // offset\n                                                                                "    // l:25
"        Offset = 0xa000u;\n                                                                        "    // l:26
"    }\n                                                                                            "    // l:27
"\n                                                                                                 "    // l:28
"    uint VRAMAddr = (240u * y + x);\n                                                              "    // l:29
"    VRAMAddr += Offset;\n                                                                          "    // l:30
"    uint PaletteIndex = readVRAM8(VRAMAddr);\n                                                     "    // l:31
"\n                                                                                                 "    // l:32
"    vec4 Color = readPALentry(PaletteIndex);\n                                                     "    // l:33
"    uint BGCNT = readIOreg(0x0cu);\n                                                               "    // l:34
"    gl_FragDepth = getDepth(BGCNT);;\n                                                             "    // l:35
"\n                                                                                                 "    // l:36
"    // We already converted to BGR when buffering data\n                                           "    // l:37
"    return vec4(Color.rgb, 1.0);\n                                                                 "    // l:38
"}\n                                                                                                "    // l:39
"\n                                                                                                 "    // l:40
;


// ObjectFragmentShaderSource (from object_fragment.glsl, lines 2 to 284)
const char* ObjectFragmentShaderSource = 
"precision mediump float;\n                                                                         "    // l:1
"\n                                                                                                 "    // l:2
"#define attr0 x\n                                                                                  "    // l:3
"#define attr1 y\n                                                                                  "    // l:4
"#define attr2 z\n                                                                                  "    // l:5
"#define attr3 w\n                                                                                  "    // l:6
"\n                                                                                                 "    // l:7
"in vec2 InObjPos;\n                                                                                "    // l:8
"\n                                                                                                 "    // l:9
"flat in uvec4 OBJ;\n                                                                               "    // l:10
"flat in uint ObjWidth;\n                                                                           "    // l:11
"flat in uint ObjHeight;\n                                                                          "    // l:12
"\n                                                                                                 "    // l:13
"uniform bool Affine;\n                                                                             "    // l:14
"uniform uint YClipStart;\n                                                                         "    // l:15
"uniform uint YClipEnd;\n                                                                           "    // l:16
"\n                                                                                                 "    // l:17
"#ifdef OBJ_WINDOW\n                                                                                "    // l:18
"    out uvec4 FragColor;\n                                                                         "    // l:19
"#else\n                                                                                            "    // l:20
"    out vec4 FragColor;\n                                                                          "    // l:21
"#endif\n                                                                                           "    // l:22
"\n                                                                                                 "    // l:23
"vec4 ColorCorrect(vec4 color);\n                                                                   "    // l:24
"vec4 AlphaCorrect(vec4 color, uint layer, uint window);\n                                          "    // l:25
"\n                                                                                                 "    // l:26
"uint readVRAM8(uint address);\n                                                                    "    // l:27
"uint readVRAM16(uint address);\n                                                                   "    // l:28
"uint readVRAM32(uint address);\n                                                                   "    // l:29
"\n                                                                                                 "    // l:30
"uint readIOreg(uint address);\n                                                                    "    // l:31
"ivec4 readOAMentry(uint index);\n                                                                  "    // l:32
"vec4 readPALentry(uint index);\n                                                                   "    // l:33
"\n                                                                                                 "    // l:34
"uint getWindow(uint x, uint y);\n                                                                  "    // l:35
"\n                                                                                                 "    // l:36
"vec4 RegularObject(bool OAM2DMapping) {\n                                                          "    // l:37
"    uint TID = OBJ.attr2 & 0x03ffu;\n                                                              "    // l:38
"\n                                                                                                 "    // l:39
"    uint dx = uint(InObjPos.x);\n                                                                  "    // l:40
"    uint dy = uint(InObjPos.y);\n                                                                  "    // l:41
"\n                                                                                                 "    // l:42
"    // mosaic effect\n                                                                             "    // l:43
"    if ((OBJ.attr0 & 0x1000u) != 0u) {\n                                                           "    // l:44
"        uint MOSAIC = readIOreg(0x4cu);\n                                                          "    // l:45
"        dx -= dx % (((MOSAIC & 0xf00u) >> 8u) + 1u);\n                                             "    // l:46
"        dy -= dy % (((MOSAIC & 0xf000u) >> 12u) + 1u);\n                                           "    // l:47
"    }\n                                                                                            "    // l:48
"\n                                                                                                 "    // l:49
"    uint PixelAddress;\n                                                                           "    // l:50
"    if ((OBJ.attr0 & 0x2000u) == 0x0000u) {\n                                                      "    // l:51
"        uint PaletteBank = OBJ.attr2 >> 12u;\n                                                     "    // l:52
"        PixelAddress = TID << 5u;\n                                                                "    // l:53
"\n                                                                                                 "    // l:54
"        // get base address for line of tiles (vertically)\n                                       "    // l:55
"        if (OAM2DMapping) {\n                                                                      "    // l:56
"            PixelAddress += ObjWidth * (dy >> 3u) << 2u;\n                                         "    // l:57
"        }\n                                                                                        "    // l:58
"        else {\n                                                                                   "    // l:59
"            PixelAddress += 32u * 0x20u * (dy >> 3u);\n                                            "    // l:60
"        }\n                                                                                        "    // l:61
"        PixelAddress += (dy & 7u) << 2; // offset within tile for sliver\n                         "    // l:62
"\n                                                                                                 "    // l:63
"        // Sprites VRAM base address is 0x10000\n                                                  "    // l:64
"        PixelAddress = (PixelAddress & 0x7fffu) | 0x10000u;\n                                      "    // l:65
"\n                                                                                                 "    // l:66
"        // horizontal offset:\n                                                                    "    // l:67
"        PixelAddress += (dx >> 3u) << 5u;    // of tile\n                                          "    // l:68
"        PixelAddress += ((dx & 7u) >> 1u);  // in tile\n                                           "    // l:69
"\n                                                                                                 "    // l:70
"        uint VRAMEntry = readVRAM8(PixelAddress);\n                                                "    // l:71
"        if ((dx & 1u) != 0u) {\n                                                                   "    // l:72
"            // upper nibble\n                                                                      "    // l:73
"            VRAMEntry >>= 4u;\n                                                                    "    // l:74
"        }\n                                                                                        "    // l:75
"        else {\n                                                                                   "    // l:76
"            VRAMEntry &= 0x0fu;\n                                                                  "    // l:77
"        }\n                                                                                        "    // l:78
"\n                                                                                                 "    // l:79
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:80
"            return vec4(readPALentry(0x100u + (PaletteBank << 4u) + VRAMEntry).rgb, 1.0);\n        "    // l:81
"        }\n                                                                                        "    // l:82
"        else {\n                                                                                   "    // l:83
"            // transparent\n                                                                       "    // l:84
"            discard;\n                                                                             "    // l:85
"        }\n                                                                                        "    // l:86
"    }\n                                                                                            "    // l:87
"    else {\n                                                                                       "    // l:88
"        // 8bpp\n                                                                                  "    // l:89
"        PixelAddress = TID << 5;\n                                                                 "    // l:90
"\n                                                                                                 "    // l:91
"        if (OAM2DMapping) {\n                                                                      "    // l:92
"            PixelAddress += ObjWidth * (dy & ~7u);\n                                               "    // l:93
"        }\n                                                                                        "    // l:94
"        else {\n                                                                                   "    // l:95
"            PixelAddress += 32u * 0x20u * (dy >> 3u);\n                                            "    // l:96
"        }\n                                                                                        "    // l:97
"        PixelAddress += (dy & 7u) << 3;\n                                                          "    // l:98
"\n                                                                                                 "    // l:99
"        // Sprites VRAM base address is 0x10000\n                                                  "    // l:100
"        PixelAddress = (PixelAddress & 0x7fffu) | 0x10000u;\n                                      "    // l:101
"\n                                                                                                 "    // l:102
"        // horizontal offset:\n                                                                    "    // l:103
"        PixelAddress += (dx >> 3) << 6;\n                                                          "    // l:104
"        PixelAddress += dx & 7u;\n                                                                 "    // l:105
"\n                                                                                                 "    // l:106
"        uint VRAMEntry = readVRAM8(PixelAddress);\n                                                "    // l:107
"\n                                                                                                 "    // l:108
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:109
"            return vec4(readPALentry(0x100u + VRAMEntry).rgb, 1);\n                                "    // l:110
"        }\n                                                                                        "    // l:111
"        else {\n                                                                                   "    // l:112
"            // transparent\n                                                                       "    // l:113
"            discard;\n                                                                             "    // l:114
"        }\n                                                                                        "    // l:115
"    }\n                                                                                            "    // l:116
"}\n                                                                                                "    // l:117
"\n                                                                                                 "    // l:118
"bool InsideBox(vec2 v, vec2 bottomLeft, vec2 topRight) {\n                                         "    // l:119
"    vec2 s = step(bottomLeft, v) - step(topRight, v);\n                                            "    // l:120
"    return (s.x * s.y) != 0.0;\n                                                                   "    // l:121
"}\n                                                                                                "    // l:122
"\n                                                                                                 "    // l:123
"vec4 AffineObject(bool OAM2DMapping) {\n                                                           "    // l:124
"    uint TID = OBJ.attr2 & 0x03ffu;\n                                                              "    // l:125
"\n                                                                                                 "    // l:126
"    uint AffineIndex = (OBJ.attr1 & 0x3e00u) >> 9;\n                                               "    // l:127
"    AffineIndex <<= 2;  // goes in groups of 4\n                                                   "    // l:128
"\n                                                                                                 "    // l:129
"    // scaling parameters\n                                                                        "    // l:130
"    int PA = readOAMentry(AffineIndex).attr3;\n                                                    "    // l:131
"    int PB = readOAMentry(AffineIndex + 1u).attr3;\n                                               "    // l:132
"    int PC = readOAMentry(AffineIndex + 2u).attr3;\n                                               "    // l:133
"    int PD = readOAMentry(AffineIndex + 3u).attr3;\n                                               "    // l:134
"\n                                                                                                 "    // l:135
"    // reference point\n                                                                           "    // l:136
"    vec2 p0 = vec2(\n                                                                              "    // l:137
"        float(ObjWidth  >> 1),\n                                                                   "    // l:138
"        float(ObjHeight >> 1)\n                                                                    "    // l:139
"    );\n                                                                                           "    // l:140
"\n                                                                                                 "    // l:141
"    vec2 p1;\n                                                                                     "    // l:142
"    if ((OBJ.attr0 & 0x0300u) == 0x0300u) {\n                                                      "    // l:143
"        // double rendering\n                                                                      "    // l:144
"        p1 = 2.0 * p0;\n                                                                           "    // l:145
"    }\n                                                                                            "    // l:146
"    else {\n                                                                                       "    // l:147
"        p1 = p0;\n                                                                                 "    // l:148
"    }\n                                                                                            "    // l:149
"\n                                                                                                 "    // l:150
"    mat2x2 rotscale = mat2x2(\n                                                                    "    // l:151
"        float(PA), float(PC),\n                                                                    "    // l:152
"        float(PB), float(PD)\n                                                                     "    // l:153
"    ) / 256.0;  // fixed point stuff\n                                                             "    // l:154
"\n                                                                                                 "    // l:155
"    ivec2 pos = ivec2(rotscale * (InObjPos - p1) + p0);\n                                          "    // l:156
"    if (!InsideBox(vec2(pos), vec2(0.0, 0.0), vec2(ObjWidth, ObjHeight))) {\n                      "    // l:157
"        // out of bounds\n                                                                         "    // l:158
"        discard;\n                                                                                 "    // l:159
"    }\n                                                                                            "    // l:160
"\n                                                                                                 "    // l:161
"    // mosaic effect\n                                                                             "    // l:162
"    if ((OBJ.attr0 & 0x1000u) != 0u) {\n                                                           "    // l:163
"        uint MOSAIC = readIOreg(0x4cu);\n                                                          "    // l:164
"        pos.x -= pos.x % int(((MOSAIC & 0xf00u) >> 8u) + 1u);\n                                    "    // l:165
"        pos.y -= pos.y % int(((MOSAIC & 0xf000u) >> 12u) + 1u);\n                                  "    // l:166
"    }\n                                                                                            "    // l:167
"\n                                                                                                 "    // l:168
"    // get actual pixel\n                                                                          "    // l:169
"    uint PixelAddress = 0x10000u;  // OBJ VRAM starts at 0x10000 in VRAM\n                         "    // l:170
"    PixelAddress += TID << 5;\n                                                                    "    // l:171
"        if (OAM2DMapping) {\n                                                                      "    // l:172
"            PixelAddress += ObjWidth * uint(pos.y & ~7) >> 1u;\n                                   "    // l:173
"        }\n                                                                                        "    // l:174
"        else {\n                                                                                   "    // l:175
"            PixelAddress += 32u * 0x20u * uint(pos.y >> 3);\n                                      "    // l:176
"        }\n                                                                                        "    // l:177
"\n                                                                                                 "    // l:178
"    // the rest is very similar to regular sprites:\n                                              "    // l:179
"    if ((OBJ.attr0 & 0x2000u) == 0x0000u) {\n                                                      "    // l:180
"        uint PaletteBank = OBJ.attr2 >> 12u;\n                                                     "    // l:181
"        PixelAddress += uint(pos.y & 7) << 2u; // offset within tile for sliver\n                  "    // l:182
"\n                                                                                                 "    // l:183
"        // horizontal offset:\n                                                                    "    // l:184
"        PixelAddress += uint(pos.x >> 3) << 5u;    // of tile\n                                    "    // l:185
"        PixelAddress += uint(pos.x & 7) >> 1u;  // in tile\n                                       "    // l:186
"\n                                                                                                 "    // l:187
"        uint VRAMEntry = readVRAM8(PixelAddress);\n                                                "    // l:188
"        if ((pos.x & 1) != 0) {\n                                                                  "    // l:189
"            // upper nibble\n                                                                      "    // l:190
"            VRAMEntry >>= 4u;\n                                                                    "    // l:191
"        }\n                                                                                        "    // l:192
"        else {\n                                                                                   "    // l:193
"            VRAMEntry &= 0x0fu;\n                                                                  "    // l:194
"        }\n                                                                                        "    // l:195
"\n                                                                                                 "    // l:196
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:197
"            return vec4(readPALentry(0x100u + (PaletteBank << 4u) + VRAMEntry).rgb, 1);\n          "    // l:198
"        }\n                                                                                        "    // l:199
"        else {\n                                                                                   "    // l:200
"            // transparent\n                                                                       "    // l:201
"            discard;\n                                                                             "    // l:202
"        }\n                                                                                        "    // l:203
"    }\n                                                                                            "    // l:204
"    else {\n                                                                                       "    // l:205
"        PixelAddress += (uint(pos.y) & 7u) << 3u; // offset within tile for sliver\n               "    // l:206
"\n                                                                                                 "    // l:207
"        // horizontal offset:\n                                                                    "    // l:208
"        PixelAddress += uint(pos.x >> 3) << 6u;  // of tile\n                                      "    // l:209
"        PixelAddress += uint(pos.x & 7);        // in tile\n                                       "    // l:210
"\n                                                                                                 "    // l:211
"        uint VRAMEntry = readVRAM8(PixelAddress);\n                                                "    // l:212
"\n                                                                                                 "    // l:213
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:214
"            return vec4(readPALentry(0x100u + VRAMEntry).rgb, 1);\n                                "    // l:215
"        }\n                                                                                        "    // l:216
"        else {\n                                                                                   "    // l:217
"            // transparent\n                                                                       "    // l:218
"            discard;\n                                                                             "    // l:219
"        }\n                                                                                        "    // l:220
"    }\n                                                                                            "    // l:221
"}\n                                                                                                "    // l:222
"\n                                                                                                 "    // l:223
"void main() {\n                                                                                    "    // l:224
"    if (OnScreenPos.x < 0.0) {\n                                                                   "    // l:225
"        discard;\n                                                                                 "    // l:226
"    }\n                                                                                            "    // l:227
"    if (OnScreenPos.x > float(240u)) {\n                                                           "    // l:228
"        discard;\n                                                                                 "    // l:229
"    }\n                                                                                            "    // l:230
"\n                                                                                                 "    // l:231
"    if (OnScreenPos.y < float(YClipStart)) {\n                                                     "    // l:232
"        discard;\n                                                                                 "    // l:233
"    }\n                                                                                            "    // l:234
"    if (OnScreenPos.y > float(YClipEnd)) {\n                                                       "    // l:235
"        discard;\n                                                                                 "    // l:236
"    }\n                                                                                            "    // l:237
"\n                                                                                                 "    // l:238
"    uint DISPCNT = readIOreg(0x00u);\n                                                             "    // l:239
"#ifndef OBJ_WINDOW\n                                                                               "    // l:240
"    if ((DISPCNT & 0x1000u) == 0u) {\n                                                             "    // l:241
"        // objects disabled in this scanline\n                                                     "    // l:242
"        discard;\n                                                                                 "    // l:243
"    }\n                                                                                            "    // l:244
"    uint window = getWindow(uint(OnScreenPos.x), uint(OnScreenPos.y));\n                           "    // l:245
"    if ((window & 0x10u) == 0u) {\n                                                                "    // l:246
"        // disabled by window\n                                                                    "    // l:247
"        discard;\n                                                                                 "    // l:248
"    }\n                                                                                            "    // l:249
"#else\n                                                                                            "    // l:250
"    if ((DISPCNT & 0x8000u) == 0u) {\n                                                             "    // l:251
"        // object window disabled in this scanline\n                                               "    // l:252
"        discard;\n                                                                                 "    // l:253
"    }\n                                                                                            "    // l:254
"#endif\n                                                                                           "    // l:255
"\n                                                                                                 "    // l:256
"    bool OAM2DMapping = (DISPCNT & (0x0040u)) != 0u;\n                                             "    // l:257
"\n                                                                                                 "    // l:258
"    vec4 Color;\n                                                                                  "    // l:259
"    if (!Affine) {\n                                                                               "    // l:260
"        Color = RegularObject(OAM2DMapping);\n                                                     "    // l:261
"    }\n                                                                                            "    // l:262
"    else{\n                                                                                        "    // l:263
"        Color = AffineObject(OAM2DMapping);\n                                                      "    // l:264
"    }\n                                                                                            "    // l:265
"\n                                                                                                 "    // l:266
"#ifndef OBJ_WINDOW\n                                                                               "    // l:267
"    FragColor = ColorCorrect(Color);\n                                                             "    // l:268
"    if ((OBJ.attr0 & 0x0c00u) == 0x0400u) {\n                                                      "    // l:269
"        FragColor = AlphaCorrect(FragColor, 4u, window);\n                                         "    // l:270
"    }\n                                                                                            "    // l:271
"    else {\n                                                                                       "    // l:272
"        FragColor = vec4(FragColor.rgb, -1);\n                                                     "    // l:273
"    }\n                                                                                            "    // l:274
"#else\n                                                                                            "    // l:275
"    // RegularObject/AffineObject will only return if it is nontransparent\n                       "    // l:276
"    uint WINOBJ = (readIOreg(0x4au) >> 8u) & 0x3fu;\n                                              "    // l:277
"\n                                                                                                 "    // l:278
"    FragColor.r = WINOBJ;\n                                                                        "    // l:279
"#endif\n                                                                                           "    // l:280
"}\n                                                                                                "    // l:281
"\n                                                                                                 "    // l:282
;


// ObjectVertexShaderSource (from object_vertex.glsl, lines 2 to 123)
const char* ObjectVertexShaderSource = 
"precision mediump float;\n                                                                         "    // l:1
"\n                                                                                                 "    // l:2
"#define attr0 x\n                                                                                  "    // l:3
"#define attr1 y\n                                                                                  "    // l:4
"#define attr2 z\n                                                                                  "    // l:5
"#define attr3 w\n                                                                                  "    // l:6
"\n                                                                                                 "    // l:7
"uniform bool Affine;\n                                                                             "    // l:8
"\n                                                                                                 "    // l:9
"layout (location = 0) in uvec4 InOBJ;\n                                                            "    // l:10
"\n                                                                                                 "    // l:11
"out vec2 InObjPos;\n                                                                               "    // l:12
"out vec2 OnScreenPos;\n                                                                            "    // l:13
"flat out uvec4 OBJ;\n                                                                              "    // l:14
"flat out uint ObjWidth;\n                                                                          "    // l:15
"flat out uint ObjHeight;\n                                                                         "    // l:16
"\n                                                                                                 "    // l:17
"struct s_ObjSize {\n                                                                               "    // l:18
"    uint width;\n                                                                                  "    // l:19
"    uint height;\n                                                                                 "    // l:20
"};\n                                                                                               "    // l:21
"\n                                                                                                 "    // l:22
"const s_ObjSize ObjSizeTable[12] = s_ObjSize[](\n                                                  "    // l:23
"    s_ObjSize(8u, 8u),  s_ObjSize(16u, 16u), s_ObjSize(32u, 32u), s_ObjSize(64u, 64u),\n           "    // l:24
"    s_ObjSize(16u, 8u), s_ObjSize(32u, 8u),  s_ObjSize(32u, 16u), s_ObjSize(64u, 32u),\n           "    // l:25
"    s_ObjSize(8u, 16u), s_ObjSize(8u, 32u),  s_ObjSize(16u, 32u), s_ObjSize(32u, 62u)\n            "    // l:26
");\n                                                                                               "    // l:27
"\n                                                                                                 "    // l:28
"struct s_Position {\n                                                                              "    // l:29
"    bool right;\n                                                                                  "    // l:30
"    bool low;\n                                                                                    "    // l:31
"};\n                                                                                               "    // l:32
"\n                                                                                                 "    // l:33
"const s_Position PositionTable[4] = s_Position[](\n                                                "    // l:34
"    s_Position(false, false), s_Position(true, false), s_Position(true, true), s_Position(false, true)\n"    // l:35
");\n                                                                                               "    // l:36
"\n                                                                                                 "    // l:37
"void main() {\n                                                                                    "    // l:38
"    OBJ = InOBJ;\n                                                                                 "    // l:39
"    s_Position Position = PositionTable[gl_VertexID & 3];\n                                        "    // l:40
"\n                                                                                                 "    // l:41
"    uint Shape = OBJ.attr0 >> 14;\n                                                                "    // l:42
"    uint Size  = OBJ.attr1 >> 14;\n                                                                "    // l:43
"\n                                                                                                 "    // l:44
"    s_ObjSize ObjSize = ObjSizeTable[(Shape * 4u) + Size];\n                                       "    // l:45
"    ObjWidth = ObjSize.width;\n                                                                    "    // l:46
"    ObjHeight = ObjSize.height;\n                                                                  "    // l:47
"\n                                                                                                 "    // l:48
"    ivec2 ScreenPos = ivec2(OBJ.attr1 & 0x1ffu, OBJ.attr0 & 0xffu);\n                              "    // l:49
"\n                                                                                                 "    // l:50
"    // correct position for screen wrapping\n                                                      "    // l:51
"    if (ScreenPos.x > int(240u)) {\n                                                               "    // l:52
"        ScreenPos.x -= 0x200;\n                                                                    "    // l:53
"    }\n                                                                                            "    // l:54
"\n                                                                                                 "    // l:55
"    if (ScreenPos.y > int(160u)) {\n                                                               "    // l:56
"        ScreenPos.y -= 0x100;\n                                                                    "    // l:57
"    }\n                                                                                            "    // l:58
"\n                                                                                                 "    // l:59
"    InObjPos = vec2(0.0, 0.0);\n                                                                   "    // l:60
"    if (Position.right) {\n                                                                        "    // l:61
"        InObjPos.x  += float(ObjWidth);\n                                                          "    // l:62
"        ScreenPos.x += int(ObjWidth);\n                                                            "    // l:63
"\n                                                                                                 "    // l:64
"        if (Affine) {\n                                                                            "    // l:65
"            if ((OBJ.attr0 & 0x0300u) == 0x0300u) {\n                                              "    // l:66
"                // double rendering\n                                                              "    // l:67
"                InObjPos.x  += float(ObjWidth);\n                                                  "    // l:68
"                ScreenPos.x += int(ObjWidth);\n                                                    "    // l:69
"            }\n                                                                                    "    // l:70
"        }\n                                                                                        "    // l:71
"    }\n                                                                                            "    // l:72
"\n                                                                                                 "    // l:73
"    if (Position.low) {\n                                                                          "    // l:74
"        InObjPos.y  += float(ObjHeight);\n                                                         "    // l:75
"        ScreenPos.y += int(ObjHeight);\n                                                           "    // l:76
"\n                                                                                                 "    // l:77
"        if (Affine) {\n                                                                            "    // l:78
"            if ((OBJ.attr0 & 0x0300u) == 0x0300u) {\n                                              "    // l:79
"                // double rendering\n                                                              "    // l:80
"                InObjPos.y  += float(ObjHeight);\n                                                 "    // l:81
"                ScreenPos.y += int(ObjHeight);\n                                                   "    // l:82
"            }\n                                                                                    "    // l:83
"        }\n                                                                                        "    // l:84
"    }\n                                                                                            "    // l:85
"\n                                                                                                 "    // l:86
"    // flipping only applies to regular sprites\n                                                  "    // l:87
"    if (!Affine) {\n                                                                               "    // l:88
"        if ((OBJ.attr1 & 0x2000u) != 0u) {\n                                                       "    // l:89
"            // VFlip\n                                                                             "    // l:90
"            InObjPos.y = float(ObjHeight) - InObjPos.y;\n                                          "    // l:91
"        }\n                                                                                        "    // l:92
"\n                                                                                                 "    // l:93
"        if ((OBJ.attr1 & 0x1000u) != 0u) {\n                                                       "    // l:94
"            // HFlip\n                                                                             "    // l:95
"            InObjPos.x = float(ObjWidth) - InObjPos.x;\n                                           "    // l:96
"        }\n                                                                                        "    // l:97
"    }\n                                                                                            "    // l:98
"\n                                                                                                 "    // l:99
"    OnScreenPos = vec2(ScreenPos);\n                                                               "    // l:100
"\n                                                                                                 "    // l:101
"#ifndef OBJ_WINDOW\n                                                                               "    // l:102
"    // depth is the same everywhere in the object anyway\n                                         "    // l:103
"    uint Priority = (OBJ.attr2 & 0x0c00u) >> 10;\n                                                 "    // l:104
"\n                                                                                                 "    // l:105
"    gl_Position = vec4(\n                                                                          "    // l:106
"        -1.0 + 2.0 * OnScreenPos.x / float(240u),\n                                                "    // l:107
"        1.0 - 2.0 * OnScreenPos.y / float(160u),\n                                                 "    // l:108
"        -1.0 + float(Priority) / 2.0,  // /2.0 because openGL clips between -1 and 1 (-1 is in front)\n"    // l:109
"        1.0\n                                                                                      "    // l:110
"    );\n                                                                                           "    // l:111
"#else\n                                                                                            "    // l:112
"    gl_Position = vec4(\n                                                                          "    // l:113
"        -1.0 + 2.0 * OnScreenPos.x / float(240u),\n                                                "    // l:114
"        1.0 - 2.0 * OnScreenPos.y / float(160u),\n                                                 "    // l:115
"        0.5,  // between WIN1 and WINOUT\n                                                         "    // l:116
"        1.0\n                                                                                      "    // l:117
"    );\n                                                                                           "    // l:118
"#endif\n                                                                                           "    // l:119
"}\n                                                                                                "    // l:120
"\n                                                                                                 "    // l:121
;


// VertexShaderSource (from vertex.glsl, lines 1 to 24)
const char* VertexShaderSource = 
"#version 320 es\n                                                                                  "    // l:1
"precision mediump float;\n                                                                         "    // l:2
"\n                                                                                                 "    // l:3
"layout (location = 0) in vec2 position;\n                                                          "    // l:4
"\n                                                                                                 "    // l:5
"out vec2 screenCoord;\n                                                                            "    // l:6
"out vec2 OnScreenPos;  // needed for fragment_helpers\n                                            "    // l:7
"\n                                                                                                 "    // l:8
"void main() {\n                                                                                    "    // l:9
"    // convert y coordinate from scanline to screen coordinate\n                                   "    // l:10
"    gl_Position = vec4(\n                                                                          "    // l:11
"        position.x,\n                                                                              "    // l:12
"        1.0 - (2.0 * position.y) / float(160u), 0.0, 1.0\n                                         "    // l:13
"    );\n                                                                                           "    // l:14
"\n                                                                                                 "    // l:15
"    screenCoord = vec2(\n                                                                          "    // l:16
"        float(240u) * (1.0 + position.x) / 2.0,\n                                                  "    // l:17
"        position.y\n                                                                               "    // l:18
"    );\n                                                                                           "    // l:19
"\n                                                                                                 "    // l:20
"    OnScreenPos = screenCoord;\n                                                                   "    // l:21
"}\n                                                                                                "    // l:22
"\n                                                                                                 "    // l:23
;


// WindowFragmentShaderSource (from window_fragment.glsl, lines 1 to 82)
const char* WindowFragmentShaderSource = 
"#version 320 es\n                                                                                  "    // l:1
"precision mediump float;\n                                                                         "    // l:2
"\n                                                                                                 "    // l:3
"in vec2 screenCoord;\n                                                                             "    // l:4
"\n                                                                                                 "    // l:5
"out uvec4 FragColor;\n                                                                             "    // l:6
"\n                                                                                                 "    // l:7
"uint readVRAM8(uint address);\n                                                                    "    // l:8
"uint readVRAM16(uint address);\n                                                                   "    // l:9
"uint readVRAM32(uint address);\n                                                                   "    // l:10
"\n                                                                                                 "    // l:11
"uint readIOreg(uint address);\n                                                                    "    // l:12
"vec4 readPALentry(uint index);\n                                                                   "    // l:13
"\n                                                                                                 "    // l:14
"void main() {\n                                                                                    "    // l:15
"    uint DISPCNT = readIOreg(0x00u);\n                                                             "    // l:16
"\n                                                                                                 "    // l:17
"    if ((DISPCNT & 0xe000u) == 0u) {\n                                                             "    // l:18
"        // windows are disabled, enable all windows\n                                              "    // l:19
"        // we should have caught this before rendering, but eh, I guess we'll check again...\n     "    // l:20
"        FragColor.x = 0x3fu;\n                                                                     "    // l:21
"        gl_FragDepth = 1.0;\n                                                                      "    // l:22
"        return;\n                                                                                  "    // l:23
"    }\n                                                                                            "    // l:24
"\n                                                                                                 "    // l:25
"    uint x = clamp(uint(screenCoord.x), 0u, 240u );\n                                              "    // l:26
"    uint y = clamp(uint(screenCoord.y), 0u, 160u); \n                                              "    // l:27
"\n                                                                                                 "    // l:28
"    // window 0 has higher priority\n                                                              "    // l:29
"    for (uint window = 0u; window < 2u; window++) {\n                                              "    // l:30
"        if ((DISPCNT & (0x2000u << window)) == 0u) {\n                                             "    // l:31
"            // window disabled\n                                                                   "    // l:32
"            continue;\n                                                                            "    // l:33
"        }\n                                                                                        "    // l:34
"\n                                                                                                 "    // l:35
"        uint WINH = readIOreg(0x40u + 2u * window);\n                                              "    // l:36
"        uint WINV = readIOreg(0x44u + 2u * window);\n                                              "    // l:37
"        uint WININ = (readIOreg(0x48u) >> (window * 8u)) & 0x3fu;\n                                "    // l:38
"\n                                                                                                 "    // l:39
"        uint X1 = clamp(WINH >> 8u, 0u, 240u );\n                                                  "    // l:40
"        uint X2 = clamp(WINH & 0xffu, 0u, 240u );\n                                                "    // l:41
"        uint Y1 = clamp(WINV >> 8u, 0u, 160u);\n                                                   "    // l:42
"        uint Y2 = clamp(WINV & 0xffu, 0u, 160u);\n                                                 "    // l:43
"\n                                                                                                 "    // l:44
"        if (Y1 <= Y2) {\n                                                                          "    // l:45
"            // no vert wrap and out of bounds, continue\n                                          "    // l:46
"            if (y < Y1 || y >= Y2) {\n                                                             "    // l:47
"                continue;\n                                                                        "    // l:48
"            }\n                                                                                    "    // l:49
"        }\n                                                                                        "    // l:50
"        else {\n                                                                                   "    // l:51
"            // vert wrap and \"in bounds\":\n                                                      "    // l:52
"            if ((y < Y1) && (y >= Y2)) {\n                                                         "    // l:53
"                continue;\n                                                                        "    // l:54
"            }\n                                                                                    "    // l:55
"        }\n                                                                                        "    // l:56
"\n                                                                                                 "    // l:57
"        if (X1 <= X2) {\n                                                                          "    // l:58
"            // no hor wrap\n                                                                       "    // l:59
"            if (x >= X1 && x < X2) {\n                                                             "    // l:60
"                // pixel in WININ\n                                                                "    // l:61
"                FragColor.x = WININ;\n                                                             "    // l:62
"                gl_FragDepth = 0.0;\n                                                              "    // l:63
"                return;\n                                                                          "    // l:64
"            }\n                                                                                    "    // l:65
"        }\n                                                                                        "    // l:66
"        else {\n                                                                                   "    // l:67
"            // hor wrap\n                                                                          "    // l:68
"            if (x < X2 || x >= X1) {\n                                                             "    // l:69
"                // pixel in WININ\n                                                                "    // l:70
"                FragColor.x = WININ;\n                                                             "    // l:71
"                gl_FragDepth = 0.0;\n                                                              "    // l:72
"                return;\n                                                                          "    // l:73
"            }\n                                                                                    "    // l:74
"        }\n                                                                                        "    // l:75
"    }\n                                                                                            "    // l:76
"\n                                                                                                 "    // l:77
"    FragColor.x = readIOreg(0x4au) & 0x3fu;  // WINOUT\n                                           "    // l:78
"    gl_FragDepth = 1.0;\n                                                                          "    // l:79
"}\n                                                                                                "    // l:80
"\n                                                                                                 "    // l:81
;

#endif  // GC__SHADER_H