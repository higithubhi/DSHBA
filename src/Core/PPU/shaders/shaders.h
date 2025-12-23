#ifndef GC__SHADER_H
#define GC__SHADER_H

// BlitFragmentShaderSource (from blit_fragment.glsl, lines 1 to 35)
const char* BlitFragmentShaderSource = 
"#version 320 es\n                                                                                  "    // l:1
"precision mediump float;\n                                                                         "    // l:2
"precision highp int;\n                                                                             "    // l:3
"precision mediump usampler2D;\n                                                                    "    // l:4
"precision mediump isampler2D;\n                                                                    "    // l:5
"\n                                                                                                 "    // l:6
"in vec2 texCoord;\n                                                                                "    // l:7
"\n                                                                                                 "    // l:8
"uniform sampler2D TopLayer;\n                                                                      "    // l:9
"uniform sampler2D BottomLayer;\n                                                                   "    // l:10
"\n                                                                                                 "    // l:11
"out vec4 FragColor;\n                                                                              "    // l:12
"\n                                                                                                 "    // l:13
"void main() {\n                                                                                    "    // l:14
"    vec4 top = texture(TopLayer, texCoord);\n                                                      "    // l:15
"    vec4 bottom = texture(BottomLayer, texCoord);\n                                                "    // l:16
"\n                                                                                                 "    // l:17
"    // default: pick top\n                                                                         "    // l:18
"    FragColor = vec4(\n                                                                            "    // l:19
"        top.rgb, 1.0\n                                                                             "    // l:20
"    );\n                                                                                           "    // l:21
"\n                                                                                                 "    // l:22
"    if ((bottom.a != 0.0) && (bottom.a <= 0.5)) {\n                                                "    // l:23
"        if (top.a >= 0.5) {\n                                                                      "    // l:24
"            float topASNorm = (top.a * 2.0) - 1.0;\n                                               "    // l:25
"            float bottomASNorm = (bottom.a * 2.0) - 1.0;\n                                         "    // l:26
"            vec3 blendedColor = top.rgb * topASNorm - 2.0 * bottom.rgb * (bottomASNorm + 0.25);\n  "    // l:27
"            FragColor = vec4(\n                                                                    "    // l:28
"                blendedColor, 1.0\n                                                                "    // l:29
"            );\n                                                                                   "    // l:30
"        }\n                                                                                        "    // l:31
"    }\n                                                                                            "    // l:32
"}\n                                                                                                "    // l:33
"\n                                                                                                 "    // l:34
;


// BlitVertexShaderSource (from blit_vertex.glsl, lines 1 to 23)
const char* BlitVertexShaderSource = 
"#version 320 es\n                                                                                  "    // l:1
"precision mediump float;\n                                                                         "    // l:2
"precision highp int;\n                                                                             "    // l:3
"precision mediump usampler2D;\n                                                                    "    // l:4
"precision mediump isampler2D;\n                                                                    "    // l:5
"\n                                                                                                 "    // l:6
"layout (location = 0) in vec2 position;\n                                                          "    // l:7
"layout (location = 1) in vec2 inTexCoord;\n                                                        "    // l:8
"\n                                                                                                 "    // l:9
"out vec2 texCoord;  // needed for fragment_helpers\n                                               "    // l:10
"\n                                                                                                 "    // l:11
"void main() {\n                                                                                    "    // l:12
"    texCoord = inTexCoord;\n                                                                       "    // l:13
"\n                                                                                                 "    // l:14
"    gl_Position = vec4(\n                                                                          "    // l:15
"        position.x,\n                                                                              "    // l:16
"        position.y,\n                                                                              "    // l:17
"        0,\n                                                                                       "    // l:18
"        1\n                                                                                        "    // l:19
"    );\n                                                                                           "    // l:20
"}\n                                                                                                "    // l:21
"\n                                                                                                 "    // l:22
;


