#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif
    
enum Colors 
{
    // Primary colors
    RED             = 0xFF0000FF,
    GREEN           = 0xFF00FF00,
    BLUE            = 0xFFFF0000,
    // Secondary colors
    CYAN            = 0xFFFFFF00,
    MAGENTA         = 0xFFFF00FF,
    YELLOW          = 0xFF00FFFF,
    // Tertiary colors
    AZURE           = 0xFFFF7F00,
    VIOLET          = 0xFFFF007F,
    ROSE            = 0xFF7F00FF,
    ORANGE          = 0xFF007FFF,
    CHARTREUSE      = 0xFF00FF7F,
    SPRING_GREEN    = 0xFF7FFF00,
    // Grayscale
    WHITE           = 0xFFFFFFFF,
    LITEGRAY        = 0xFFBFBFBF,
    GRAY            = 0xFF7F7F7F,
    DARKGRAY        = 0xFF3F3F3F,
    BLACK           = 0xFF000000
};

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */

