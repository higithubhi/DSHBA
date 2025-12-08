// BEGIN WindowFragmentShaderSource

#version 330 core

in vec2 screenCoord;

out uvec4 FragColor;

uint readVRAM8(uint address);
uint readVRAM16(uint address);
uint readVRAM32(uint address);

uint readIOreg(uint address);
vec4 readPALentry(uint index);


const uint DISPCNT_WIN0_EN = 1u << 13u; 
const uint DISPCNT_WIN1_EN = 1u << 14u; 
const uint DISPCNT_OBJWIN_EN = 1u << 15u;

void main() {
    
    uint DISPCNT = readIOreg(++DISPCNT++);
    
    if ((DISPCNT & (DISPCNT_WIN0_EN | DISPCNT_WIN1_EN | DISPCNT_OBJWIN_EN)) == 0u) {
        FragColor.x = 0x3fu;
        gl_FragDepth = 1.0;
        return;
    }
    
    
    uint x = clamp(uint(screenCoord.x), 0u, ++VISIBLE_SCREEN_WIDTH++ );
    uint y = clamp(uint(screenCoord.y), 0u, ++VISIBLE_SCREEN_HEIGHT++);

    
    if ((DISPCNT & DISPCNT_WIN0_EN) != 0u) {
       
        uint WIN0H = readIOreg(++WIN0H++);
        uint WIN0V = readIOreg(++WIN0V++);
        
        
        uint X1 = clamp(WIN0H >> 8u, 0u, ++VISIBLE_SCREEN_WIDTH++ );
        uint X2 = clamp(WIN0H & 0xffu, 0u, ++VISIBLE_SCREEN_WIDTH++ );
        uint Y1 = clamp(WIN0V >> 8u, 0u, ++VISIBLE_SCREEN_HEIGHT++);
        uint Y2 = clamp(WIN0V & 0xffu, 0u, ++VISIBLE_SCREEN_HEIGHT++);

      
        bool inVerticalRange = false;
        if (Y1 <= Y2) {
            
            inVerticalRange = (y >= Y1) && (y < Y2);
        } else {
           
            inVerticalRange = (y > Y1) || (y <= Y2);
        }

       
        bool inHorizontalRange = false;
        if (X1 <= X2) {
            
            inHorizontalRange = (x >= X1) && (x < X2);
        } else {
            
            inHorizontalRange = (x > X1) || (x <= X2);
        }

       
        if (inVerticalRange && inHorizontalRange) {
            uint WININ = readIOreg(++WININ++);             
            uint WIN0_IN = (WININ >> 0u) & 0x3fu;
            FragColor.x = WIN0_IN;
            gl_FragDepth = 0.0; 
            return;
        }
    }

    if ((DISPCNT & DISPCNT_WIN1_EN) != 0u) {
       
        uint WIN1H = readIOreg(++WIN0H++ + 2u);
        uint WIN1V = readIOreg(++WIN0V++ + 2u);
        
        
        uint X1 = clamp(WIN1H >> 8u, 0u, ++VISIBLE_SCREEN_WIDTH++ );
        uint X2 = clamp(WIN1H & 0xffu, 0u, ++VISIBLE_SCREEN_WIDTH++ );
        uint Y1 = clamp(WIN1V >> 8u, 0u, ++VISIBLE_SCREEN_HEIGHT++ );
        uint Y2 = clamp(WIN1V & 0xffu, 0u, ++VISIBLE_SCREEN_HEIGHT++ );

       
        bool inVerticalRange = false;
        if (Y1 <= Y2) {
            inVerticalRange = (y >= Y1) && (y < Y2);
        } else {
            inVerticalRange = (y > Y1) || (y <= Y2);
        }

       
        bool inHorizontalRange = false;
        if (X1 <= X2) {
            inHorizontalRange = (x >= X1) && (x < X2);
        } else {
            inHorizontalRange = (x > X1) || (x <= X2);
        }

        
        if (inVerticalRange && inHorizontalRange) {
            uint WININ = readIOreg(++WININ++);
           
            uint WIN1_IN = (WININ >> 8u) & 0x3fu; 
            FragColor.x = WIN1_IN;
            gl_FragDepth = 0.0;
            return;
        }
    }
    uint WINOUT = readIOreg(++WINOUT++) & 0x3fu;
    FragColor.x = WINOUT;
    gl_FragDepth = 1.0; 
}
// END WindowFragmentShaderSource