// FragmentShaderSource (from fragment.glsl, lines 1 to 304)
const char* FragmentShaderSource = 
"#version 320 es\n                                                                                  "    // l:1
"precision mediump float;\n                                                                         "    // l:2
"precision highp int;\n                                                                             "    // l:3
"precision mediump usampler2D;\n                                                                    "    // l:4
"precision mediump isampler2D;\n                                                                    "    // l:5
"\n                                                                                                 "    // l:6
"in vec2 screenCoord;\n                                                                             "    // l:7
"\n                                                                                                 "    // l:8
"out vec4 FragColor;\n                                                                              "    // l:9
"uniform uint ReferenceLine2[160u];\n                                                               "    // l:10
"uniform uint ReferenceLine3[160u];\n                                                               "    // l:11
"\n                                                                                                 "    // l:12
"// BG 0 - 3 or 4 for backdrop\n                                                                    "    // l:13
"uniform uint BG;\n                                                                                 "    // l:14
"\n                                                                                                 "    // l:15
"vec4 ColorCorrect(vec4 color);\n                                                                   "    // l:16
"void CheckBottom(uint layer, uint window);\n                                                       "    // l:17
"vec4 AlphaCorrect(vec4 color, uint layer, uint window);\n                                          "    // l:18
"\n                                                                                                 "    // l:19
"uint readVRAM8(uint address);\n                                                                    "    // l:20
"uint readVRAM16(uint address);\n                                                                   "    // l:21
"\n                                                                                                 "    // l:22
"uint readVRAM32(uint address);\n                                                                   "    // l:23
"uint readIOreg(uint address);\n                                                                    "    // l:24
"vec4 readPALentry(uint index);\n                                                                   "    // l:25
"\n                                                                                                 "    // l:26
"uint getWindow(uint x, uint y);\n                                                                  "    // l:27
"\n                                                                                                 "    // l:28
"float getDepth(uint BGCNT) {\n                                                                     "    // l:29
"    return ((2.0 * float(BGCNT & 3u)) / 8.0) + (float(1u + BG) / 32.0);\n                          "    // l:30
"}\n                                                                                                "    // l:31
"\n                                                                                                 "    // l:32
"uint VRAMIndex(uint Tx, uint Ty, uint Size) {\n                                                    "    // l:33
"    uint temp = ((Ty & 0x1fu) << 6);\n                                                             "    // l:34
"    temp |= temp | ((Tx & 0x1fu) << 1);\n                                                          "    // l:35
"    switch (Size) {\n                                                                              "    // l:36
"        case 0u:  // 32x32\n                                                                       "    // l:37
"            break;\n                                                                               "    // l:38
"        case 1u:  // 64x32\n                                                                       "    // l:39
"            if ((Tx & 0x20u) != 0u) {\n                                                            "    // l:40
"                temp |= 0x800u;\n                                                                  "    // l:41
"            }\n                                                                                    "    // l:42
"            break;\n                                                                               "    // l:43
"        case 2u:  // 32x64\n                                                                       "    // l:44
"            if ((Ty & 0x20u) != 0u) {\n                                                            "    // l:45
"                temp |= 0x800u;\n                                                                  "    // l:46
"            }\n                                                                                    "    // l:47
"            break;\n                                                                               "    // l:48
"        case 3u:  // 64x64\n                                                                       "    // l:49
"            if ((Ty & 0x20u) != 0u) {\n                                                            "    // l:50
"                temp |= 0x1000u;\n                                                                 "    // l:51
"            }\n                                                                                    "    // l:52
"            if ((Tx & 0x20u) != 0u) {\n                                                            "    // l:53
"                temp |= 0x800u;\n                                                                  "    // l:54
"            }\n                                                                                    "    // l:55
"            break;\n                                                                               "    // l:56
"        default:\n                                                                                 "    // l:57
"            // invalid, should not happen\n                                                        "    // l:58
"            return 0u;\n                                                                           "    // l:59
"    }\n                                                                                            "    // l:60
"    return temp;\n                                                                                 "    // l:61
"}\n                                                                                                "    // l:62
"\n                                                                                                 "    // l:63
"vec4 regularScreenEntryPixel(uint dx, uint dy, uint ScreenEntry, uint CBB, bool ColorMode) {\n     "    // l:64
"    uint PaletteBank = ScreenEntry >> 12;  // 16 bits, we need top 4\n                             "    // l:65
"    if ((ScreenEntry & 0x0800u) != 0u) {\n                                                         "    // l:66
"        // VFlip\n                                                                                 "    // l:67
"        dy = 7u - dy;\n                                                                            "    // l:68
"    }\n                                                                                            "    // l:69
"\n                                                                                                 "    // l:70
"    if ((ScreenEntry & 0x0400u) != 0u) {\n                                                         "    // l:71
"        // HFlip\n                                                                                 "    // l:72
"        dx = 7u - dx;\n                                                                            "    // l:73
"    }\n                                                                                            "    // l:74
"\n                                                                                                 "    // l:75
"    uint TID     = ScreenEntry & 0x3ffu;\n                                                         "    // l:76
"    uint Address = CBB << 14;\n                                                                    "    // l:77
"\n                                                                                                 "    // l:78
"    if (!ColorMode) {\n                                                                            "    // l:79
"        // 4bpp\n                                                                                  "    // l:80
"        Address += TID << 5; // beginning of tile\n                                                "    // l:81
"        Address += dy << 2;  // beginning of sliver\n                                              "    // l:82
"\n                                                                                                 "    // l:83
"        Address += dx >> 1;  // offset into sliver\n                                               "    // l:84
"        uint VRAMEntry = readVRAM8(Address);\n                                                     "    // l:85
"        if ((dx & 1u) != 0u) {\n                                                                   "    // l:86
"            VRAMEntry >>= 4;     // odd x, upper nibble\n                                          "    // l:87
"        }\n                                                                                        "    // l:88
"        else {\n                                                                                   "    // l:89
"            VRAMEntry &= 0xfu;  // even x, lower nibble\n                                          "    // l:90
"        }\n                                                                                        "    // l:91
"\n                                                                                                 "    // l:92
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:93
"            return vec4(readPALentry((PaletteBank << 4) + VRAMEntry).rgb, 1.0);\n                  "    // l:94
"        }\n                                                                                        "    // l:95
"    }\n                                                                                            "    // l:96
"    else {\n                                                                                       "    // l:97
"        // 8bpp\n                                                                                  "    // l:98
"        Address += TID << 6; // beginning of tile\n                                                "    // l:99
"        Address += dy << 3;  // beginning of sliver\n                                              "    // l:100
"\n                                                                                                 "    // l:101
"        Address += dx;       // offset into sliver\n                                               "    // l:102
"        uint VRAMEntry = readVRAM8(Address);\n                                                     "    // l:103
"\n                                                                                                 "    // l:104
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:105
"            return vec4(readPALentry(VRAMEntry).rgb, 1.0);\n                                       "    // l:106
"        }\n                                                                                        "    // l:107
"    }\n                                                                                            "    // l:108
"\n                                                                                                 "    // l:109
"    // transparent\n                                                                               "    // l:110
"    discard;\n                                                                                     "    // l:111
"}\n                                                                                                "    // l:112
"\n                                                                                                 "    // l:113
"vec4 regularBGPixel(uint BGCNT, uint x, uint y) {\n                                                "    // l:114
"    uint HOFS, VOFS;\n                                                                             "    // l:115
"    uint CBB, SBB, Size;\n                                                                         "    // l:116
"    bool ColorMode;\n                                                                              "    // l:117
"\n                                                                                                 "    // l:118
"    HOFS  = readIOreg(0x10u + (BG << 2)) & 0x1ffu;\n                                               "    // l:119
"    VOFS  = readIOreg(0x12u + (BG << 2)) & 0x1ffu;\n                                               "    // l:120
"\n                                                                                                 "    // l:121
"    CBB       = (BGCNT >> 2) & 3u;\n                                                               "    // l:122
"    ColorMode = (BGCNT & 0x0080u) == 0x0080u;  // 0: 4bpp, 1: 8bpp\n                               "    // l:123
"    SBB       = (BGCNT >> 8) & 0x1fu;\n                                                            "    // l:124
"    Size      = (BGCNT >> 14) & 3u;\n                                                              "    // l:125
"\n                                                                                                 "    // l:126
"    uint x_eff = (x + HOFS) & 0xffffu;\n                                                           "    // l:127
"    uint y_eff = (y + VOFS) & 0xffffu;\n                                                           "    // l:128
"\n                                                                                                 "    // l:129
"    // mosaic effect\n                                                                             "    // l:130
"    if ((BGCNT & 0x0040u) != 0u) {\n                                                               "    // l:131
"        uint MOSAIC = readIOreg(0x4cu);\n                                                          "    // l:132
"        x_eff -= x_eff % ((MOSAIC & 0xfu) + 1u);\n                                                 "    // l:133
"        y_eff -= y_eff % (((MOSAIC & 0xf0u) >> 4) + 1u);\n                                         "    // l:134
"    }\n                                                                                            "    // l:135
"\n                                                                                                 "    // l:136
"    uint ScreenEntryIndex = VRAMIndex(x_eff >> 3u, y_eff >> 3u, Size);\n                           "    // l:137
"    ScreenEntryIndex += (SBB << 11u);\n                                                            "    // l:138
"    uint ScreenEntry = readVRAM16(ScreenEntryIndex);  // always halfword aligned\n                 "    // l:139
"\n                                                                                                 "    // l:140
"    return regularScreenEntryPixel(x_eff & 7u, y_eff & 7u, ScreenEntry, CBB, ColorMode);\n         "    // l:141
"}\n                                                                                                "    // l:142
"\n                                                                                                 "    // l:143
"const uint AffineBGSizeTable[4] = uint[](\n                                                        "    // l:144
"    128u, 256u, 512u, 1024u\n                                                                      "    // l:145
");\n                                                                                               "    // l:146
"\n                                                                                                 "    // l:147
"vec4 affineBGPixel(uint BGCNT, vec2 screen_pos) {\n                                                "    // l:148
"    uint x = uint(screen_pos.x);\n                                                                 "    // l:149
"    uint y = uint(screen_pos.y);\n                                                                 "    // l:150
"\n                                                                                                 "    // l:151
"    uint ReferenceLine;\n                                                                          "    // l:152
"    uint BGX_raw, BGY_raw;\n                                                                       "    // l:153
"    int PA, PB, PC, PD;\n                                                                          "    // l:154
"    if (BG == 2u) {\n                                                                              "    // l:155
"        ReferenceLine = ReferenceLine2[y];\n                                                       "    // l:156
"\n                                                                                                 "    // l:157
"        BGX_raw  = readIOreg(0x28u);\n                                                             "    // l:158
"        BGX_raw |= readIOreg(0x2au) << 16;\n                                                       "    // l:159
"        BGY_raw  = readIOreg(0x2cu);\n                                                             "    // l:160
"        BGY_raw |= readIOreg(0x2eu) << 16;\n                                                       "    // l:161
"        PA = int(readIOreg(0x20u)) << 16;\n                                                        "    // l:162
"        PB = int(readIOreg(0x22u)) << 16;\n                                                        "    // l:163
"        PC = int(readIOreg(0x24u)) << 16;\n                                                        "    // l:164
"        PD = int(readIOreg(0x26u)) << 16;\n                                                        "    // l:165
"    }\n                                                                                            "    // l:166
"    else {\n                                                                                       "    // l:167
"        ReferenceLine = ReferenceLine3[y];\n                                                       "    // l:168
"\n                                                                                                 "    // l:169
"        BGX_raw  = readIOreg(0x38u);\n                                                             "    // l:170
"        BGX_raw |= readIOreg(0x3au) << 16;\n                                                       "    // l:171
"        BGY_raw  = readIOreg(0x3cu);\n                                                             "    // l:172
"        BGY_raw |= readIOreg(0x3eu) << 16;\n                                                       "    // l:173
"        PA = int(readIOreg(0x30u)) << 16;\n                                                        "    // l:174
"        PB = int(readIOreg(0x32u)) << 16;\n                                                        "    // l:175
"        PC = int(readIOreg(0x34u)) << 16;\n                                                        "    // l:176
"        PD = int(readIOreg(0x36u)) << 16;\n                                                        "    // l:177
"    }\n                                                                                            "    // l:178
"\n                                                                                                 "    // l:179
"    // convert to signed\n                                                                         "    // l:180
"    int BGX = int(BGX_raw) << 4;\n                                                                 "    // l:181
"    int BGY = int(BGY_raw) << 4;\n                                                                 "    // l:182
"    BGX >>= 4;\n                                                                                   "    // l:183
"    BGY >>= 4;\n                                                                                   "    // l:184
"\n                                                                                                 "    // l:185
"    // was already shifted left\n                                                                  "    // l:186
"    PA >>= 16;\n                                                                                   "    // l:187
"    PB >>= 16;\n                                                                                   "    // l:188
"    PC >>= 16;\n                                                                                   "    // l:189
"    PD >>= 16;\n                                                                                   "    // l:190
"\n                                                                                                 "    // l:191
"    uint CBB, SBB, Size;\n                                                                         "    // l:192
"    bool ColorMode;\n                                                                              "    // l:193
"\n                                                                                                 "    // l:194
"    CBB       = (BGCNT >> 2) & 3u;\n                                                               "    // l:195
"    SBB       = (BGCNT >> 8) & 0x1fu;\n                                                            "    // l:196
"    Size      = AffineBGSizeTable[(BGCNT >> 14) & 3u];\n                                           "    // l:197
"\n                                                                                                 "    // l:198
"    mat2x2 RotationScaling = mat2x2(\n                                                             "    // l:199
"        float(PA), float(PC),  // first column\n                                                   "    // l:200
"        float(PB), float(PD)   // second column\n                                                  "    // l:201
"    );\n                                                                                           "    // l:202
"\n                                                                                                 "    // l:203
"    vec2 pos  = screen_pos - vec2(0, ReferenceLine);\n                                             "    // l:204
"    int x_eff = int(float(BGX) + dot(vec2(PA, PB), pos));\n                                        "    // l:205
"    int y_eff = int(float(BGY) + dot(vec2(PC, PD), pos));\n                                        "    // l:206
"\n                                                                                                 "    // l:207
"    // correct for fixed point math\n                                                              "    // l:208
"    x_eff >>= 8;\n                                                                                 "    // l:209
"    y_eff >>= 8;\n                                                                                 "    // l:210
"\n                                                                                                 "    // l:211
"    if ((x_eff < 0) || (x_eff > int(Size)) || (y_eff < 0) || (y_eff > int(Size))) {\n              "    // l:212
"        if ((BGCNT & 0x2000u) == 0u) {\n                                                           "    // l:213
"            // no display area overflow\n                                                          "    // l:214
"            discard;\n                                                                             "    // l:215
"        }\n                                                                                        "    // l:216
"\n                                                                                                 "    // l:217
"        // wrapping\n                                                                              "    // l:218
"        x_eff &= int(Size) - 1;\n                                                                  "    // l:219
"        y_eff &= int(Size) - 1;\n                                                                  "    // l:220
"    }\n                                                                                            "    // l:221
"\n                                                                                                 "    // l:222
"    // mosaic effect\n                                                                             "    // l:223
"    if ((BGCNT & 0x0040u) != 0u) {\n                                                               "    // l:224
"        uint MOSAIC = readIOreg(0x4cu);\n                                                          "    // l:225
"        x_eff -= x_eff % int((MOSAIC & 0xfu) + 1u);\n                                              "    // l:226
"        y_eff -= y_eff % int(((MOSAIC & 0xf0u) >> 4u) + 1u);\n                                     "    // l:227
"    }\n                                                                                            "    // l:228
"\n                                                                                                 "    // l:229
"    uint TIDAddress = (SBB << 11u);  // base\n                                                     "    // l:230
"    TIDAddress += ((uint(y_eff) >> 3) * (Size >> 3)) | (uint(x_eff) >> 3);  // tile\n              "    // l:231
"    uint TID = readVRAM8(TIDAddress);\n                                                            "    // l:232
"\n                                                                                                 "    // l:233
"    uint PixelAddress = (CBB << 14) | (TID << 6) | ((uint(y_eff) & 7u) << 3) | (uint(x_eff) & 7u);\n"    // l:234
"    uint VRAMEntry = readVRAM8(PixelAddress);\n                                                    "    // l:235
"\n                                                                                                 "    // l:236
"    // transparent\n                                                                               "    // l:237
"    if (VRAMEntry == 0u) {\n                                                                       "    // l:238
"        discard;\n                                                                                 "    // l:239
"    }\n                                                                                            "    // l:240
"\n                                                                                                 "    // l:241
"    return vec4(readPALentry(VRAMEntry).rgb, 1.0);\n                                               "    // l:242
"}\n                                                                                                "    // l:243
"\n                                                                                                 "    // l:244
"vec4 mode0(uint, uint);\n                                                                          "    // l:245
"vec4 mode1(uint, uint, vec2);\n                                                                    "    // l:246
"vec4 mode2(uint, uint, vec2);\n                                                                    "    // l:247
"vec4 mode3(uint, uint);\n                                                                          "    // l:248
"vec4 mode4(uint, uint);\n                                                                          "    // l:249
"\n                                                                                                 "    // l:250
"void main() {\n                                                                                    "    // l:251
"    uint x = uint(screenCoord.x);\n                                                                "    // l:252
"    uint y = uint(screenCoord.y);\n                                                                "    // l:253
"\n                                                                                                 "    // l:254
"    uint window = getWindow(x, y);\n                                                               "    // l:255
"    uint BLDCNT = readIOreg(0x50u);\n                                                              "    // l:256
"\n                                                                                                 "    // l:257
"    if (BG >= 4u) {\n                                                                              "    // l:258
"        CheckBottom(5u, window);\n                                                                 "    // l:259
"\n                                                                                                 "    // l:260
"        // backdrop, highest frag depth\n                                                          "    // l:261
"        gl_FragDepth = 1.0;\n                                                                      "    // l:262
"        FragColor = ColorCorrect(vec4(readPALentry(0u).rgb, 1.0));\n                               "    // l:263
"        FragColor = AlphaCorrect(FragColor, 5u, window);\n                                         "    // l:264
"        return;\n                                                                                  "    // l:265
"    }\n                                                                                            "    // l:266
"\n                                                                                                 "    // l:267
"    // check if we are rendering on the bottom layer, and if we even need to render this fragment\n"    // l:268
"    CheckBottom(BG, window);\n                                                                     "    // l:269
"\n                                                                                                 "    // l:270
"    // disabled by window\n                                                                        "    // l:271
"    if ((window & (1u << BG)) == 0u) {\n                                                           "    // l:272
"        discard;\n                                                                                 "    // l:273
"    }\n                                                                                            "    // l:274
"\n                                                                                                 "    // l:275
"    uint DISPCNT = readIOreg(0u);\n                                                                "    // l:276
"\n                                                                                                 "    // l:277
"    vec4 outColor;\n                                                                               "    // l:278
"    switch(DISPCNT & 7u) {\n                                                                       "    // l:279
"        case 0u:\n                                                                                 "    // l:280
"            outColor = mode0(x, y);\n                                                              "    // l:281
"            break;\n                                                                               "    // l:282
"        case 1u:\n                                                                                 "    // l:283
"            outColor = mode1(x, y, screenCoord);\n                                                 "    // l:284
"            break;\n                                                                               "    // l:285
"        case 2u:\n                                                                                 "    // l:286
"            outColor = mode2(x, y, screenCoord);\n                                                 "    // l:287
"            break;\n                                                                               "    // l:288
"        case 3u:\n                                                                                 "    // l:289
"            outColor = mode3(x, y);\n                                                              "    // l:290
"            break;\n                                                                               "    // l:291
"        case 4u:\n                                                                                 "    // l:292
"            outColor = mode4(x, y);\n                                                              "    // l:293
"            break;\n                                                                               "    // l:294
"        default:\n                                                                                 "    // l:295
"            outColor = vec4(float(x) / float(240u), float(y) / float(160u), 1.0, 1.0);\n           "    // l:296
"            break;\n                                                                               "    // l:297
"    }\n                                                                                            "    // l:298
"\n                                                                                                 "    // l:299
"    FragColor = ColorCorrect(outColor);\n                                                          "    // l:300
"    FragColor = AlphaCorrect(FragColor, BG, window);\n                                             "    // l:301
"}\n                                                                                                "    // l:302
"\n                                                                                                 "    // l:303
;


