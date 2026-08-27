//VERSION LOG
//version 1.0: 8/12/2026:       Initial commit.
//version 1.01: 8/17/2026:      Added background() to globe.h, which enables the drawing of a background surface
                                //before the geoscape itself is drawn.
//version 1.02: 8/27/2026:      //updated BuildFrame with correct cross product math to prevent disappearing edge triangles.

#define CURRENT_VERSION     1.02

#define MODE_GEOSCAPE       0
#define MODE_TERROR         1
#define MODE_EARTH          2
#define MODE_DEBUG          3

#define DISPLAY_MODE        MODE_GEOSCAPE

//cmake --build . --config Debug ; if ($?) { .\Debug\geoscape.exe } run this in the build folder!