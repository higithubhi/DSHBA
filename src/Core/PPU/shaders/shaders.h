#ifndef GC__SHADER_H
#define GC__SHADER_H

// BlitFragmentShaderSource (from blit_fragment.glsl, lines 3 to 32)
const char* BlitFragmentShaderSource = 
"in vec2 texCoord;\n                                                                                "    // l:1
"\n                                                                                                 "    // l:2
"uniform sampler2D TopLayer;\n                                                                      "    // l:3
"uniform sampler2D BottomLayer;\n                                                                   "    // l:4
"\n                                                                                                 "    // l:5
"out vec4 FragColor;\n                                                                              "    // l:6
"\n                                                                                                 "    // l:7
"void main() {\n                                                                                    "    // l:8
"    vec4 top = texture(TopLayer, texCoord);\n                                                      "    // l:9
"    vec4 bottom = texture(BottomLayer, texCoord);\n                                                "    // l:10
"\n                                                                                                 "    // l:11
"    // default: pick top\n                                                                         "    // l:12
"    FragColor = vec4(\n                                                                            "    // l:13
"        top.rgb, 1\n                                                                               "    // l:14
"    );\n                                                                                           "    // l:15
"    if ((bottom.a != 0.0) && (bottom.a <= 0.5)) {\n                                                "    // l:16
"        // there was a bottom layer in the bottom framebuffer\n                                    "    // l:17
"        if (top.a >= 0.5) {\n                                                                      "    // l:18
"            float top_a_decoded = (top.a - 0.5) * 2.0;\n                                           "    // l:19
"            float bottom_a_decoded = (bottom.a - 0.5) * 2.0;\n                                     "    // l:20
"\n                                                                                                 "    // l:21
"            FragColor = vec4(\n                                                                    "    // l:22
"                // correct for how we store bottom alpha\n                                         "    // l:23
"                top.rgb * top_a_decoded - 2.0 * bottom.rgb * (bottom_a_decoded + 0.25), 1\n        "    // l:24
"            );\n                                                                                   "    // l:25
"        }\n                                                                                        "    // l:26
"    }\n                                                                                            "    // l:27
"}\n                                                                                                "    // l:28
"\n                                                                                                 "    // l:29
;


// BlitVertexShaderSource (from blit_vertex.glsl, lines 3 to 19)
const char* BlitVertexShaderSource = 
"layout (location = 0) in vec2 position;\n                                                          "    // l:1
"layout (location = 1) in vec2 inTexCoord;\n                                                        "    // l:2
"\n                                                                                                 "    // l:3
"out vec2 texCoord;  // needed for fragment_helpers\n                                               "    // l:4
"\n                                                                                                 "    // l:5
"void main() {\n                                                                                    "    // l:6
"    texCoord = inTexCoord;\n                                                                       "    // l:7
"\n                                                                                                 "    // l:8
"    gl_Position = vec4(\n                                                                          "    // l:9
"        position.x,\n                                                                              "    // l:10
"        position.y,\n                                                                              "    // l:11
"        0.0,\n                                                                                     "    // l:12
"        1.0\n                                                                                      "    // l:13
"    );\n                                                                                           "    // l:14
"}\n                                                                                                "    // l:15
"\n                                                                                                 "    // l:16
;