// FragmentHelperSource (from fragment_helpers.glsl, lines 1 to 165)
const char* FragmentHelperSource = 
"/* GENERAL */\n                                                                                    "    // l:1
"precision mediump float;\n                                                                         "    // l:2
"precision highp int;\n                                                                             "    // l:3
"precision mediump usampler2D;\n                                                                    "    // l:4
"precision mediump isampler2D;\n                                                                    "    // l:5
"\n                                                                                                 "    // l:6
"in vec2 OnScreenPos;\n                                                                             "    // l:7
"\n                                                                                                 "    // l:8
"uniform usampler2D PAL;\n                                                                          "    // l:9
"uniform usampler2D VRAM;\n                                                                         "    // l:10
"uniform usampler2D IO;\n                                                                           "    // l:11
"uniform isampler2D OAM;\n                                                                          "    // l:12
"uniform usampler2D Window;\n                                                                       "    // l:13
"\n                                                                                                 "    // l:14
"uniform bool Bottom;\n                                                                             "    // l:15
"\n                                                                                                 "    // l:16
"uniform uint PALBufferIndex[160u];\n                                                               "    // l:17
"\n                                                                                                 "    // l:18
"uint readIOreg(uint address);\n                                                                    "    // l:19
"\n                                                                                                 "    // l:20
"// algorithm from https://byuu.net/video/color-emulation/\n                                        "    // l:21
"const float lcdGamma = 4.0;\n                                                                      "    // l:22
"const float outGamma = 2.2;\n                                                                      "    // l:23
"const mat3x3 CorrectionMatrix = mat3x3(\n                                                          "    // l:24
"        255.0,  10.0,  50.0,\n                                                                     "    // l:25
"         50.0, 230.0,  10.0,\n                                                                     "    // l:26
"          0.0,  30.0, 220.0\n                                                                      "    // l:27
") / 255.0;\n                                                                                       "    // l:28
"\n                                                                                                 "    // l:29
"vec4 ColorCorrect(vec4 color) {\n                                                                  "    // l:30
"    vec3 lrgb = pow(color.rgb, vec3(lcdGamma));\n                                                  "    // l:31
"    vec3 rgb = pow(CorrectionMatrix * lrgb, vec3(1.0 / outGamma)) * (255.0 / 280.0);\n             "    // l:32
"    return vec4(rgb, color.a);\n                                                                   "    // l:33
"}\n                                                                                                "    // l:34
"\n                                                                                                 "    // l:35
"void CheckBottom(uint layer, uint window) {\n                                                      "    // l:36
"    if (Bottom) {\n                                                                                "    // l:37
"        uint BLDCNT = readIOreg(0x50u);\n                                                          "    // l:38
"        if (((BLDCNT & 0x00c0u) != 0x0040u)) {\n                                                   "    // l:39
"            // not interesting, not normal alpha blending\n                                        "    // l:40
"            discard;\n                                                                             "    // l:41
"        }\n                                                                                        "    // l:42
"\n                                                                                                 "    // l:43
"        if ((window & 0x20u) == 0u) {\n                                                            "    // l:44
"            // blending disabled in window, don't render on bottom frame\n                         "    // l:45
"            discard;\n                                                                             "    // l:46
"        }\n                                                                                        "    // l:47
"    }\n                                                                                            "    // l:48
"}\n                                                                                                "    // l:49
"\n                                                                                                 "    // l:50
"vec4 AlphaCorrect(vec4 color, uint layer, uint window) {\n                                         "    // l:51
"    // BG0-3, 4 for Obj, 5 for BD\n                                                                "    // l:52
"    if ((window & 0x20u) == 0u) {\n                                                                "    // l:53
"        // blending disabled in window\n                                                           "    // l:54
"        return vec4(color.rgb, -1);\n                                                              "    // l:55
"    }\n                                                                                            "    // l:56
"\n                                                                                                 "    // l:57
"    uint BLDCNT = readIOreg(0x50u);\n                                                              "    // l:58
"    uint BLDY = clamp(readIOreg(0x54u) & 0x1fu, 0u, 16u);\n                                        "    // l:59
"\n                                                                                                 "    // l:60
"    switch (BLDCNT & 0x00c0u) {\n                                                                  "    // l:61
"        case 0x0000u:\n                                                                            "    // l:62
"            // blending disabled\n                                                                 "    // l:63
"            return vec4(color.rgb, -1);\n                                                          "    // l:64
"        case 0x0040u:\n                                                                            "    // l:65
"            // normal blending, do this after (most complicated)\n                                 "    // l:66
"            break;\n                                                                               "    // l:67
"        case 0x0080u:\n                                                                            "    // l:68
"        {\n                                                                                        "    // l:69
"            // blend A with white\n                                                                "    // l:70
"            if ((BLDCNT & (1u << layer)) != 0u) {\n                                                "    // l:71
"                // layer is top layer\n                                                            "    // l:72
"                return vec4(mix(color.rgb, vec3(1), float(BLDY) / 16.0), -1.0);\n                  "    // l:73
"            }\n                                                                                    "    // l:74
"            // bottom layer, not blended\n                                                         "    // l:75
"            return vec4(color.rgb, -1);\n                                                          "    // l:76
"        }\n                                                                                        "    // l:77
"        case 0x00c0u:\n                                                                            "    // l:78
"        {\n                                                                                        "    // l:79
"            // blend A with black\n                                                                "    // l:80
"            if ((BLDCNT & (1u << layer)) != 0u) {\n                                                "    // l:81
"                // layer is top layer\n                                                            "    // l:82
"                return vec4(mix(color.rgb, vec3(0), float(BLDY) / 16.0), -1.0);\n                  "    // l:83
"            }\n                                                                                    "    // l:84
"            // bottom layer, not blended\n                                                         "    // l:85
"            return vec4(color.rgb, -1);\n                                                          "    // l:86
"        }\n                                                                                        "    // l:87
"    }\n                                                                                            "    // l:88
"\n                                                                                                 "    // l:89
"    // value was not normal blending / fade\n                                                      "    // l:90
"    uint BLDALPHA = readIOreg(0x52u);\n                                                            "    // l:91
"    uint BLD_EVA = clamp(BLDALPHA & 0x1fu, 0u, 16u);\n                                             "    // l:92
"    uint BLD_EVB = clamp((BLDALPHA >> 8u) & 0x1fu, 0u, 16u);\n                                     "    // l:93
"\n                                                                                                 "    // l:94
"    if ((BLDCNT & (1u << layer)) != 0u) {\n                                                        "    // l:95
"        // top layer\n                                                                             "    // l:96
"        if (!Bottom) {\n                                                                           "    // l:97
"            return vec4(color.rgb, float(BLD_EVA) / 16.0);\n                                       "    // l:98
"        }\n                                                                                        "    // l:99
"        else {\n                                                                                   "    // l:100
"            discard;\n                                                                             "    // l:101
"        }\n                                                                                        "    // l:102
"    }\n                                                                                            "    // l:103
"    // bottom layer\n                                                                              "    // l:104
"    if ((BLDCNT & (0x100u << layer)) != 0u) {\n                                                    "    // l:105
"        // set alpha value to -half of the actual value\n                                          "    // l:106
"        // -1 means: final color\n                                                                 "    // l:107
"        // negative: bottom\n                                                                      "    // l:108
"        // positive: top\n                                                                         "    // l:109
"        return vec4(color.rgb, -0.25 - (float(BLD_EVB) / 32.0));\n                                 "    // l:110
"    }\n                                                                                            "    // l:111
"\n                                                                                                 "    // l:112
"    // neither\n                                                                                   "    // l:113
"    return vec4(color.rgb, -1);\n                                                                  "    // l:114
"}\n                                                                                                "    // l:115
"\n                                                                                                 "    // l:116
"uint readVRAM8(uint address) {\n                                                                   "    // l:117
"    return texelFetch(\n                                                                           "    // l:118
"        VRAM, ivec2(address & 0x7fu, address >> 7u), 0\n                                           "    // l:119
"    ).x;\n                                                                                         "    // l:120
"}\n                                                                                                "    // l:121
"\n                                                                                                 "    // l:122
"uint readVRAM16(uint address) {\n                                                                  "    // l:123
"    address &= ~1u;\n                                                                              "    // l:124
"    uint lsb = readVRAM8(address);\n                                                               "    // l:125
"    return lsb | (readVRAM8(address + 1u) << 8u);\n                                                "    // l:126
"}\n                                                                                                "    // l:127
"\n                                                                                                 "    // l:128
"uint readVRAM32(uint address) {\n                                                                  "    // l:129
"    address &= ~3u;\n                                                                              "    // l:130
"    uint lsh = readVRAM16(address);\n                                                              "    // l:131
"    return lsh | (readVRAM16(address + 2u) << 16u);\n                                              "    // l:132
"}\n                                                                                                "    // l:133
"\n                                                                                                 "    // l:134
"uint readIOreg(uint address) {\n                                                                   "    // l:135
"    return texelFetch(\n                                                                           "    // l:136
"        IO, ivec2(int(address >> 1u), int(OnScreenPos.y)), 0\n                                     "    // l:137
"    ).x;\n                                                                                         "    // l:138
"}\n                                                                                                "    // l:139
"\n                                                                                                 "    // l:140
"ivec4 readOAMentry(uint index) {\n                                                                 "    // l:141
"    return texelFetch(\n                                                                           "    // l:142
"        OAM, ivec2(int(index), 0), 0\n                                                             "    // l:143
"    );\n                                                                                           "    // l:144
"}\n                                                                                                "    // l:145
"\n                                                                                                 "    // l:146
"vec3 decodeBGR555(uint v) {\n                                                                      "    // l:147
"    uint r =  v        & 31u;\n                                                                    "    // l:148
"    uint g = (v >> 5u) & 31u;\n                                                                    "    // l:149
"    uint b = (v >> 10u)& 31u;\n                                                                    "    // l:150
"    return vec3(r, g, b) / 31.0;\n                                                                 "    // l:151
"}\n                                                                                                "    // l:152
"\n                                                                                                 "    // l:153
"vec4 readPALentry(uint index) {\n                                                                  "    // l:154
"    uint v = texelFetch(PAL, ivec2(int(index), int(PALBufferIndex[uint(OnScreenPos.y)])), 0).r;\n  "    // l:155
"    return vec4(decodeBGR555(v), 1.0);\n                                                           "    // l:156
"}\n                                                                                                "    // l:157
"\n                                                                                                 "    // l:158
"uint getWindow(uint x, uint y) {\n                                                                 "    // l:159
"    return texelFetch(\n                                                                           "    // l:160
"        Window, ivec2(x, int(160u) - int(y)), 0\n                                                  "    // l:161
"    ).r;\n                                                                                         "    // l:162
"}\n                                                                                                "    // l:163
"\n                                                                                                 "    // l:164
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


