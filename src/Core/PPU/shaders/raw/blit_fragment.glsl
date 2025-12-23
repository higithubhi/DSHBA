// BEGIN BlitFragmentShaderSource
#version 320 es
precision mediump float;
precision highp int;
precision mediump usampler2D;
precision mediump isampler2D;

in vec2 texCoord;

uniform sampler2D TopLayer;
uniform sampler2D BottomLayer;

out vec4 FragColor;

void main() {
    vec4 top = texture(TopLayer, texCoord);
    vec4 bottom = texture(BottomLayer, texCoord);

    // default: pick top
    FragColor = vec4(
        top.rgb, 1.0
    );

    if ((bottom.a != 0.0) && (bottom.a <= 0.5)) {
        if (top.a >= 0.5) {
            float topASNorm = (top.a * 2.0) - 1.0;
            float bottomASNorm = (bottom.a * 2.0) - 1.0;
            vec3 blendedColor = top.rgb * topASNorm - 2.0 * bottom.rgb * (bottomASNorm + 0.25);
            FragColor = vec4(
                blendedColor, 1.0
            );
        }
    }
}

// END BlitFragmentShaderSource