// FragmentShaderSource (from fragment.glsl, lines 3 to 294)
const char* FragmentShaderSource = 
"in vec2 screenCoord;\n                                                                             "    // l:1
"\n                                                                                                 "    // l:2
"out vec4 FragColor;\n                                                                              "    // l:3
"uniform uint ReferenceLine2[160u];\n                                                               "    // l:4
"uniform uint ReferenceLine3[160u];\n                                                               "    // l:5
"\n                                                                                                 "    // l:6
"// BG 0 - 3 or 4 for backdrop\n                                                                    "    // l:7
"// BG defined in helpers\n                                                                         "    // l:8
"\n                                                                                                 "    // l:9
"// Prototypes defined in helpers\n                                                                 "    // l:10
"\n                                                                                                 "    // l:11
"uint getWindow(uint x, uint y);\n                                                                  "    // l:12
"\n                                                                                                 "    // l:13
"float getDepth(uint BGCNT) {\n                                                                     "    // l:14
"    return ((2.0 * float(BGCNT & 3u)) / 8.0) + (float(1u + BG) / 32.0);\n                          "    // l:15
"}\n                                                                                                "    // l:16
"\n                                                                                                 "    // l:17
"uint VRAMIndex(uint Tx, uint Ty, uint Size) {\n                                                    "    // l:18
"    uint temp = ((Ty & 0x1fu) << 6);\n                                                             "    // l:19
"    temp |= temp | ((Tx & 0x1fu) << 1);\n                                                          "    // l:20
"    switch (Size) {\n                                                                              "    // l:21
"        case 0u:  // 32x32\n                                                                       "    // l:22
"            break;\n                                                                               "    // l:23
"        case 1u:  // 64x32\n                                                                       "    // l:24
"            if ((Tx & 0x20u) != 0u) {\n                                                            "    // l:25
"                temp |= 0x800u;\n                                                                  "    // l:26
"            }\n                                                                                    "    // l:27
"            break;\n                                                                               "    // l:28
"        case 2u:  // 32x64\n                                                                       "    // l:29
"            if ((Ty & 0x20u) != 0u) {\n                                                            "    // l:30
"                temp |= 0x800u;\n                                                                  "    // l:31
"            }\n                                                                                    "    // l:32
"            break;\n                                                                               "    // l:33
"        case 3u:  // 64x64\n                                                                       "    // l:34
"            if ((Ty & 0x20u) != 0u) {\n                                                            "    // l:35
"                temp |= 0x1000u;\n                                                                 "    // l:36
"            }\n                                                                                    "    // l:37
"            if ((Tx & 0x20u) != 0u) {\n                                                            "    // l:38
"                temp |= 0x800u;\n                                                                  "    // l:39
"            }\n                                                                                    "    // l:40
"            break;\n                                                                               "    // l:41
"        default:\n                                                                                 "    // l:42
"            // invalid, should not happen\n                                                        "    // l:43
"            return 0u;\n                                                                           "    // l:44
"    }\n                                                                                            "    // l:45
"    return temp;\n                                                                                 "    // l:46
"}\n                                                                                                "    // l:47
"\n                                                                                                 "    // l:48
"vec4 regularScreenEntryPixel(uint dx, uint dy, uint ScreenEntry, uint CBB, bool ColorMode) {\n     "    // l:49
"    uint PaletteBank = ScreenEntry >> 12;  // 16 bits, we need top 4\n                             "    // l:50
"    if ((ScreenEntry & 0x0800u) != 0u) {\n                                                         "    // l:51
"        // VFlip\n                                                                                 "    // l:52
"        dy = 7u - dy;\n                                                                            "    // l:53
"    }\n                                                                                            "    // l:54
"\n                                                                                                 "    // l:55
"    if ((ScreenEntry & 0x0400u) != 0u) {\n                                                         "    // l:56
"        // HFlip\n                                                                                 "    // l:57
"        dx = 7u - dx;\n                                                                            "    // l:58
"    }\n                                                                                            "    // l:59
"\n                                                                                                 "    // l:60
"    uint TID     = ScreenEntry & 0x3ffu;\n                                                         "    // l:61
"    uint Address = CBB << 14;\n                                                                    "    // l:62
"\n                                                                                                 "    // l:63
"    if (!ColorMode) {\n                                                                            "    // l:64
"        // 4bpp\n                                                                                  "    // l:65
"        Address += TID << 5; // beginning of tile\n                                                "    // l:66
"        Address += dy << 2;  // beginning of sliver\n                                              "    // l:67
"\n                                                                                                 "    // l:68
"        Address += dx >> 1;  // offset into sliver\n                                               "    // l:69
"        uint VRAMEntry = readVRAM8(Address);\n                                                     "    // l:70
"        if ((dx & 1u) != 0u) {\n                                                                   "    // l:71
"            VRAMEntry >>= 4;     // odd x, upper nibble\n                                          "    // l:72
"        }\n                                                                                        "    // l:73
"        else {\n                                                                                   "    // l:74
"            VRAMEntry &= 0xfu;  // even x, lower nibble\n                                          "    // l:75
"        }\n                                                                                        "    // l:76
"\n                                                                                                 "    // l:77
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:78
"            return vec4(readPALentry((PaletteBank << 4) + VRAMEntry).rgb, 1);\n                    "    // l:79
"        }\n                                                                                        "    // l:80
"    }\n                                                                                            "    // l:81
"    else {\n                                                                                       "    // l:82
"        // 8bpp\n                                                                                  "    // l:83
"        Address += TID << 6; // beginning of tile\n                                                "    // l:84
"        Address += dy << 3;  // beginning of sliver\n                                              "    // l:85
"\n                                                                                                 "    // l:86
"        Address += dx;       // offset into sliver\n                                               "    // l:87
"        uint VRAMEntry = readVRAM8(Address);\n                                                     "    // l:88
"\n                                                                                                 "    // l:89
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:90
"            return vec4(readPALentry(VRAMEntry).rgb, 1);\n                                         "    // l:91
"        }\n                                                                                        "    // l:92
"    }\n                                                                                            "    // l:93
"\n                                                                                                 "    // l:94
"    // transparent\n                                                                               "    // l:95
"    discard;\n                                                                                     "    // l:96
"}\n                                                                                                "    // l:97
"\n                                                                                                 "    // l:98
"vec4 regularBGPixel(uint BGCNT, uint x, uint y) {\n                                                "    // l:99
"    uint HOFS, VOFS;\n                                                                             "    // l:100
"    uint CBB, SBB, Size;\n                                                                         "    // l:101
"    bool ColorMode;\n                                                                              "    // l:102
"\n                                                                                                 "    // l:103
"    HOFS  = readIOreg(0x10u + (BG << 2)) & 0x1ffu;\n                                               "    // l:104
"    VOFS  = readIOreg(0x12u + (BG << 2)) & 0x1ffu;\n                                               "    // l:105
"\n                                                                                                 "    // l:106
"    CBB       = (BGCNT >> 2) & 3u;\n                                                               "    // l:107
"    ColorMode = (BGCNT & 0x0080u) == 0x0080u;  // 0: 4bpp, 1: 8bpp\n                               "    // l:108
"    SBB       = (BGCNT >> 8) & 0x1fu;\n                                                            "    // l:109
"    Size      = (BGCNT >> 14) & 3u;\n                                                              "    // l:110
"\n                                                                                                 "    // l:111
"    uint x_eff = (x + HOFS) & 0xffffu;\n                                                           "    // l:112
"    uint y_eff = (y + VOFS) & 0xffffu;\n                                                           "    // l:113
"\n                                                                                                 "    // l:114
"    // mosaic effect\n                                                                             "    // l:115
"    if ((BGCNT & 0x0040u) != 0u) {\n                                                               "    // l:116
"        uint MOSAIC = readIOreg(0x4cu);\n                                                          "    // l:117
"        x_eff -= x_eff % ((MOSAIC & 0xfu) + 1u);\n                                                 "    // l:118
"        y_eff -= y_eff % (((MOSAIC & 0xf0u) >> 4) + 1u);\n                                         "    // l:119
"    }\n                                                                                            "    // l:120
"\n                                                                                                 "    // l:121
"    uint ScreenEntryIndex = VRAMIndex(x_eff >> 3u, y_eff >> 3u, Size);\n                           "    // l:122
"    ScreenEntryIndex += (SBB << 11u);\n                                                            "    // l:123
"    uint ScreenEntry = readVRAM16(ScreenEntryIndex);  // always halfword aligned\n                 "    // l:124
"\n                                                                                                 "    // l:125
"    return regularScreenEntryPixel(x_eff & 7u, y_eff & 7u, ScreenEntry, CBB, ColorMode);\n         "    // l:126
"}\n                                                                                                "    // l:127
"\n                                                                                                 "    // l:128
"const uint AffineBGSizeTable[4] = uint[](\n                                                        "    // l:129
"    128u, 256u, 512u, 1024u\n                                                                      "    // l:130
");\n                                                                                               "    // l:131
"\n                                                                                                 "    // l:132
"vec4 affineBGPixel(uint BGCNT, vec2 screen_pos) {\n                                                "    // l:133
"    uint x = uint(screen_pos.x);\n                                                                 "    // l:134
"    uint y = uint(screen_pos.y);\n                                                                 "    // l:135
"\n                                                                                                 "    // l:136
"    uint ReferenceLine;\n                                                                          "    // l:137
"    uint BGX_raw, BGY_raw;\n                                                                       "    // l:138
"    int PA, PB, PC, PD;\n                                                                          "    // l:139
"    if (BG == 2u) {\n                                                                              "    // l:140
"        ReferenceLine = ReferenceLine2[y];\n                                                       "    // l:141
"\n                                                                                                 "    // l:142
"        BGX_raw  = readIOreg(0x28u);\n                                                             "    // l:143
"        BGX_raw |= readIOreg(0x2au) << 16;\n                                                       "    // l:144
"        BGY_raw  = readIOreg(0x2cu);\n                                                             "    // l:145
"        BGY_raw |= readIOreg(0x2eu) << 16;\n                                                       "    // l:146
"        PA = int(readIOreg(0x20u)) << 16;\n                                                        "    // l:147
"        PB = int(readIOreg(0x22u)) << 16;\n                                                        "    // l:148
"        PC = int(readIOreg(0x24u)) << 16;\n                                                        "    // l:149
"        PD = int(readIOreg(0x26u)) << 16;\n                                                        "    // l:150
"    }\n                                                                                            "    // l:151
"    else {\n                                                                                       "    // l:152
"        ReferenceLine = ReferenceLine3[y];\n                                                       "    // l:153
"\n                                                                                                 "    // l:154
"        BGX_raw  = readIOreg(0x38u);\n                                                             "    // l:155
"        BGX_raw |= readIOreg(0x3au) << 16;\n                                                       "    // l:156
"        BGY_raw  = readIOreg(0x3cu);\n                                                             "    // l:157
"        BGY_raw |= readIOreg(0x3eu) << 16;\n                                                       "    // l:158
"        PA = int(readIOreg(0x30u)) << 16;\n                                                        "    // l:159
"        PB = int(readIOreg(0x32u)) << 16;\n                                                        "    // l:160
"        PC = int(readIOreg(0x34u)) << 16;\n                                                        "    // l:161
"        PD = int(readIOreg(0x36u)) << 16;\n                                                        "    // l:162
"    }\n                                                                                            "    // l:163
"\n                                                                                                 "    // l:164
"    // convert to signed\n                                                                         "    // l:165
"    int BGX = int(BGX_raw) << 4;\n                                                                 "    // l:166
"    int BGY = int(BGY_raw) << 4;\n                                                                 "    // l:167
"    BGX >>= 4;\n                                                                                   "    // l:168
"    BGY >>= 4;\n                                                                                   "    // l:169
"\n                                                                                                 "    // l:170
"    // was already shifted left\n                                                                  "    // l:171
"    PA >>= 16;\n                                                                                   "    // l:172
"    PB >>= 16;\n                                                                                   "    // l:173
"    PC >>= 16;\n                                                                                   "    // l:174
"    PD >>= 16;\n                                                                                   "    // l:175
"\n                                                                                                 "    // l:176
"    uint CBB, SBB, Size;\n                                                                         "    // l:177
"    bool ColorMode;\n                                                                              "    // l:178
"\n                                                                                                 "    // l:179
"    CBB       = (BGCNT >> 2) & 3u;\n                                                               "    // l:180
"    SBB       = (BGCNT >> 8) & 0x1fu;\n                                                            "    // l:181
"    Size      = AffineBGSizeTable[(BGCNT >> 14) & 3u];\n                                           "    // l:182
"\n                                                                                                 "    // l:183
"    mat2x2 RotationScaling = mat2x2(\n                                                             "    // l:184
"        float(PA), float(PC),  // first column\n                                                   "    // l:185
"        float(PB), float(PD)   // second column\n                                                  "    // l:186
"    );\n                                                                                           "    // l:187
"\n                                                                                                 "    // l:188
"    vec2 pos  = screen_pos - vec2(0, ReferenceLine);\n                                             "    // l:189
"    int x_eff = int(BGX + int(dot(vec2(PA, PB), pos)));\n                                          "    // l:190
"    int y_eff = int(BGY + int(dot(vec2(PC, PD), pos)));\n                                          "    // l:191
"\n                                                                                                 "    // l:192
"    // correct for fixed point math\n                                                              "    // l:193
"    x_eff >>= 8;\n                                                                                 "    // l:194
"    y_eff >>= 8;\n                                                                                 "    // l:195
"\n                                                                                                 "    // l:196
"    if ((x_eff < 0) || (x_eff > int(Size)) || (y_eff < 0) || (y_eff > int(Size))) {\n              "    // l:197
"        if ((BGCNT & 0x2000u) == 0u) {\n                                                           "    // l:198
"            // no display area overflow\n                                                          "    // l:199
"            discard;\n                                                                             "    // l:200
"        }\n                                                                                        "    // l:201
"\n                                                                                                 "    // l:202
"        // wrapping\n                                                                              "    // l:203
"        x_eff &= int(Size) - 1;\n                                                                  "    // l:204
"        y_eff &= int(Size) - 1;\n                                                                  "    // l:205
"    }\n                                                                                            "    // l:206
"\n                                                                                                 "    // l:207
"    // mosaic effect\n                                                                             "    // l:208
"    if ((BGCNT & 0x0040u) != 0u) {\n                                                               "    // l:209
"        uint MOSAIC = readIOreg(0x4cu);\n                                                          "    // l:210
"        x_eff -= x_eff % int((MOSAIC & 0xfu) + 1u);\n                                              "    // l:211
"        y_eff -= y_eff % int(((MOSAIC & 0xf0u) >> 4) + 1u);\n                                      "    // l:212
"    }\n                                                                                            "    // l:213
"\n                                                                                                 "    // l:214
"    uint TIDAddress = (SBB << 11u);  // base\n                                                     "    // l:215
"    TIDAddress += ((uint(y_eff) >> 3) * (Size >> 3)) | (uint(x_eff) >> 3);  // tile\n              "    // l:216
"    uint TID = readVRAM8(TIDAddress);\n                                                            "    // l:217
"\n                                                                                                 "    // l:218
"    uint PixelAddress = (CBB << 14) | (TID << 6) | ((uint(y_eff) & 7u) << 3) | (uint(x_eff) & 7u);\n"    // l:219
"    uint VRAMEntry = readVRAM8(PixelAddress);\n                                                    "    // l:220
"\n                                                                                                 "    // l:221
"    // transparent\n                                                                               "    // l:222
"    if (VRAMEntry == 0u) {\n                                                                       "    // l:223
"        discard;\n                                                                                 "    // l:224
"    }\n                                                                                            "    // l:225
"\n                                                                                                 "    // l:226
"    return vec4(readPALentry(VRAMEntry).rgb, 1);\n                                                 "    // l:227
"}\n                                                                                                "    // l:228
"\n                                                                                                 "    // l:229
"vec4 mode0(uint, uint);\n                                                                          "    // l:230
"vec4 mode1(uint, uint, vec2);\n                                                                    "    // l:231
"vec4 mode2(uint, uint, vec2);\n                                                                    "    // l:232
"vec4 mode3(uint, uint);\n                                                                          "    // l:233
"vec4 mode4(uint, uint);\n                                                                          "    // l:234
"\n                                                                                                 "    // l:235
"void main() {\n                                                                                    "    // l:236
"    uint x = uint(screenCoord.x);\n                                                                "    // l:237
"    uint y = uint(screenCoord.y);\n                                                                "    // l:238
"\n                                                                                                 "    // l:239
"    uint window = getWindow(x, y);\n                                                               "    // l:240
"    uint BLDCNT = readIOreg(0x50u);\n                                                              "    // l:241
"\n                                                                                                 "    // l:242
"    if (BG >= 4u) {\n                                                                              "    // l:243
"        CheckBottom(5u, window);\n                                                                 "    // l:244
"\n                                                                                                 "    // l:245
"        // backdrop, highest frag depth\n                                                          "    // l:246
"        gl_FragDepth = 1.0;\n                                                                      "    // l:247
"        FragColor = ColorCorrect(vec4(readPALentry(0u).rgb, 1.0));\n                               "    // l:248
"        FragColor = AlphaCorrect(FragColor, 5u, window);\n                                         "    // l:249
"        return;\n                                                                                  "    // l:250
"    }\n                                                                                            "    // l:251
"\n                                                                                                 "    // l:252
"    // check if we are rendering on the bottom layer, and if we even need to render this fragment\n"    // l:253
"    CheckBottom(BG, window);\n                                                                     "    // l:254
"\n                                                                                                 "    // l:255
"    // disabled by window\n                                                                        "    // l:256
"    if ((window & (1u << BG)) == 0u) {\n                                                           "    // l:257
"        discard;\n                                                                                 "    // l:258
"    }\n                                                                                            "    // l:259
"\n                                                                                                 "    // l:260
"   //uint BGCNT = readIOreg(0x08u + (BG * 2u));\n                                                  "    // l:261
"   //gl_FragDepth = getDepth(BGCNT);\n                                                             "    // l:262
"\n                                                                                                 "    // l:263
"    uint DISPCNT = readIOreg(0u);\n                                                                "    // l:264
"\n                                                                                                 "    // l:265
"    vec4 outColor;\n                                                                               "    // l:266
"    switch(DISPCNT & 7u) {\n                                                                       "    // l:267
"        case 0u:\n                                                                                 "    // l:268
"            outColor = mode0(x, y);\n                                                              "    // l:269
"            break;\n                                                                               "    // l:270
"        case 1u:\n                                                                                 "    // l:271
"            outColor = mode1(x, y, screenCoord);\n                                                 "    // l:272
"            break;\n                                                                               "    // l:273
"        case 2u:\n                                                                                 "    // l:274
"            outColor = mode2(x, y, screenCoord);\n                                                 "    // l:275
"            break;\n                                                                               "    // l:276
"        case 3u:\n                                                                                 "    // l:277
"            outColor = mode3(x, y);\n                                                              "    // l:278
"            break;\n                                                                               "    // l:279
"        case 4u:\n                                                                                 "    // l:280
"            outColor = mode4(x, y);\n                                                              "    // l:281
"            break;\n                                                                               "    // l:282
"        default:\n                                                                                 "    // l:283
"            outColor = vec4(float(x) / float(240u), float(y) / float(160u), 1.0, 1.0);\n           "    // l:284
"            break;\n                                                                               "    // l:285
"    }\n                                                                                            "    // l:286
"\n                                                                                                 "    // l:287
"    FragColor = ColorCorrect(outColor);\n                                                          "    // l:288
"    FragColor = AlphaCorrect(FragColor, BG, window);\n                                             "    // l:289
"}\n                                                                                                "    // l:290
"\n                                                                                                 "    // l:291
;