// ObjectFragmentShaderSource (from object_fragment.glsl, lines 2 to 288)
const char* ObjectFragmentShaderSource = 
"precision mediump float;\n                                                                         "    // l:1
"precision highp int;\n                                                                             "    // l:2
"precision mediump usampler2D;\n                                                                    "    // l:3
"precision mediump isampler2D;\n                                                                    "    // l:4
"\n                                                                                                 "    // l:5
"\n                                                                                                 "    // l:6
"#define attr0 x\n                                                                                  "    // l:7
"#define attr1 y\n                                                                                  "    // l:8
"#define attr2 z\n                                                                                  "    // l:9
"#define attr3 w\n                                                                                  "    // l:10
"\n                                                                                                 "    // l:11
"in vec2 InObjPos;\n                                                                                "    // l:12
"\n                                                                                                 "    // l:13
"flat in uvec4 OBJ;\n                                                                               "    // l:14
"flat in uint ObjWidth;\n                                                                           "    // l:15
"flat in uint ObjHeight;\n                                                                          "    // l:16
"\n                                                                                                 "    // l:17
"uniform bool Affine;\n                                                                             "    // l:18
"uniform uint YClipStart;\n                                                                         "    // l:19
"uniform uint YClipEnd;\n                                                                           "    // l:20
"\n                                                                                                 "    // l:21
"#ifdef OBJ_WINDOW\n                                                                                "    // l:22
"    out uvec4 FragColor;\n                                                                         "    // l:23
"#else\n                                                                                            "    // l:24
"    out vec4 FragColor;\n                                                                          "    // l:25
"#endif\n                                                                                           "    // l:26
"\n                                                                                                 "    // l:27
"vec4 ColorCorrect(vec4 color);\n                                                                   "    // l:28
"vec4 AlphaCorrect(vec4 color, uint layer, uint window);\n                                          "    // l:29
"\n                                                                                                 "    // l:30
"uint readVRAM8(uint address);\n                                                                    "    // l:31
"uint readVRAM16(uint address);\n                                                                   "    // l:32
"uint readVRAM32(uint address);\n                                                                   "    // l:33
"\n                                                                                                 "    // l:34
"uint readIOreg(uint address);\n                                                                    "    // l:35
"ivec4 readOAMentry(uint index);\n                                                                  "    // l:36
"vec4 readPALentry(uint index);\n                                                                   "    // l:37
"\n                                                                                                 "    // l:38
"uint getWindow(uint x, uint y);\n                                                                  "    // l:39
"\n                                                                                                 "    // l:40
"vec4 RegularObject(bool OAM2DMapping) {\n                                                          "    // l:41
"    uint TID = OBJ.attr2 & 0x03ffu;\n                                                              "    // l:42
"\n                                                                                                 "    // l:43
"    uint dx = uint(InObjPos.x);\n                                                                  "    // l:44
"    uint dy = uint(InObjPos.y);\n                                                                  "    // l:45
"\n                                                                                                 "    // l:46
"    // mosaic effect\n                                                                             "    // l:47
"    if ((OBJ.attr0 & 0x1000u) != 0u) {\n                                                           "    // l:48
"        uint MOSAIC = readIOreg(0x4cu);\n                                                          "    // l:49
"        dx -= dx % (((MOSAIC & 0xf00u) >> 8u) + 1u);\n                                             "    // l:50
"        dy -= dy % (((MOSAIC & 0xf000u) >> 12u) + 1u);\n                                           "    // l:51
"    }\n                                                                                            "    // l:52
"\n                                                                                                 "    // l:53
"    uint PixelAddress;\n                                                                           "    // l:54
"    if ((OBJ.attr0 & 0x2000u) == 0x0000u) {\n                                                      "    // l:55
"        uint PaletteBank = OBJ.attr2 >> 12u;\n                                                     "    // l:56
"        PixelAddress = TID << 5u;\n                                                                "    // l:57
"\n                                                                                                 "    // l:58
"        // get base address for line of tiles (vertically)\n                                       "    // l:59
"        if (OAM2DMapping) {\n                                                                      "    // l:60
"            PixelAddress += ObjWidth * (dy >> 3u) << 2u;\n                                         "    // l:61
"        }\n                                                                                        "    // l:62
"        else {\n                                                                                   "    // l:63
"            PixelAddress += 32u * 0x20u * (dy >> 3u);\n                                            "    // l:64
"        }\n                                                                                        "    // l:65
"        PixelAddress += (dy & 7u) << 2; // offset within tile for sliver\n                         "    // l:66
"\n                                                                                                 "    // l:67
"        // Sprites VRAM base address is 0x10000\n                                                  "    // l:68
"        PixelAddress = (PixelAddress & 0x7fffu) | 0x10000u;\n                                      "    // l:69
"\n                                                                                                 "    // l:70
"        // horizontal offset:\n                                                                    "    // l:71
"        PixelAddress += (dx >> 3u) << 5u;    // of tile\n                                          "    // l:72
"        PixelAddress += ((dx & 7u) >> 1u);  // in tile\n                                           "    // l:73
"\n                                                                                                 "    // l:74
"        uint VRAMEntry = readVRAM8(PixelAddress);\n                                                "    // l:75
"        if ((dx & 1u) != 0u) {\n                                                                   "    // l:76
"            // upper nibble\n                                                                      "    // l:77
"            VRAMEntry >>= 4u;\n                                                                    "    // l:78
"        }\n                                                                                        "    // l:79
"        else {\n                                                                                   "    // l:80
"            VRAMEntry &= 0x0fu;\n                                                                  "    // l:81
"        }\n                                                                                        "    // l:82
"\n                                                                                                 "    // l:83
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:84
"            return vec4(readPALentry(0x100u + (PaletteBank << 4u) + VRAMEntry).rgb, 1.0);\n        "    // l:85
"        }\n                                                                                        "    // l:86
"        else {\n                                                                                   "    // l:87
"            // transparent\n                                                                       "    // l:88
"            discard;\n                                                                             "    // l:89
"        }\n                                                                                        "    // l:90
"    }\n                                                                                            "    // l:91
"    else {\n                                                                                       "    // l:92
"        // 8bpp\n                                                                                  "    // l:93
"        PixelAddress = TID << 5;\n                                                                 "    // l:94
"\n                                                                                                 "    // l:95
"        if (OAM2DMapping) {\n                                                                      "    // l:96
"            PixelAddress += ObjWidth * (dy & ~7u);\n                                               "    // l:97
"        }\n                                                                                        "    // l:98
"        else {\n                                                                                   "    // l:99
"            PixelAddress += 32u * 0x20u * (dy >> 3u);\n                                            "    // l:100
"        }\n                                                                                        "    // l:101
"        PixelAddress += (dy & 7u) << 3;\n                                                          "    // l:102
"\n                                                                                                 "    // l:103
"        // Sprites VRAM base address is 0x10000\n                                                  "    // l:104
"        PixelAddress = (PixelAddress & 0x7fffu) | 0x10000u;\n                                      "    // l:105
"\n                                                                                                 "    // l:106
"        // horizontal offset:\n                                                                    "    // l:107
"        PixelAddress += (dx >> 3) << 6;\n                                                          "    // l:108
"        PixelAddress += dx & 7u;\n                                                                 "    // l:109
"\n                                                                                                 "    // l:110
"        uint VRAMEntry = readVRAM8(PixelAddress);\n                                                "    // l:111
"\n                                                                                                 "    // l:112
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:113
"            return vec4(readPALentry(0x100u + VRAMEntry).rgb, 1);\n                                "    // l:114
"        }\n                                                                                        "    // l:115
"        else {\n                                                                                   "    // l:116
"            // transparent\n                                                                       "    // l:117
"            discard;\n                                                                             "    // l:118
"        }\n                                                                                        "    // l:119
"    }\n                                                                                            "    // l:120
"}\n                                                                                                "    // l:121
"\n                                                                                                 "    // l:122
"bool InsideBox(vec2 v, vec2 bottomLeft, vec2 topRight) {\n                                         "    // l:123
"    vec2 s = step(bottomLeft, v) - step(topRight, v);\n                                            "    // l:124
"    return (s.x * s.y) != 0.0;\n                                                                   "    // l:125
"}\n                                                                                                "    // l:126
"\n                                                                                                 "    // l:127
"vec4 AffineObject(bool OAM2DMapping) {\n                                                           "    // l:128
"    uint TID = OBJ.attr2 & 0x03ffu;\n                                                              "    // l:129
"\n                                                                                                 "    // l:130
"    uint AffineIndex = (OBJ.attr1 & 0x3e00u) >> 9;\n                                               "    // l:131
"    AffineIndex <<= 2;  // goes in groups of 4\n                                                   "    // l:132
"\n                                                                                                 "    // l:133
"    // scaling parameters\n                                                                        "    // l:134
"    int PA = readOAMentry(AffineIndex).attr3;\n                                                    "    // l:135
"    int PB = readOAMentry(AffineIndex + 1u).attr3;\n                                               "    // l:136
"    int PC = readOAMentry(AffineIndex + 2u).attr3;\n                                               "    // l:137
"    int PD = readOAMentry(AffineIndex + 3u).attr3;\n                                               "    // l:138
"\n                                                                                                 "    // l:139
"    // reference point\n                                                                           "    // l:140
"    vec2 p0 = vec2(\n                                                                              "    // l:141
"        float(ObjWidth  >> 1),\n                                                                   "    // l:142
"        float(ObjHeight >> 1)\n                                                                    "    // l:143
"    );\n                                                                                           "    // l:144
"\n                                                                                                 "    // l:145
"    vec2 p1;\n                                                                                     "    // l:146
"    if ((OBJ.attr0 & 0x0300u) == 0x0300u) {\n                                                      "    // l:147
"        // double rendering\n                                                                      "    // l:148
"        p1 = 2.0 * p0;\n                                                                           "    // l:149
"    }\n                                                                                            "    // l:150
"    else {\n                                                                                       "    // l:151
"        p1 = p0;\n                                                                                 "    // l:152
"    }\n                                                                                            "    // l:153
"\n                                                                                                 "    // l:154
"    mat2x2 rotscale = mat2x2(\n                                                                    "    // l:155
"        float(PA), float(PC),\n                                                                    "    // l:156
"        float(PB), float(PD)\n                                                                     "    // l:157
"    ) / 256.0;  // fixed point stuff\n                                                             "    // l:158
"\n                                                                                                 "    // l:159
"    ivec2 pos = ivec2(rotscale * (InObjPos - p1) + p0);\n                                          "    // l:160
"    if (!InsideBox(vec2(pos), vec2(0.0, 0.0), vec2(ObjWidth, ObjHeight))) {\n                      "    // l:161
"        // out of bounds\n                                                                         "    // l:162
"        discard;\n                                                                                 "    // l:163
"    }\n                                                                                            "    // l:164
"\n                                                                                                 "    // l:165
"    // mosaic effect\n                                                                             "    // l:166
"    if ((OBJ.attr0 & 0x1000u) != 0u) {\n                                                           "    // l:167
"        uint MOSAIC = readIOreg(0x4cu);\n                                                          "    // l:168
"        pos.x -= pos.x % int(((MOSAIC & 0xf00u) >> 8u) + 1u);\n                                    "    // l:169
"        pos.y -= pos.y % int(((MOSAIC & 0xf000u) >> 12u) + 1u);\n                                  "    // l:170
"    }\n                                                                                            "    // l:171
"\n                                                                                                 "    // l:172
"    // get actual pixel\n                                                                          "    // l:173
"    uint PixelAddress = 0x10000u;  // OBJ VRAM starts at 0x10000 in VRAM\n                         "    // l:174
"    PixelAddress += TID << 5;\n                                                                    "    // l:175
"        if (OAM2DMapping) {\n                                                                      "    // l:176
"            PixelAddress += ObjWidth * uint(pos.y & ~7) >> 1u;\n                                   "    // l:177
"        }\n                                                                                        "    // l:178
"        else {\n                                                                                   "    // l:179
"            PixelAddress += 32u * 0x20u * uint(pos.y >> 3);\n                                      "    // l:180
"        }\n                                                                                        "    // l:181
"\n                                                                                                 "    // l:182
"    // the rest is very similar to regular sprites:\n                                              "    // l:183
"    if ((OBJ.attr0 & 0x2000u) == 0x0000u) {\n                                                      "    // l:184
"        uint PaletteBank = OBJ.attr2 >> 12u;\n                                                     "    // l:185
"        PixelAddress += uint(pos.y & 7) << 2u; // offset within tile for sliver\n                  "    // l:186
"\n                                                                                                 "    // l:187
"        // horizontal offset:\n                                                                    "    // l:188
"        PixelAddress += uint(pos.x >> 3) << 5u;    // of tile\n                                    "    // l:189
"        PixelAddress += uint(pos.x & 7) >> 1u;  // in tile\n                                       "    // l:190
"\n                                                                                                 "    // l:191
"        uint VRAMEntry = readVRAM8(PixelAddress);\n                                                "    // l:192
"        if ((pos.x & 1) != 0) {\n                                                                  "    // l:193
"            // upper nibble\n                                                                      "    // l:194
"            VRAMEntry >>= 4u;\n                                                                    "    // l:195
"        }\n                                                                                        "    // l:196
"        else {\n                                                                                   "    // l:197
"            VRAMEntry &= 0x0fu;\n                                                                  "    // l:198
"        }\n                                                                                        "    // l:199
"\n                                                                                                 "    // l:200
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:201
"            return vec4(readPALentry(0x100u + (PaletteBank << 4u) + VRAMEntry).rgb, 1);\n          "    // l:202
"        }\n                                                                                        "    // l:203
"        else {\n                                                                                   "    // l:204
"            // transparent\n                                                                       "    // l:205
"            discard;\n                                                                             "    // l:206
"        }\n                                                                                        "    // l:207
"    }\n                                                                                            "    // l:208
"    else {\n                                                                                       "    // l:209
"        PixelAddress += (uint(pos.y) & 7u) << 3u; // offset within tile for sliver\n               "    // l:210
"\n                                                                                                 "    // l:211
"        // horizontal offset:\n                                                                    "    // l:212
"        PixelAddress += uint(pos.x >> 3) << 6u;  // of tile\n                                      "    // l:213
"        PixelAddress += uint(pos.x & 7);        // in tile\n                                       "    // l:214
"\n                                                                                                 "    // l:215
"        uint VRAMEntry = readVRAM8(PixelAddress);\n                                                "    // l:216
"\n                                                                                                 "    // l:217
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:218
"            return vec4(readPALentry(0x100u + VRAMEntry).rgb, 1);\n                                "    // l:219
"        }\n                                                                                        "    // l:220
"        else {\n                                                                                   "    // l:221
"            // transparent\n                                                                       "    // l:222
"            discard;\n                                                                             "    // l:223
"        }\n                                                                                        "    // l:224
"    }\n                                                                                            "    // l:225
"}\n                                                                                                "    // l:226
"\n                                                                                                 "    // l:227
"void main() {\n                                                                                    "    // l:228
"    if (OnScreenPos.x < 0.0) {\n                                                                   "    // l:229
"        discard;\n                                                                                 "    // l:230
"    }\n                                                                                            "    // l:231
"    if (OnScreenPos.x > float(240u)) {\n                                                           "    // l:232
"        discard;\n                                                                                 "    // l:233
"    }\n                                                                                            "    // l:234
"\n                                                                                                 "    // l:235
"    if (OnScreenPos.y < float(YClipStart)) {\n                                                     "    // l:236
"        discard;\n                                                                                 "    // l:237
"    }\n                                                                                            "    // l:238
"    if (OnScreenPos.y > float(YClipEnd)) {\n                                                       "    // l:239
"        discard;\n                                                                                 "    // l:240
"    }\n                                                                                            "    // l:241
"\n                                                                                                 "    // l:242
"    uint DISPCNT = readIOreg(0x00u);\n                                                             "    // l:243
"#ifndef OBJ_WINDOW\n                                                                               "    // l:244
"    if ((DISPCNT & 0x1000u) == 0u) {\n                                                             "    // l:245
"        // objects disabled in this scanline\n                                                     "    // l:246
"        discard;\n                                                                                 "    // l:247
"    }\n                                                                                            "    // l:248
"    uint window = getWindow(uint(OnScreenPos.x), uint(OnScreenPos.y));\n                           "    // l:249
"    if ((window & 0x10u) == 0u) {\n                                                                "    // l:250
"        // disabled by window\n                                                                    "    // l:251
"        discard;\n                                                                                 "    // l:252
"    }\n                                                                                            "    // l:253
"#else\n                                                                                            "    // l:254
"    if ((DISPCNT & 0x8000u) == 0u) {\n                                                             "    // l:255
"        // object window disabled in this scanline\n                                               "    // l:256
"        discard;\n                                                                                 "    // l:257
"    }\n                                                                                            "    // l:258
"#endif\n                                                                                           "    // l:259
"\n                                                                                                 "    // l:260
"    bool OAM2DMapping = (DISPCNT & (0x0040u)) != 0u;\n                                             "    // l:261
"\n                                                                                                 "    // l:262
"    vec4 Color;\n                                                                                  "    // l:263
"    if (!Affine) {\n                                                                               "    // l:264
"        Color = RegularObject(OAM2DMapping);\n                                                     "    // l:265
"    }\n                                                                                            "    // l:266
"    else{\n                                                                                        "    // l:267
"        Color = AffineObject(OAM2DMapping);\n                                                      "    // l:268
"    }\n                                                                                            "    // l:269
"\n                                                                                                 "    // l:270
"#ifndef OBJ_WINDOW\n                                                                               "    // l:271
"    FragColor = ColorCorrect(Color);\n                                                             "    // l:272
"    if ((OBJ.attr0 & 0x0c00u) == 0x0400u) {\n                                                      "    // l:273
"        FragColor = AlphaCorrect(FragColor, 4u, window);\n                                         "    // l:274
"    }\n                                                                                            "    // l:275
"    else {\n                                                                                       "    // l:276
"        FragColor = vec4(FragColor.rgb, -1);\n                                                     "    // l:277
"    }\n                                                                                            "    // l:278
"#else\n                                                                                            "    // l:279
"    // RegularObject/AffineObject will only return if it is nontransparent\n                       "    // l:280
"    uint WINOBJ = (readIOreg(0x4au) >> 8u) & 0x3fu;\n                                              "    // l:281
"\n                                                                                                 "    // l:282
"    FragColor.r = WINOBJ;\n                                                                        "    // l:283
"#endif\n                                                                                           "    // l:284
"}\n                                                                                                "    // l:285
"\n                                                                                                 "    // l:286
;


