// BEGIN BlitVertexShaderSource
#version 320 es
precision mediump float;
precision highp int;
precision mediump usampler2D;
precision mediump isampler2D;

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 inTexCoord;

out vec2 texCoord;  // needed for fragment_helpers

void main() {
    texCoord = inTexCoord;

    gl_Position = vec4(
        position.x,
        position.y,
        0,
        1
    );
}

// END BlitVertexShaderSource