// FragmentHelperSource (from fragment_helpers.glsl, lines 3 to 174)
const char* FragmentHelperSource = 
"/* GENERAL */\n                                                                                    "    // l:1
"in vec2 OnScreenPos;\n                                                                             "    // l:2
"\n                                                                                                 "    // l:3
"uniform usampler2D PAL;\n                                                                          "    // l:4
"uniform usampler2D VRAM;\n                                                                         "    // l:5
"uniform usampler2D IO;\n                                                                           "    // l:6
"uniform isampler2D OAM;\n                                                                          "    // l:7
"uniform usampler2D Window;\n                                                                       "    // l:8
"\n                                                                                                 "    // l:9
"uniform bool Bottom;\n                                                                             "    // l:10
"\n                                                                                                 "    // l:11
"uniform int PALBufferIndex[160u];\n                                                                "    // l:12
"\n                                                                                                 "    // l:13
"// Common Uniforms\n                                                                               "    // l:14
"uniform uint BG;\n                                                                                 "    // l:15
"\n                                                                                                 "    // l:16
"// Forward Declarations for functions implemented in fragment.glsl\n                               "    // l:17
"vec4 regularBGPixel(uint BGCNT, uint x, uint y);\n                                                 "    // l:18
"vec4 affineBGPixel(uint BGCNT, vec2 screen_pos);\n                                                 "    // l:19
"float getDepth(uint BGCNT);\n                                                                      "    // l:20
"\n                                                                                                 "    // l:21
"uint readIOreg(uint address);\n                                                                    "    // l:22
"\n                                                                                                 "    // l:23
"// algorithm from https://byuu.net/video/color-emulation/\n                                        "    // l:24
"const float lcdGamma = 4.0;\n                                                                      "    // l:25
"const float outGamma = 2.2;\n                                                                      "    // l:26
"const mat3x3 CorrectionMatrix = mat3x3(\n                                                          "    // l:27
"        255.0,  10.0,  50.0,\n                                                                     "    // l:28
"         50.0, 230.0,  10.0,\n                                                                     "    // l:29
"          0.0,  30.0, 220.0\n                                                                      "    // l:30
") / 255.0;\n                                                                                       "    // l:31
"\n                                                                                                 "    // l:32
"vec4 ColorCorrect(vec4 color) {\n                                                                  "    // l:33
"    vec3 lrgb = pow(color.rgb, vec3(lcdGamma));\n                                                  "    // l:34
"    vec3 rgb = pow(CorrectionMatrix * lrgb, vec3(1.0 / outGamma)) * (255.0 / 280.0);\n             "    // l:35
"    return vec4(rgb, color.a);\n                                                                   "    // l:36
"}\n                                                                                                "    // l:37
"\n                                                                                                 "    // l:38
"void CheckBottom(uint layer, uint window) {\n                                                      "    // l:39
"    if (Bottom) {\n                                                                                "    // l:40
"        uint BLDCNT = readIOreg(0x50u);\n                                                          "    // l:41
"        if (((BLDCNT & 0x00c0u) != 0x0040u)) {\n                                                   "    // l:42
"            // not interesting, not normal alpha blending\n                                        "    // l:43
"            discard;\n                                                                             "    // l:44
"        }\n                                                                                        "    // l:45
"\n                                                                                                 "    // l:46
"        if ((window & 0x20u) == 0u) {\n                                                            "    // l:47
"            // blending disabled in window, don't render on bottom layer\n                         "    // l:48
"            discard;\n                                                                             "    // l:49
"        }\n                                                                                        "    // l:50
"    }\n                                                                                            "    // l:51
"}\n                                                                                                "    // l:52
"\n                                                                                                 "    // l:53
"vec4 AlphaCorrect(vec4 color, uint layer, uint window) {\n                                         "    // l:54
"    // BG0-3, 4 for Obj, 5 for BD\n                                                                "    // l:55
"    if ((window & 0x20u) == 0u) {\n                                                                "    // l:56
"        // blending disabled in window\n                                                           "    // l:57
"        return vec4(color.rgb, 0.0);\n                                                             "    // l:58
"    }\n                                                                                            "    // l:59
"\n                                                                                                 "    // l:60
"    uint BLDCNT = readIOreg(0x50u);\n                                                              "    // l:61
"    uint BLDY = clamp(readIOreg(0x54u) & 0x1fu, 0u, 16u);\n                                        "    // l:62
"\n                                                                                                 "    // l:63
"    switch (BLDCNT & 0x00c0u) {\n                                                                  "    // l:64
"        case 0x0000u:\n                                                                            "    // l:65
"            // blending disabled\n                                                                 "    // l:66
"            return vec4(color.rgb, 0.0);\n                                                         "    // l:67
"        case 0x0040u:\n                                                                            "    // l:68
"            // normal blending, do this after (most complicated)\n                                 "    // l:69
"            break;\n                                                                               "    // l:70
"        case 0x0080u:\n                                                                            "    // l:71
"        {\n                                                                                        "    // l:72
"            // blend A with white\n                                                                "    // l:73
"            if ((BLDCNT & (1u << layer)) != 0u) {\n                                                "    // l:74
"                // layer is top layer\n                                                            "    // l:75
"                return vec4(mix(color.rgb, vec3(1), float(BLDY) / 16.0), 0.0);\n                   "    // l:76
"            }\n                                                                                    "    // l:77
"            // bottom layer, not blended\n                                                         "    // l:78
"            return vec4(color.rgb, 0.0);\n                                                         "    // l:79
"        }\n                                                                                        "    // l:80
"        case 0x00c0u:\n                                                                            "    // l:81
"        {\n                                                                                        "    // l:82
"            // blend A with black\n                                                                "    // l:83
"            if ((BLDCNT & (1u << layer)) != 0u) {\n                                                "    // l:84
"                // layer is top layer\n                                                            "    // l:85
"                return vec4(mix(color.rgb, vec3(0), float(BLDY) / 16.0), 0.0);\n                   "    // l:86
"            }\n                                                                                    "    // l:87
"            // bottom layer, not blended\n                                                         "    // l:88
"            return vec4(color.rgb, 0.0);\n                                                         "    // l:89
"        }\n                                                                                        "    // l:90
"    }\n                                                                                            "    // l:91
"\n                                                                                                 "    // l:92
"    // value was not normal blending / fade\n                                                      "    // l:93
"    uint BLDALPHA = readIOreg(0x52u);\n                                                            "    // l:94
"    uint BLD_EVA = clamp(BLDALPHA & 0x1fu, 0u, 16u);\n                                             "    // l:95
"    uint BLD_EVB = clamp((BLDALPHA >> 8u) & 0x1fu, 0u, 16u);\n                                     "    // l:96
"\n                                                                                                 "    // l:97
"    if ((BLDCNT & (1u << layer)) != 0u) {\n                                                        "    // l:98
"        // top layer\n                                                                             "    // l:99
"        if (!Bottom) {\n                                                                           "    // l:100
"            float val = float(BLD_EVA) / 16.0;\n                                                   "    // l:101
"            return vec4(color.rgb, val * 0.5 + 0.5);\n                                             "    // l:102
"        }\n                                                                                        "    // l:103
"        else {\n                                                                                   "    // l:104
"            discard;\n                                                                             "    // l:105
"        }\n                                                                                        "    // l:106
"    }\n                                                                                            "    // l:107
"    // bottom layer\n                                                                              "    // l:108
"    if ((BLDCNT & (0x100u << layer)) != 0u) {\n                                                    "    // l:109
"        // set alpha value to -half of the actual value\n                                          "    // l:110
"        // -1 means: final color\n                                                                 "    // l:111
"        // negative: bottom\n                                                                      "    // l:112
"        // positive: top\n                                                                         "    // l:113
"        float val = -0.25 - (float(BLD_EVB) / 32.0);\n                                             "    // l:114
"        return vec4(color.rgb, val * 0.5 + 0.5);\n                                                 "    // l:115
"    }\n                                                                                            "    // l:116
"\n                                                                                                 "    // l:117
"    // neither\n                                                                                   "    // l:118
"    return vec4(color.rgb, 0.0);\n                                                                 "    // l:119
"}\n                                                                                                "    // l:120
"\n                                                                                                 "    // l:121
"uint readVRAM8(uint address) {\n                                                                   "    // l:122
"    return texelFetch(\n                                                                           "    // l:123
"        VRAM, ivec2(address & 0x7fu, address >> 7u), 0\n                                           "    // l:124
"    ).x;\n                                                                                         "    // l:125
"}\n                                                                                                "    // l:126
"\n                                                                                                 "    // l:127
"uint readVRAM16(uint address) {\n                                                                  "    // l:128
"    address &= ~1u;\n                                                                              "    // l:129
"    uint lsb = readVRAM8(address);\n                                                               "    // l:130
"    return lsb | (readVRAM8(address + 1u) << 8u);\n                                                "    // l:131
"}\n                                                                                                "    // l:132
"\n                                                                                                 "    // l:133
"uint readVRAM32(uint address) {\n                                                                  "    // l:134
"    address &= ~3u;\n                                                                              "    // l:135
"    uint lsh = readVRAM16(address);\n                                                              "    // l:136
"    return lsh | (readVRAM16(address + 2u) << 16u);\n                                              "    // l:137
"}\n                                                                                                "    // l:138
"\n                                                                                                 "    // l:139
"uint readIOreg(uint address) {\n                                                                   "    // l:140
"    return texelFetch(\n                                                                           "    // l:141
"        IO, ivec2(address >> 1u, uint(OnScreenPos.y)), 0\n                                         "    // l:142
"    ).x;\n                                                                                         "    // l:143
"}\n                                                                                                "    // l:144
"\n                                                                                                 "    // l:145
"ivec4 readOAMentry(uint index) {\n                                                                 "    // l:146
"    return texelFetch(\n                                                                           "    // l:147
"        OAM, ivec2(index + 0u, 0), 0\n                                                             "    // l:148
"    );\n                                                                                           "    // l:149
"}\n                                                                                                "    // l:150
"\n                                                                                                 "    // l:151
"vec4 readPALentry(uint index) {\n                                                                  "    // l:152
"    // Decode 15-bit color (BGR 555)\n                                                             "    // l:153
"    // format: 0BBBBBGGGGGRRRRR\n                                                                  "    // l:154
"    uint val = texelFetch(\n                                                                       "    // l:155
"        PAL, ivec2(index, PALBufferIndex[uint(OnScreenPos.y)]), 0\n                                "    // l:156
"    ).r;\n                                                                                         "    // l:157
"    \n                                                                                             "    // l:158
"    float r = float(val & 0x1Fu) / 31.0;\n                                                         "    // l:159
"    float g = float((val >> 5u) & 0x1Fu) / 31.0;\n                                                 "    // l:160
"    float b = float((val >> 10u) & 0x1Fu) / 31.0;\n                                                "    // l:161
"    \n                                                                                             "    // l:162
"    return vec4(r, g, b, 1.0);\n                                                                   "    // l:163
"}\n                                                                                                "    // l:164
"\n                                                                                                 "    // l:165
"uint getWindow(uint x, uint y) {\n                                                                 "    // l:166
"    return texelFetch(\n                                                                           "    // l:167
"        Window, ivec2(x, 160u - y), 0\n                                                            "    // l:168
"    ).r;\n                                                                                         "    // l:169
"}\n                                                                                                "    // l:170
"\n                                                                                                 "    // l:171
;