// ObjectVertexShaderSource (from object_vertex.glsl, lines 2 to 127)
const char* ObjectVertexShaderSource = 
"precision mediump float;\n                                                                         "    // l:1
"precision highp int;\n                                                                             "    // l:2
"precision mediump usampler2D;\n                                                                    "    // l:3
"precision mediump isampler2D;\n                                                                    "    // l:4
"\n                                                                                                 "    // l:5
"\n                                                                                                 "    // l:6
"#define attr0 x\n                                                                                  "    // l:7
"#define attr1 y\n                                                                                  "    // l:8
"#define attr2 z\n                                                                                  "    // l:9
"#define attr3 w\n                                                                                  "    // l:10
"\n                                                                                                 "    // l:11
"uniform bool Affine;\n                                                                             "    // l:12
"\n                                                                                                 "    // l:13
"layout (location = 0) in uvec4 InOBJ;\n                                                            "    // l:14
"\n                                                                                                 "    // l:15
"out vec2 InObjPos;\n                                                                               "    // l:16
"out vec2 OnScreenPos;\n                                                                            "    // l:17
"flat out uvec4 OBJ;\n                                                                              "    // l:18
"flat out uint ObjWidth;\n                                                                          "    // l:19
"flat out uint ObjHeight;\n                                                                         "    // l:20
"\n                                                                                                 "    // l:21
"struct s_ObjSize {\n                                                                               "    // l:22
"    uint width;\n                                                                                  "    // l:23
"    uint height;\n                                                                                 "    // l:24
"};\n                                                                                               "    // l:25
"\n                                                                                                 "    // l:26
"const s_ObjSize ObjSizeTable[12] = s_ObjSize[](\n                                                  "    // l:27
"    s_ObjSize(8u, 8u),  s_ObjSize(16u, 16u), s_ObjSize(32u, 32u), s_ObjSize(64u, 64u),\n           "    // l:28
"    s_ObjSize(16u, 8u), s_ObjSize(32u, 8u),  s_ObjSize(32u, 16u), s_ObjSize(64u, 32u),\n           "    // l:29
"    s_ObjSize(8u, 16u), s_ObjSize(8u, 32u),  s_ObjSize(16u, 32u), s_ObjSize(32u, 62u)\n            "    // l:30
");\n                                                                                               "    // l:31
"\n                                                                                                 "    // l:32
"struct s_Position {\n                                                                              "    // l:33
"    bool right;\n                                                                                  "    // l:34
"    bool low;\n                                                                                    "    // l:35
"};\n                                                                                               "    // l:36
"\n                                                                                                 "    // l:37
"const s_Position PositionTable[4] = s_Position[](\n                                                "    // l:38
"    s_Position(false, false), s_Position(true, false), s_Position(true, true), s_Position(false, true)\n"    // l:39
");\n                                                                                               "    // l:40
"\n                                                                                                 "    // l:41
"void main() {\n                                                                                    "    // l:42
"    OBJ = InOBJ;\n                                                                                 "    // l:43
"    s_Position Position = PositionTable[gl_VertexID & 3];\n                                        "    // l:44
"\n                                                                                                 "    // l:45
"    uint Shape = OBJ.attr0 >> 14;\n                                                                "    // l:46
"    uint Size  = OBJ.attr1 >> 14;\n                                                                "    // l:47
"\n                                                                                                 "    // l:48
"    s_ObjSize ObjSize = ObjSizeTable[(Shape * 4u) + Size];\n                                       "    // l:49
"    ObjWidth = ObjSize.width;\n                                                                    "    // l:50
"    ObjHeight = ObjSize.height;\n                                                                  "    // l:51
"\n                                                                                                 "    // l:52
"    ivec2 ScreenPos = ivec2(OBJ.attr1 & 0x1ffu, OBJ.attr0 & 0xffu);\n                              "    // l:53
"\n                                                                                                 "    // l:54
"    // correct position for screen wrapping\n                                                      "    // l:55
"    if (ScreenPos.x > int(240u)) {\n                                                               "    // l:56
"        ScreenPos.x -= 0x200;\n                                                                    "    // l:57
"    }\n                                                                                            "    // l:58
"\n                                                                                                 "    // l:59
"    if (ScreenPos.y > int(160u)) {\n                                                               "    // l:60
"        ScreenPos.y -= 0x100;\n                                                                    "    // l:61
"    }\n                                                                                            "    // l:62
"\n                                                                                                 "    // l:63
"    InObjPos = vec2(0.0, 0.0);\n                                                                   "    // l:64
"    if (Position.right) {\n                                                                        "    // l:65
"        InObjPos.x  += float(ObjWidth);\n                                                          "    // l:66
"        ScreenPos.x += int(ObjWidth);\n                                                            "    // l:67
"\n                                                                                                 "    // l:68
"        if (Affine) {\n                                                                            "    // l:69
"            if ((OBJ.attr0 & 0x0300u) == 0x0300u) {\n                                              "    // l:70
"                // double rendering\n                                                              "    // l:71
"                InObjPos.x  += float(ObjWidth);\n                                                  "    // l:72
"                ScreenPos.x += int(ObjWidth);\n                                                    "    // l:73
"            }\n                                                                                    "    // l:74
"        }\n                                                                                        "    // l:75
"    }\n                                                                                            "    // l:76
"\n                                                                                                 "    // l:77
"    if (Position.low) {\n                                                                          "    // l:78
"        InObjPos.y  += float(ObjHeight);\n                                                         "    // l:79
"        ScreenPos.y += int(ObjHeight);\n                                                           "    // l:80
"\n                                                                                                 "    // l:81
"        if (Affine) {\n                                                                            "    // l:82
"            if ((OBJ.attr0 & 0x0300u) == 0x0300u) {\n                                              "    // l:83
"                // double rendering\n                                                              "    // l:84
"                InObjPos.y  += float(ObjHeight);\n                                                 "    // l:85
"                ScreenPos.y += int(ObjHeight);\n                                                   "    // l:86
"            }\n                                                                                    "    // l:87
"        }\n                                                                                        "    // l:88
"    }\n                                                                                            "    // l:89
"\n                                                                                                 "    // l:90
"    // flipping only applies to regular sprites\n                                                  "    // l:91
"    if (!Affine) {\n                                                                               "    // l:92
"        if ((OBJ.attr1 & 0x2000u) != 0u) {\n                                                       "    // l:93
"            // VFlip\n                                                                             "    // l:94
"            InObjPos.y = float(ObjHeight) - InObjPos.y;\n                                          "    // l:95
"        }\n                                                                                        "    // l:96
"\n                                                                                                 "    // l:97
"        if ((OBJ.attr1 & 0x1000u) != 0u) {\n                                                       "    // l:98
"            // HFlip\n                                                                             "    // l:99
"            InObjPos.x = float(ObjWidth) - InObjPos.x;\n                                           "    // l:100
"        }\n                                                                                        "    // l:101
"    }\n                                                                                            "    // l:102
"\n                                                                                                 "    // l:103
"    OnScreenPos = vec2(ScreenPos);\n                                                               "    // l:104
"\n                                                                                                 "    // l:105
"#ifndef OBJ_WINDOW\n                                                                               "    // l:106
"    // depth is the same everywhere in the object anyway\n                                         "    // l:107
"    uint Priority = (OBJ.attr2 & 0x0c00u) >> 10;\n                                                 "    // l:108
"\n                                                                                                 "    // l:109
"    gl_Position = vec4(\n                                                                          "    // l:110
"        -1.0 + 2.0 * OnScreenPos.x / float(240u),\n                                                "    // l:111
"        1.0 - 2.0 * OnScreenPos.y / float(160u),\n                                                 "    // l:112
"        -1.0 + float(Priority) / 2.0,  // /2.0 because openGL clips between -1 and 1 (-1 is in front)\n"    // l:113
"        1.0\n                                                                                      "    // l:114
"    );\n                                                                                           "    // l:115
"#else\n                                                                                            "    // l:116
"    gl_Position = vec4(\n                                                                          "    // l:117
"        -1.0 + 2.0 * OnScreenPos.x / float(240u),\n                                                "    // l:118
"        1.0 - 2.0 * OnScreenPos.y / float(160u),\n                                                 "    // l:119
"        0.5,  // between WIN1 and WINOUT\n                                                         "    // l:120
"        1.0\n                                                                                      "    // l:121
"    );\n                                                                                           "    // l:122
"#endif\n                                                                                           "    // l:123
"}\n                                                                                                "    // l:124
"\n                                                                                                 "    // l:125
;


