// BEGIN BlitFragmentShaderSource


in vec2 texCoord;

uniform sampler2D TopLayer;
uniform sampler2D BottomLayer;

out vec4 FragColor;

void main() {
    vec4 top = texture(TopLayer, texCoord);
    vec4 bottom = texture(BottomLayer, texCoord);

    // default: pick top
    FragColor = vec4(
        top.rgb, 1
    );
    if ((bottom.a != 0.0) && (bottom.a <= 0.5)) {
        // there was a bottom layer in the bottom framebuffer
        if (top.a >= 0.5) {
            float top_a_decoded = (top.a - 0.5) * 2.0;
            float bottom_a_decoded = (bottom.a - 0.5) * 2.0;

            FragColor = vec4(
                // correct for how we store bottom alpha
                top.rgb * top_a_decoded - 2.0 * bottom.rgb * (bottom_a_decoded + 0.25), 1
            );
        }
    }
}

// END BlitFragmentShaderSource