// FragmentShaderMode0Source (from fragment_mode0.glsl, lines 3 to 27)
const char* FragmentShaderMode0Source = 
"// Declarations moved to shared header (fragment.glsl/fragment_helpers.glsl)\n                     "    // l:1
"\n                                                                                                 "    // l:2
"vec4 mode0(uint x, uint y) {\n                                                                     "    // l:3
"    uint DISPCNT = readIOreg(0x00u);\n                                                             "    // l:4
"\n                                                                                                 "    // l:5
"    uint BGCNT = readIOreg(0x08u + (BG << 1));\n                                                   "    // l:6
"\n                                                                                                 "    // l:7
"    vec4 Color;\n                                                                                  "    // l:8
"    if ((DISPCNT & (0x0100u << BG)) == 0u) {\n                                                     "    // l:9
"        discard;  // background disabled\n                                                         "    // l:10
"    }\n                                                                                            "    // l:11
"\n                                                                                                 "    // l:12
"    Color = regularBGPixel(BGCNT, x, y);\n                                                         "    // l:13
"\n                                                                                                 "    // l:14
"    if (Color.w != 0.0) {\n                                                                        "    // l:15
"        // priority\n                                                                              "    // l:16
"        gl_FragDepth = getDepth(BGCNT);\n                                                          "    // l:17
"        return Color;\n                                                                            "    // l:18
"    }\n                                                                                            "    // l:19
"    else {\n                                                                                       "    // l:20
"        discard;\n                                                                                 "    // l:21
"    }\n                                                                                            "    // l:22
"}\n                                                                                                "    // l:23
"\n                                                                                                 "    // l:24
;


// FragmentShaderMode1Source (from fragment_mode1.glsl, lines 3 to 38)
const char* FragmentShaderMode1Source = 
"// Declarations moved to shared header (fragment.glsl/fragment_helpers.glsl)\n                     "    // l:1
"\n                                                                                                 "    // l:2
"vec4 mode1(uint x, uint y, vec2 screen_pos) {\n                                                    "    // l:3
"    if (BG == 3u) {\n                                                                              "    // l:4
"        // BG 3 is not drawn\n                                                                     "    // l:5
"        discard;\n                                                                                 "    // l:6
"    }\n                                                                                            "    // l:7
"\n                                                                                                 "    // l:8
"    uint DISPCNT = readIOreg(0x00u);\n                                                             "    // l:9
"\n                                                                                                 "    // l:10
"    uint BGCNT = readIOreg(0x08u + (BG << 1));\n                                                   "    // l:11
"\n                                                                                                 "    // l:12
"    vec4 Color;\n                                                                                  "    // l:13
"    if ((DISPCNT & (0x0100u << BG)) == 0u) {\n                                                     "    // l:14
"        discard;  // background disabled\n                                                         "    // l:15
"    }\n                                                                                            "    // l:16
"\n                                                                                                 "    // l:17
"\n                                                                                                 "    // l:18
"    if (BG < 2u) {\n                                                                               "    // l:19
"        Color = regularBGPixel(BGCNT, x, y);\n                                                     "    // l:20
"    }\n                                                                                            "    // l:21
"    else {\n                                                                                       "    // l:22
"        Color = affineBGPixel(BGCNT, screen_pos);\n                                                "    // l:23
"    }\n                                                                                            "    // l:24
"\n                                                                                                 "    // l:25
"    if (Color.w != 0.0) {\n                                                                        "    // l:26
"        // background priority\n                                                                   "    // l:27
"        gl_FragDepth = getDepth(BGCNT);\n                                                          "    // l:28
"        return Color;\n                                                                            "    // l:29
"    }\n                                                                                            "    // l:30
"    else {\n                                                                                       "    // l:31
"        discard;\n                                                                                 "    // l:32
"    }\n                                                                                            "    // l:33
"}\n                                                                                                "    // l:34
"\n                                                                                                 "    // l:35
;