// VertexShaderSource (from vertex.glsl, lines 1 to 27)
const char* VertexShaderSource = 
"#version 320 es\n                                                                                  "    // l:1
"precision mediump float;\n                                                                         "    // l:2
"precision highp int;\n                                                                             "    // l:3
"precision mediump usampler2D;\n                                                                    "    // l:4
"precision mediump isampler2D;\n                                                                    "    // l:5
"\n                                                                                                 "    // l:6
"layout (location = 0) in vec2 position;\n                                                          "    // l:7
"\n                                                                                                 "    // l:8
"out vec2 screenCoord;\n                                                                            "    // l:9
"out vec2 OnScreenPos;  // needed for fragment_helpers\n                                            "    // l:10
"\n                                                                                                 "    // l:11
"void main() {\n                                                                                    "    // l:12
"    // convert y coordinate from scanline to screen coordinate\n                                   "    // l:13
"    gl_Position = vec4(\n                                                                          "    // l:14
"        position.x,\n                                                                              "    // l:15
"        1.0 - (2.0 * position.y) / float(160u), 0.0, 1.0\n                                         "    // l:16
"    );\n                                                                                           "    // l:17
"\n                                                                                                 "    // l:18
"    screenCoord = vec2(\n                                                                          "    // l:19
"        float(240u) * (1.0 + position.x) / 2.0,\n                                                  "    // l:20
"        position.y\n                                                                               "    // l:21
"    );\n                                                                                           "    // l:22
"\n                                                                                                 "    // l:23
"    OnScreenPos = screenCoord;\n                                                                   "    // l:24
"}\n                                                                                                "    // l:25
"\n                                                                                                 "    // l:26
;


// WindowFragmentShaderSource (from window_fragment.glsl, lines 1 to 85)
const char* WindowFragmentShaderSource = 
"#version 320 es\n                                                                                  "    // l:1
"precision mediump float;\n                                                                         "    // l:2
"precision highp int;\n                                                                             "    // l:3
"precision mediump usampler2D;\n                                                                    "    // l:4
"precision mediump isampler2D;\n                                                                    "    // l:5
"\n                                                                                                 "    // l:6
"in vec2 screenCoord;\n                                                                             "    // l:7
"\n                                                                                                 "    // l:8
"out uvec4 FragColor;\n                                                                             "    // l:9
"\n                                                                                                 "    // l:10
"uint readVRAM8(uint address);\n                                                                    "    // l:11
"uint readVRAM16(uint address);\n                                                                   "    // l:12
"uint readVRAM32(uint address);\n                                                                   "    // l:13
"\n                                                                                                 "    // l:14
"uint readIOreg(uint address);\n                                                                    "    // l:15
"vec4 readPALentry(uint index);\n                                                                   "    // l:16
"\n                                                                                                 "    // l:17
"void main() {\n                                                                                    "    // l:18
"    uint DISPCNT = readIOreg(0x00u);\n                                                             "    // l:19
"\n                                                                                                 "    // l:20
"    if ((DISPCNT & 0xe000u) == 0u) {\n                                                             "    // l:21
"        // windows are disabled, enable all windows\n                                              "    // l:22
"        // we should have caught this before rendering, but eh, I guess we'll check again...\n     "    // l:23
"        FragColor.x = 0x3fu;\n                                                                     "    // l:24
"        gl_FragDepth = 1.0;\n                                                                      "    // l:25
"        return;\n                                                                                  "    // l:26
"    }\n                                                                                            "    // l:27
"\n                                                                                                 "    // l:28
"    uint x = clamp(uint(screenCoord.x), 0u, 240u );\n                                              "    // l:29
"    uint y = clamp(uint(screenCoord.y), 0u, 160u); \n                                              "    // l:30
"\n                                                                                                 "    // l:31
"    // window 0 has higher priority\n                                                              "    // l:32
"    for (uint window = 0u; window < 2u; window++) {\n                                              "    // l:33
"        if ((DISPCNT & (0x2000u << window)) == 0u) {\n                                             "    // l:34
"            // window disabled\n                                                                   "    // l:35
"            continue;\n                                                                            "    // l:36
"        }\n                                                                                        "    // l:37
"\n                                                                                                 "    // l:38
"        uint WINH = readIOreg(0x40u + 2u * window);\n                                              "    // l:39
"        uint WINV = readIOreg(0x44u + 2u * window);\n                                              "    // l:40
"        uint WININ = (readIOreg(0x48u) >> (window * 8u)) & 0x3fu;\n                                "    // l:41
"\n                                                                                                 "    // l:42
"        uint X1 = clamp(WINH >> 8u, 0u, 240u );\n                                                  "    // l:43
"        uint X2 = clamp(WINH & 0xffu, 0u, 240u );\n                                                "    // l:44
"        uint Y1 = clamp(WINV >> 8u, 0u, 160u);\n                                                   "    // l:45
"        uint Y2 = clamp(WINV & 0xffu, 0u, 160u);\n                                                 "    // l:46
"\n                                                                                                 "    // l:47
"        if (Y1 <= Y2) {\n                                                                          "    // l:48
"            // no vert wrap and out of bounds, continue\n                                          "    // l:49
"            if (y < Y1 || y >= Y2) {\n                                                             "    // l:50
"                continue;\n                                                                        "    // l:51
"            }\n                                                                                    "    // l:52
"        }\n                                                                                        "    // l:53
"        else {\n                                                                                   "    // l:54
"            // vert wrap and \"in bounds\":\n                                                      "    // l:55
"            if ((y < Y1) && (y >= Y2)) {\n                                                         "    // l:56
"                continue;\n                                                                        "    // l:57
"            }\n                                                                                    "    // l:58
"        }\n                                                                                        "    // l:59
"\n                                                                                                 "    // l:60
"        if (X1 <= X2) {\n                                                                          "    // l:61
"            // no hor wrap\n                                                                       "    // l:62
"            if (x >= X1 && x < X2) {\n                                                             "    // l:63
"                // pixel in WININ\n                                                                "    // l:64
"                FragColor.x = WININ;\n                                                             "    // l:65
"                gl_FragDepth = 0.0;\n                                                              "    // l:66
"                return;\n                                                                          "    // l:67
"            }\n                                                                                    "    // l:68
"        }\n                                                                                        "    // l:69
"        else {\n                                                                                   "    // l:70
"            // hor wrap\n                                                                          "    // l:71
"            if (x < X2 || x >= X1) {\n                                                             "    // l:72
"                // pixel in WININ\n                                                                "    // l:73
"                FragColor.x = WININ;\n                                                             "    // l:74
"                gl_FragDepth = 0.0;\n                                                              "    // l:75
"                return;\n                                                                          "    // l:76
"            }\n                                                                                    "    // l:77
"        }\n                                                                                        "    // l:78
"    }\n                                                                                            "    // l:79
"\n                                                                                                 "    // l:80
"    FragColor.x = readIOreg(0x4au) & 0x3fu;  // WINOUT\n                                           "    // l:81
"    gl_FragDepth = 1.0;\n                                                                          "    // l:82
"}\n                                                                                                "    // l:83
"\n                                                                                                 "    // l:84
;

#endif  // GC__SHADER_H