// FragmentShaderMode2Source (from fragment_mode2.glsl, lines 3 to 32)
const char* FragmentShaderMode2Source = 
"// Declarations moved to shared header (fragment.glsl/fragment_helpers.glsl)\n                     "    // l:1
"\n                                                                                                 "    // l:2
"vec4 mode2(uint x, uint y, vec2 screen_pos) {\n                                                    "    // l:3
"    if (BG < 2u) {\n                                                                               "    // l:4
"        // only BG2 and 3 enabled\n                                                                "    // l:5
"        discard;\n                                                                                 "    // l:6
"    }\n                                                                                            "    // l:7
"\n                                                                                                 "    // l:8
"    uint DISPCNT = readIOreg(0x00u);\n                                                             "    // l:9
"\n                                                                                                 "    // l:10
"    uint BGCNT = readIOreg(0x08u + (BG << 1));\n                                                   "    // l:11
"\n                                                                                                 "    // l:12
"    vec4 Color;\n                                                                                  "    // l:13
"    if ((DISPCNT & (0x0100u << BG)) == 0u) {\n                                                     "    // l:14
"        discard;  // background disabled\n                                                         "    // l:15
"    }\n                                                                                            "    // l:16
"\n                                                                                                 "    // l:17
"    Color = affineBGPixel(BGCNT, screen_pos);\n                                                    "    // l:18
"\n                                                                                                 "    // l:19
"    if (Color.w != 0.0) {\n                                                                        "    // l:20
"        // BG priority\n                                                                           "    // l:21
"        gl_FragDepth = getDepth(BGCNT);\n                                                          "    // l:22
"        return Color;\n                                                                            "    // l:23
"    }\n                                                                                            "    // l:24
"    else {\n                                                                                       "    // l:25
"        discard;\n                                                                                 "    // l:26
"    }\n                                                                                            "    // l:27
"}\n                                                                                                "    // l:28
"\n                                                                                                 "    // l:29
;


// FragmentShaderMode3Source (from fragment_mode3.glsl, lines 3 to 38)
const char* FragmentShaderMode3Source = 
"// Declarations moved to shared header (fragment.glsl/fragment_helpers.glsl)\n                     "    // l:1
"\n                                                                                                 "    // l:2
"\n                                                                                                 "    // l:3
"vec4 mode3(uint x, uint y) {\n                                                                     "    // l:4
"    if (BG != 2u) {\n                                                                              "    // l:5
"        // only BG2 is drawn\n                                                                     "    // l:6
"        discard;\n                                                                                 "    // l:7
"    }\n                                                                                            "    // l:8
"\n                                                                                                 "    // l:9
"    uint DISPCNT = readIOreg(0x00u);\n                                                             "    // l:10
"\n                                                                                                 "    // l:11
"    if ((DISPCNT & 0x0400u) == 0u) {\n                                                             "    // l:12
"        // background 2 is disabled\n                                                              "    // l:13
"        discard;\n                                                                                 "    // l:14
"    }\n                                                                                            "    // l:15
"\n                                                                                                 "    // l:16
"    uint VRAMAddr = (240u * y + x) << 1;  // 16bpp\n                                               "    // l:17
"\n                                                                                                 "    // l:18
"    uint PackedColor = readVRAM16(VRAMAddr);\n                                                     "    // l:19
"\n                                                                                                 "    // l:20
"    vec4 Color = vec4(0.0, 0.0, 0.0, 32.0);  // to be scaled later\n                               "    // l:21
"\n                                                                                                 "    // l:22
"    // BGR format\n                                                                                "    // l:23
"    Color.r = float(PackedColor & 0x1fu);\n                                                        "    // l:24
"    PackedColor >>= 5u;\n                                                                          "    // l:25
"    Color.g = float(PackedColor & 0x1fu);\n                                                        "    // l:26
"    PackedColor >>= 5u;\n                                                                          "    // l:27
"    Color.b = float(PackedColor & 0x1fu);\n                                                        "    // l:28
"\n                                                                                                 "    // l:29
"    uint BGCNT = readIOreg(0x0cu);\n                                                               "    // l:30
"    gl_FragDepth = getDepth(BGCNT);\n                                                              "    // l:31
"\n                                                                                                 "    // l:32
"    return Color / 32.0;\n                                                                         "    // l:33
"}\n                                                                                                "    // l:34
"\n                                                                                                 "    // l:35
;


// FragmentShaderMode4Source (from fragment_mode4.glsl, lines 3 to 37)
const char* FragmentShaderMode4Source = 
"// Declarations moved to shared header (fragment.glsl/fragment_helpers.glsl)\n                     "    // l:1
"\n                                                                                                 "    // l:2
"vec4 mode4(uint x, uint y) {\n                                                                     "    // l:3
"    if (BG != 2u) {\n                                                                              "    // l:4
"        // only BG2 is drawn\n                                                                     "    // l:5
"        discard;\n                                                                                 "    // l:6
"    }\n                                                                                            "    // l:7
"\n                                                                                                 "    // l:8
"    uint DISPCNT = readIOreg(0x00u);\n                                                             "    // l:9
"\n                                                                                                 "    // l:10
"    if ((DISPCNT & 0x0400u) == 0u) {\n                                                             "    // l:11
"        // background 2 is disabled\n                                                              "    // l:12
"        discard;\n                                                                                 "    // l:13
"    }\n                                                                                            "    // l:14
"\n                                                                                                 "    // l:15
"    // offset is specified in DISPCNT\n                                                            "    // l:16
"    uint Offset = 0u;\n                                                                            "    // l:17
"    if ((DISPCNT & 0x0010u) != 0u) {\n                                                             "    // l:18
"        // offset\n                                                                                "    // l:19
"        Offset = 0xa000u;\n                                                                        "    // l:20
"    }\n                                                                                            "    // l:21
"\n                                                                                                 "    // l:22
"    uint VRAMAddr = (240u * y + x);\n                                                              "    // l:23
"    VRAMAddr += Offset;\n                                                                          "    // l:24
"    uint PaletteIndex = readVRAM8(VRAMAddr);\n                                                     "    // l:25
"\n                                                                                                 "    // l:26
"    vec4 Color = readPALentry(PaletteIndex);\n                                                     "    // l:27
"    uint BGCNT = readIOreg(0x0cu);\n                                                               "    // l:28
"    gl_FragDepth = getDepth(BGCNT);;\n                                                             "    // l:29
"\n                                                                                                 "    // l:30
"    // We already converted to BGR when buffering data\n                                           "    // l:31
"    return vec4(Color.rgb, 1.0);\n                                                                 "    // l:32
"}\n                                                                                                "    // l:33
"\n                                                                                                 "    // l:34
;


// ObjectFragmentShaderSource (from object_fragment.glsl, lines 4 to 278)
const char* ObjectFragmentShaderSource = 
"#define attr0 x\n                                                                                  "    // l:1
"#define attr1 y\n                                                                                  "    // l:2
"#define attr2 z\n                                                                                  "    // l:3
"#define attr3 w\n                                                                                  "    // l:4
"\n                                                                                                 "    // l:5
"in vec2 InObjPos;\n                                                                                "    // l:6
"\n                                                                                                 "    // l:7
"flat in uvec4 OBJ;\n                                                                               "    // l:8
"flat in uint ObjWidth;\n                                                                           "    // l:9
"flat in uint ObjHeight;\n                                                                          "    // l:10
"\n                                                                                                 "    // l:11
"uniform bool Affine;\n                                                                             "    // l:12
"uniform uint YClipStart;\n                                                                         "    // l:13
"uniform uint YClipEnd;\n                                                                           "    // l:14
"\n                                                                                                 "    // l:15
"#ifdef OBJ_WINDOW\n                                                                                "    // l:16
"    out uvec4 FragColor;\n                                                                         "    // l:17
"#else\n                                                                                            "    // l:18
"    out vec4 FragColor;\n                                                                          "    // l:19
"#endif\n                                                                                           "    // l:20
"\n                                                                                                 "    // l:21
"vec4 ColorCorrect(vec4 color);\n                                                                   "    // l:22
"vec4 AlphaCorrect(vec4 color, uint layer, uint window);\n                                          "    // l:23
"\n                                                                                                 "    // l:24
"// Prototypes defined in helpers\n                                                                 "    // l:25
"\n                                                                                                 "    // l:26
"uint getWindow(uint x, uint y);\n                                                                  "    // l:27
"\n                                                                                                 "    // l:28
"vec4 RegularObject(bool OAM2DMapping) {\n                                                          "    // l:29
"    uint TID = OBJ.attr2 & 0x03ffu;\n                                                              "    // l:30
"\n                                                                                                 "    // l:31
"    uint dx = uint(InObjPos.x);\n                                                                  "    // l:32
"    uint dy = uint(InObjPos.y);\n                                                                  "    // l:33
"\n                                                                                                 "    // l:34
"    // mosaic effect\n                                                                             "    // l:35
"    if ((OBJ.attr0 & 0x1000u) != 0u) {\n                                                           "    // l:36
"        uint MOSAIC = readIOreg(0x4cu);\n                                                          "    // l:37
"        dx -= dx % (((MOSAIC & 0xf00u) >> 8) + 1u);\n                                              "    // l:38
"        dy -= dy % (((MOSAIC & 0xf000u) >> 12) + 1u);\n                                            "    // l:39
"    }\n                                                                                            "    // l:40
"\n                                                                                                 "    // l:41
"    uint PixelAddress;\n                                                                           "    // l:42
"    if ((OBJ.attr0 & 0x2000u) == 0x0000u) {\n                                                      "    // l:43
"        uint PaletteBank = OBJ.attr2 >> 12;\n                                                      "    // l:44
"        PixelAddress = TID << 5;\n                                                                 "    // l:45
"\n                                                                                                 "    // l:46
"        // get base address for line of tiles (vertically)\n                                       "    // l:47
"        if (OAM2DMapping) {\n                                                                      "    // l:48
"            PixelAddress += ObjWidth * (dy >> 3) << 2;\n                                           "    // l:49
"        }\n                                                                                        "    // l:50
"        else {\n                                                                                   "    // l:51
"            PixelAddress += 32u * 0x20u * (dy >> 3);\n                                             "    // l:52
"        }\n                                                                                        "    // l:53
"        PixelAddress += (dy & 7u) << 2; // offset within tile for sliver\n                         "    // l:54
"\n                                                                                                 "    // l:55
"        // Sprites VRAM base address is 0x10000\n                                                  "    // l:56
"        PixelAddress = (PixelAddress & 0x7fffu) | 0x10000u;\n                                      "    // l:57
"\n                                                                                                 "    // l:58
"        // horizontal offset:\n                                                                    "    // l:59
"        PixelAddress += (dx >> 3) << 5;    // of tile\n                                            "    // l:60
"        PixelAddress += ((dx & 7u) >> 1);  // in tile\n                                            "    // l:61
"\n                                                                                                 "    // l:62
"        uint VRAMEntry = readVRAM8(PixelAddress);\n                                                "    // l:63
"        if ((dx & 1u) != 0u) {\n                                                                   "    // l:64
"            // upper nibble\n                                                                      "    // l:65
"            VRAMEntry >>= 4;\n                                                                     "    // l:66
"        }\n                                                                                        "    // l:67
"        else {\n                                                                                   "    // l:68
"            VRAMEntry &= 0x0fu;\n                                                                  "    // l:69
"        }\n                                                                                        "    // l:70
"\n                                                                                                 "    // l:71
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:72
"            return vec4(readPALentry(0x100u + (PaletteBank << 4) + VRAMEntry).rgb, 1);\n           "    // l:73
"        }\n                                                                                        "    // l:74
"        else {\n                                                                                   "    // l:75
"            // transparent\n                                                                       "    // l:76
"            discard;\n                                                                             "    // l:77
"        }\n                                                                                        "    // l:78
"    }\n                                                                                            "    // l:79
"    else {\n                                                                                       "    // l:80
"        // 8bpp\n                                                                                  "    // l:81
"        PixelAddress = TID << 5;\n                                                                 "    // l:82
"\n                                                                                                 "    // l:83
"        if (OAM2DMapping) {\n                                                                      "    // l:84
"            PixelAddress += ObjWidth * (dy & ~7u);\n                                               "    // l:85
"        }\n                                                                                        "    // l:86
"        else {\n                                                                                   "    // l:87
"            PixelAddress += 32u * 0x20u * (dy >> 3);\n                                             "    // l:88
"        }\n                                                                                        "    // l:89
"        PixelAddress += (dy & 7u) << 3;\n                                                          "    // l:90
"\n                                                                                                 "    // l:91
"        // Sprites VRAM base address is 0x10000\n                                                  "    // l:92
"        PixelAddress = (PixelAddress & 0x7fffu) | 0x10000u;\n                                      "    // l:93
"\n                                                                                                 "    // l:94
"        // horizontal offset:\n                                                                    "    // l:95
"        PixelAddress += (dx >> 3) << 6;\n                                                          "    // l:96
"        PixelAddress += dx & 7u;\n                                                                 "    // l:97
"\n                                                                                                 "    // l:98
"        uint VRAMEntry = readVRAM8(PixelAddress);\n                                                "    // l:99
"\n                                                                                                 "    // l:100
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:101
"            return vec4(readPALentry(0x100u + VRAMEntry).rgb, 1);\n                                "    // l:102
"        }\n                                                                                        "    // l:103
"        else {\n                                                                                   "    // l:104
"            // transparent\n                                                                       "    // l:105
"            discard;\n                                                                             "    // l:106
"        }\n                                                                                        "    // l:107
"    }\n                                                                                            "    // l:108
"}\n                                                                                                "    // l:109
"\n                                                                                                 "    // l:110
"bool InsideBox(vec2 v, vec2 bottomLeft, vec2 topRight) {\n                                         "    // l:111
"    vec2 s = step(bottomLeft, v) - step(topRight, v);\n                                            "    // l:112
"    return (s.x * s.y) != 0.0;\n                                                                   "    // l:113
"}\n                                                                                                "    // l:114
"\n                                                                                                 "    // l:115
"vec4 AffineObject(bool OAM2DMapping) {\n                                                           "    // l:116
"    uint TID = OBJ.attr2 & 0x03ffu;\n                                                              "    // l:117
"\n                                                                                                 "    // l:118
"    uint AffineIndex = (OBJ.attr1 & 0x3e00u) >> 9;\n                                               "    // l:119
"    AffineIndex <<= 2;  // goes in groups of 4\n                                                   "    // l:120
"\n                                                                                                 "    // l:121
"    // scaling parameters\n                                                                        "    // l:122
"    int PA = readOAMentry(AffineIndex).attr3;\n                                                    "    // l:123
"    int PB = readOAMentry(AffineIndex + 1u).attr3;\n                                               "    // l:124
"    int PC = readOAMentry(AffineIndex + 2u).attr3;\n                                               "    // l:125
"    int PD = readOAMentry(AffineIndex + 3u).attr3;\n                                               "    // l:126
"\n                                                                                                 "    // l:127
"    // reference point\n                                                                           "    // l:128
"    vec2 p0 = vec2(\n                                                                              "    // l:129
"        float(ObjWidth  >> 1),\n                                                                   "    // l:130
"        float(ObjHeight >> 1)\n                                                                    "    // l:131
"    );\n                                                                                           "    // l:132
"\n                                                                                                 "    // l:133
"    vec2 p1;\n                                                                                     "    // l:134
"    if ((OBJ.attr0 & 0x0300u) == 0x0300u) {\n                                                      "    // l:135
"        // double rendering\n                                                                      "    // l:136
"        p1 = 2.0 * p0;\n                                                                           "    // l:137
"    }\n                                                                                            "    // l:138
"    else {\n                                                                                       "    // l:139
"        p1 = p0;\n                                                                                 "    // l:140
"    }\n                                                                                            "    // l:141
"\n                                                                                                 "    // l:142
"    mat2x2 rotscale = mat2x2(\n                                                                    "    // l:143
"        float(PA), float(PC),\n                                                                    "    // l:144
"        float(PB), float(PD)\n                                                                     "    // l:145
"    ) / 256.0;  // fixed point stuff\n                                                             "    // l:146
"\n                                                                                                 "    // l:147
"    ivec2 pos = ivec2(rotscale * (InObjPos - p1) + p0);\n                                          "    // l:148
"    if (!InsideBox(vec2(pos), vec2(0, 0), vec2(ObjWidth, ObjHeight))) {\n                          "    // l:149
"        // out of bounds\n                                                                         "    // l:150
"        discard;\n                                                                                 "    // l:151
"    }\n                                                                                            "    // l:152
"\n                                                                                                 "    // l:153
"    // mosaic effect\n                                                                             "    // l:154
"    if ((OBJ.attr0 & 0x1000u) != 0u) {\n                                                           "    // l:155
"        uint MOSAIC = readIOreg(0x4cu);\n                                                          "    // l:156
"        pos.x -= pos.x % int(((MOSAIC & 0xf00u) >> 8) + 1u);\n                                     "    // l:157
"        pos.y -= pos.y % int(((MOSAIC & 0xf000u) >> 12) + 1u);\n                                   "    // l:158
"    }\n                                                                                            "    // l:159
"\n                                                                                                 "    // l:160
"    // get actual pixel\n                                                                          "    // l:161
"    uint PixelAddress = 0x10000u;  // OBJ VRAM starts at 0x10000 in VRAM\n                         "    // l:162
"    PixelAddress += TID << 5;\n                                                                    "    // l:163
"    if (OAM2DMapping) {\n                                                                          "    // l:164
"        PixelAddress += ObjWidth * uint(pos.y & ~7) >> 1;\n                                        "    // l:165
"    }\n                                                                                            "    // l:166
"    else {\n                                                                                       "    // l:167
"        PixelAddress += 32u * 0x20u * uint(pos.y >> 3);\n                                          "    // l:168
"    }\n                                                                                            "    // l:169
"\n                                                                                                 "    // l:170
"    // the rest is very similar to regular sprites:\n                                              "    // l:171
"    if ((OBJ.attr0 & 0x2000u) == 0x0000u) {\n                                                      "    // l:172
"        uint PaletteBank = OBJ.attr2 >> 12;\n                                                      "    // l:173
"        PixelAddress += uint(pos.y & 7) << 2; // offset within tile for sliver\n                   "    // l:174
"\n                                                                                                 "    // l:175
"        // horizontal offset:\n                                                                    "    // l:176
"        PixelAddress += uint(pos.x >> 3) << 5;    // of tile\n                                     "    // l:177
"        PixelAddress += uint(pos.x & 7) >> 1;  // in tile\n                                        "    // l:178
"\n                                                                                                 "    // l:179
"        uint VRAMEntry = readVRAM8(PixelAddress);\n                                                "    // l:180
"        if ((pos.x & 1) != 0) {\n                                                                  "    // l:181
"            // upper nibble\n                                                                      "    // l:182
"            VRAMEntry >>= 4;\n                                                                     "    // l:183
"        }\n                                                                                        "    // l:184
"        else {\n                                                                                   "    // l:185
"            VRAMEntry &= 0x0fu;\n                                                                  "    // l:186
"        }\n                                                                                        "    // l:187
"\n                                                                                                 "    // l:188
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:189
"            return vec4(readPALentry(0x100u + (PaletteBank << 4) + VRAMEntry).rgb, 1);\n           "    // l:190
"        }\n                                                                                        "    // l:191
"        else {\n                                                                                   "    // l:192
"            // transparent\n                                                                       "    // l:193
"            discard;\n                                                                             "    // l:194
"        }\n                                                                                        "    // l:195
"    }\n                                                                                            "    // l:196
"    else {\n                                                                                       "    // l:197
"        PixelAddress += (uint(pos.y) & 7u) << 3; // offset within tile for sliver\n                "    // l:198
"\n                                                                                                 "    // l:199
"        // horizontal offset:\n                                                                    "    // l:200
"        PixelAddress += uint(pos.x >> 3) << 6;  // of tile\n                                       "    // l:201
"        PixelAddress += uint(pos.x & 7);        // in tile\n                                       "    // l:202
"\n                                                                                                 "    // l:203
"        uint VRAMEntry = readVRAM8(PixelAddress);\n                                                "    // l:204
"\n                                                                                                 "    // l:205
"        if (VRAMEntry != 0u) {\n                                                                   "    // l:206
"            return vec4(readPALentry(0x100u + VRAMEntry).rgb, 1);\n                                "    // l:207
"        }\n                                                                                        "    // l:208
"        else {\n                                                                                   "    // l:209
"            // transparent\n                                                                       "    // l:210
"            discard;\n                                                                             "    // l:211
"        }\n                                                                                        "    // l:212
"    }\n                                                                                            "    // l:213
"}\n                                                                                                "    // l:214
"\n                                                                                                 "    // l:215
"void main() {\n                                                                                    "    // l:216
"    if (OnScreenPos.x < 0.0) {\n                                                                   "    // l:217
"        discard;\n                                                                                 "    // l:218
"    }\n                                                                                            "    // l:219
"    if (OnScreenPos.x > float(240u)) {\n                                                           "    // l:220
"        discard;\n                                                                                 "    // l:221
"    }\n                                                                                            "    // l:222
"\n                                                                                                 "    // l:223
"    if (OnScreenPos.y < float(YClipStart)) {\n                                                     "    // l:224
"        discard;\n                                                                                 "    // l:225
"    }\n                                                                                            "    // l:226
"    if (OnScreenPos.y > float(YClipEnd)) {\n                                                       "    // l:227
"        discard;\n                                                                                 "    // l:228
"    }\n                                                                                            "    // l:229
"\n                                                                                                 "    // l:230
"    uint DISPCNT = readIOreg(0x00u);\n                                                             "    // l:231
"#ifndef OBJ_WINDOW\n                                                                               "    // l:232
"    if ((DISPCNT & 0x1000u) == 0u) {\n                                                             "    // l:233
"        // objects disabled in this scanline\n                                                     "    // l:234
"        discard;\n                                                                                 "    // l:235
"    }\n                                                                                            "    // l:236
"    uint window = getWindow(uint(OnScreenPos.x), uint(OnScreenPos.y));\n                           "    // l:237
"    if ((window & 0x10u) == 0u) {\n                                                                "    // l:238
"        // disabled by window\n                                                                    "    // l:239
"        discard;\n                                                                                 "    // l:240
"    }\n                                                                                            "    // l:241
"#else\n                                                                                            "    // l:242
"    if ((DISPCNT & 0x8000u) == 0u) {\n                                                             "    // l:243
"        // object window disabled in this scanline\n                                               "    // l:244
"        discard;\n                                                                                 "    // l:245
"    }\n                                                                                            "    // l:246
"#endif\n                                                                                           "    // l:247
"\n                                                                                                 "    // l:248
"    bool OAM2DMapping = (DISPCNT & (0x0040u)) != 0u;\n                                             "    // l:249
"\n                                                                                                 "    // l:250
"    vec4 Color;\n                                                                                  "    // l:251
"    if (!Affine) {\n                                                                               "    // l:252
"        Color = RegularObject(OAM2DMapping);\n                                                     "    // l:253
"    }\n                                                                                            "    // l:254
"    else{\n                                                                                        "    // l:255
"        Color = AffineObject(OAM2DMapping);\n                                                      "    // l:256
"    }\n                                                                                            "    // l:257
"\n                                                                                                 "    // l:258
"#ifndef OBJ_WINDOW\n                                                                               "    // l:259
"    FragColor = ColorCorrect(Color);\n                                                             "    // l:260
"    if ((OBJ.attr0 & 0x0c00u) == 0x0400u) {\n                                                      "    // l:261
"        FragColor = AlphaCorrect(FragColor, 4u, window);\n                                         "    // l:262
"    }\n                                                                                            "    // l:263
"    else {\n                                                                                       "    // l:264
"        FragColor = vec4(FragColor.rgb, 0.0);\n                                                    "    // l:265
"    }\n                                                                                            "    // l:266
"#else\n                                                                                            "    // l:267
"    // RegularObject/AffineObject will only return if it is nontransparent\n                       "    // l:268
"    uint WINOBJ = (readIOreg(0x4au) >> 8) & 0x3fu;\n                                               "    // l:269
"\n                                                                                                 "    // l:270
"    FragColor.r = WINOBJ;\n                                                                        "    // l:271
"#endif\n                                                                                           "    // l:272
"}\n                                                                                                "    // l:273
"\n                                                                                                 "    // l:274
;


// ObjectVertexShaderSource (from object_vertex.glsl, lines 4 to 123)
const char* ObjectVertexShaderSource = 
"#define attr0 x\n                                                                                  "    // l:1
"#define attr1 y\n                                                                                  "    // l:2
"#define attr2 z\n                                                                                  "    // l:3
"#define attr3 w\n                                                                                  "    // l:4
"\n                                                                                                 "    // l:5
"uniform bool Affine;\n                                                                             "    // l:6
"\n                                                                                                 "    // l:7
"layout (location = 0) in uvec4 InOBJ;\n                                                            "    // l:8
"\n                                                                                                 "    // l:9
"out vec2 InObjPos;\n                                                                               "    // l:10
"out vec2 OnScreenPos;\n                                                                            "    // l:11
"flat out uvec4 OBJ;\n                                                                              "    // l:12
"flat out uint ObjWidth;\n                                                                          "    // l:13
"flat out uint ObjHeight;\n                                                                         "    // l:14
"\n                                                                                                 "    // l:15
"struct s_ObjSize {\n                                                                               "    // l:16
"    uint width;\n                                                                                  "    // l:17
"    uint height;\n                                                                                 "    // l:18
"};\n                                                                                               "    // l:19
"\n                                                                                                 "    // l:20
"const s_ObjSize ObjSizeTable[12] = s_ObjSize[](\n                                                  "    // l:21
"    s_ObjSize(8u, 8u),  s_ObjSize(16u, 16u), s_ObjSize(32u, 32u), s_ObjSize(64u, 64u),\n           "    // l:22
"    s_ObjSize(16u, 8u), s_ObjSize(32u, 8u),  s_ObjSize(32u, 16u), s_ObjSize(64u, 32u),\n           "    // l:23
"    s_ObjSize(8u, 16u), s_ObjSize(8u, 32u),  s_ObjSize(16u, 32u), s_ObjSize(32u, 62u)\n            "    // l:24
");\n                                                                                               "    // l:25
"\n                                                                                                 "    // l:26
"struct s_Position {\n                                                                              "    // l:27
"    bool right;\n                                                                                  "    // l:28
"    bool low;\n                                                                                    "    // l:29
"};\n                                                                                               "    // l:30
"\n                                                                                                 "    // l:31
"const s_Position PositionTable[4] = s_Position[](\n                                                "    // l:32
"    s_Position(false, false), s_Position(true, false), s_Position(true, true), s_Position(false, true)\n"    // l:33
");\n                                                                                               "    // l:34
"\n                                                                                                 "    // l:35
"void main() {\n                                                                                    "    // l:36
"    OBJ = InOBJ;\n                                                                                 "    // l:37
"    s_Position Position = PositionTable[gl_VertexID & 3];\n                                        "    // l:38
"\n                                                                                                 "    // l:39
"    uint Shape = OBJ.attr0 >> 14;\n                                                                "    // l:40
"    uint Size  = OBJ.attr1 >> 14;\n                                                                "    // l:41
"\n                                                                                                 "    // l:42
"    s_ObjSize ObjSize = ObjSizeTable[(Shape * 4u) + Size];\n                                       "    // l:43
"    ObjWidth = ObjSize.width;\n                                                                    "    // l:44
"    ObjHeight = ObjSize.height;\n                                                                  "    // l:45
"\n                                                                                                 "    // l:46
"    ivec2 ScreenPos = ivec2(OBJ.attr1 & 0x1ffu, OBJ.attr0 & 0xffu);\n                              "    // l:47
"\n                                                                                                 "    // l:48
"    // correct position for screen wrapping\n                                                      "    // l:49
"    if (ScreenPos.x > int(240u)) {\n                                                               "    // l:50
"        ScreenPos.x -= 0x200;\n                                                                    "    // l:51
"    }\n                                                                                            "    // l:52
"\n                                                                                                 "    // l:53
"    if (ScreenPos.y > int(160u)) {\n                                                               "    // l:54
"        ScreenPos.y -= 0x100;\n                                                                    "    // l:55
"    }\n                                                                                            "    // l:56
"\n                                                                                                 "    // l:57
"    InObjPos = vec2(0.0, 0.0);\n                                                                   "    // l:58
"    if (Position.right) {\n                                                                        "    // l:59
"        InObjPos.x  += float(ObjWidth);\n                                                          "    // l:60
"        ScreenPos.x += int(ObjWidth);\n                                                            "    // l:61
"\n                                                                                                 "    // l:62
"        if (Affine) {\n                                                                            "    // l:63
"            if ((OBJ.attr0 & 0x0300u) == 0x0300u) {\n                                              "    // l:64
"                // double rendering\n                                                              "    // l:65
"                InObjPos.x  += float(ObjWidth);\n                                                  "    // l:66
"                ScreenPos.x += int(ObjWidth);\n                                                    "    // l:67
"            }\n                                                                                    "    // l:68
"        }\n                                                                                        "    // l:69
"    }\n                                                                                            "    // l:70
"\n                                                                                                 "    // l:71
"    if (Position.low) {\n                                                                          "    // l:72
"        InObjPos.y  += float(ObjHeight);\n                                                         "    // l:73
"        ScreenPos.y += int(ObjHeight);\n                                                           "    // l:74
"\n                                                                                                 "    // l:75
"        if (Affine) {\n                                                                            "    // l:76
"            if ((OBJ.attr0 & 0x0300u) == 0x0300u) {\n                                              "    // l:77
"                // double rendering\n                                                              "    // l:78
"                InObjPos.y  += float(ObjHeight);\n                                                 "    // l:79
"                ScreenPos.y += int(ObjHeight);\n                                                   "    // l:80
"            }\n                                                                                    "    // l:81
"        }\n                                                                                        "    // l:82
"    }\n                                                                                            "    // l:83
"\n                                                                                                 "    // l:84
"    // flipping only applies to regular sprites\n                                                  "    // l:85
"    if (!Affine) {\n                                                                               "    // l:86
"        if ((OBJ.attr1 & 0x2000u) != 0u) {\n                                                       "    // l:87
"            // VFlip\n                                                                             "    // l:88
"            InObjPos.y = float(ObjHeight) - InObjPos.y;\n                                          "    // l:89
"        }\n                                                                                        "    // l:90
"\n                                                                                                 "    // l:91
"        if ((OBJ.attr1 & 0x1000u) != 0u) {\n                                                       "    // l:92
"            // HFlip\n                                                                             "    // l:93
"            InObjPos.x = float(ObjWidth) - InObjPos.x;\n                                           "    // l:94
"        }\n                                                                                        "    // l:95
"    }\n                                                                                            "    // l:96
"\n                                                                                                 "    // l:97
"    OnScreenPos = vec2(ScreenPos);\n                                                               "    // l:98
"\n                                                                                                 "    // l:99
"#ifndef OBJ_WINDOW\n                                                                               "    // l:100
"    // depth is the same everywhere in the object anyway\n                                         "    // l:101
"    uint Priority = (OBJ.attr2 & 0x0c00u) >> 10;\n                                                 "    // l:102
"\n                                                                                                 "    // l:103
"    gl_Position = vec4(\n                                                                          "    // l:104
"        -1.0 + 2.0 * OnScreenPos.x / float(240u),\n                                                "    // l:105
"        1.0 - 2.0 * OnScreenPos.y / float(160u),\n                                                 "    // l:106
"        -1.0 + float(Priority) / 2.0,  // /2.0 because openGL clips between -1 and 1 (-1 is in front)\n"    // l:107
"        1.0\n                                                                                      "    // l:108
"    );\n                                                                                           "    // l:109
"#else\n                                                                                            "    // l:110
"    gl_Position = vec4(\n                                                                          "    // l:111
"        -1.0 + 2.0 * OnScreenPos.x / float(240u),\n                                                "    // l:112
"        1.0 - 2.0 * OnScreenPos.y / float(160u),\n                                                 "    // l:113
"        0.5,  // between WIN1 and WINOUT\n                                                         "    // l:114
"        1.0\n                                                                                      "    // l:115
"    );\n                                                                                           "    // l:116
"#endif\n                                                                                           "    // l:117
"}\n                                                                                                "    // l:118
"\n                                                                                                 "    // l:119
;


// VertexShaderSource (from vertex.glsl, lines 3 to 23)
const char* VertexShaderSource = 
"layout (location = 0) in vec2 position;\n                                                          "    // l:1
"\n                                                                                                 "    // l:2
"out vec2 screenCoord;\n                                                                            "    // l:3
"out vec2 OnScreenPos;  // needed for fragment_helpers\n                                            "    // l:4
"\n                                                                                                 "    // l:5
"void main() {\n                                                                                    "    // l:6
"    // convert y coordinate from scanline to screen coordinate\n                                   "    // l:7
"    gl_Position = vec4(\n                                                                          "    // l:8
"        position.x,\n                                                                              "    // l:9
"        1.0 - (2.0 * position.y) / float(160u), 0.0, 1.0\n                                         "    // l:10
"    );\n                                                                                           "    // l:11
"\n                                                                                                 "    // l:12
"    screenCoord = vec2(\n                                                                          "    // l:13
"        float(240u) * float((1.0 + position.x)) / 2.0,\n                                           "    // l:14
"        position.y\n                                                                               "    // l:15
"    );\n                                                                                           "    // l:16
"\n                                                                                                 "    // l:17
"    OnScreenPos = screenCoord;\n                                                                   "    // l:18
"}\n                                                                                                "    // l:19
"\n                                                                                                 "    // l:20
;


// WindowFragmentShaderSource (from window_fragment.glsl, lines 3 to 81)
const char* WindowFragmentShaderSource = 
"in vec2 screenCoord;\n                                                                             "    // l:1
"\n                                                                                                 "    // l:2
"out uvec4 FragColor;\n                                                                             "    // l:3
"\n                                                                                                 "    // l:4
"uint readVRAM8(uint address);\n                                                                    "    // l:5
"uint readVRAM16(uint address);\n                                                                   "    // l:6
"uint readVRAM32(uint address);\n                                                                   "    // l:7
"\n                                                                                                 "    // l:8
"uint readIOreg(uint address);\n                                                                    "    // l:9
"vec4 readPALentry(uint index);\n                                                                   "    // l:10
"\n                                                                                                 "    // l:11
"void main() {\n                                                                                    "    // l:12
"    uint DISPCNT = readIOreg(0x00u);\n                                                             "    // l:13
"\n                                                                                                 "    // l:14
"    if ((DISPCNT & 0xe000u) == 0u) {\n                                                             "    // l:15
"        // windows are disabled, enable all windows\n                                              "    // l:16
"        // we should have caught this before rendering, but eh, I guess we'll check again...\n     "    // l:17
"        FragColor.x = 0x3fu;\n                                                                     "    // l:18
"        gl_FragDepth = 1.0;\n                                                                      "    // l:19
"        return;\n                                                                                  "    // l:20
"    }\n                                                                                            "    // l:21
"\n                                                                                                 "    // l:22
"    uint x = clamp(uint(screenCoord.x), 0u, uint(240u) );\n                                        "    // l:23
"    uint y = clamp(uint(screenCoord.y), 0u, uint(160u)); \n                                        "    // l:24
"\n                                                                                                 "    // l:25
"    // window 0 has higher priority\n                                                              "    // l:26
"    for (uint window = 0u; window < 2u; window++) {\n                                              "    // l:27
"        if ((DISPCNT & (0x2000u << window)) == 0u) {\n                                             "    // l:28
"            // window disabled\n                                                                   "    // l:29
"            continue;\n                                                                            "    // l:30
"        }\n                                                                                        "    // l:31
"\n                                                                                                 "    // l:32
"        uint WINH = readIOreg(0x40u + 2u * window);\n                                              "    // l:33
"        uint WINV = readIOreg(0x44u + 2u * window);\n                                              "    // l:34
"        uint WININ = (readIOreg(0x48u) >> (window * 8u)) & 0x3fu;\n                                "    // l:35
"\n                                                                                                 "    // l:36
"        uint X1 = clamp(WINH >> 8u, 0u, uint(240u) );\n                                            "    // l:37
"        uint X2 = clamp(WINH & 0xffu, 0u, uint(240u) );\n                                          "    // l:38
"        uint Y1 = clamp(WINV >> 8u, 0u, uint(160u));\n                                             "    // l:39
"        uint Y2 = clamp(WINV & 0xffu, 0u, uint(160u));\n                                           "    // l:40
"\n                                                                                                 "    // l:41
"        if (Y1 <= Y2) {\n                                                                          "    // l:42
"            // no vert wrap and out of bounds, continue\n                                          "    // l:43
"            if (y < Y1 || y >= Y2) {\n                                                             "    // l:44
"                continue;\n                                                                        "    // l:45
"            }\n                                                                                    "    // l:46
"        }\n                                                                                        "    // l:47
"        else {\n                                                                                   "    // l:48
"            // vert wrap and \"in bounds\":\n                                                      "    // l:49
"            if ((y < Y1) && (y >= Y2)) {\n                                                         "    // l:50
"                continue;\n                                                                        "    // l:51
"            }\n                                                                                    "    // l:52
"        }\n                                                                                        "    // l:53
"\n                                                                                                 "    // l:54
"        if (X1 <= X2) {\n                                                                          "    // l:55
"            // no hor wrap\n                                                                       "    // l:56
"            if (x >= X1 && x < X2) {\n                                                             "    // l:57
"                // pixel in WININ\n                                                                "    // l:58
"                FragColor.x = WININ;\n                                                             "    // l:59
"                gl_FragDepth = 0.0;\n                                                              "    // l:60
"                return;\n                                                                          "    // l:61
"            }\n                                                                                    "    // l:62
"        }\n                                                                                        "    // l:63
"        else {\n                                                                                   "    // l:64
"            // hor wrap\n                                                                          "    // l:65
"            if (x < X2 || x >= X1) {\n                                                             "    // l:66
"                // pixel in WININ\n                                                                "    // l:67
"                FragColor.x = WININ;\n                                                             "    // l:68
"                gl_FragDepth = 0.0;\n                                                              "    // l:69
"                return;\n                                                                          "    // l:70
"            }\n                                                                                    "    // l:71
"        }\n                                                                                        "    // l:72
"    }\n                                                                                            "    // l:73
"\n                                                                                                 "    // l:74
"    FragColor.x = readIOreg(0x4au) & 0x3fu;  // WINOUT\n                                           "    // l:75
"    gl_FragDepth = 1.0;\n                                                                          "    // l:76
"}\n                                                                                                "    // l:77
"\n                                                                                                 "    // l:78
;

#endif  // GC__SHADER_H