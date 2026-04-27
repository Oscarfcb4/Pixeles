#pragma once

// Los tipos especificos del motor Inazuma

// Valores de teclado

#define K_SPACE          32   //   //
#define K_APOSTROPHE     39   // ' // 
#define K_COMMA          44   // , // 
#define K_MINUS          45   // - // 
#define K_DOT            46   // . // 
#define K_SLASH          47   // / // 
#define K_0              48 
#define K_1              49 
#define K_2              50 
#define K_3              51 
#define K_4              52 
#define K_5              53 
#define K_6              54 
#define K_7              55 
#define K_8              56 
#define K_9              57 
#define K_SEMICOLON      59   // ; // 
#define K_EQUAL          61   // = // 
#define K_A              65 
#define K_B              66 
#define K_C              67 
#define K_D              68 
#define K_E              69 
#define K_F              70 
#define K_G              71 
#define K_H              72 
#define K_I              73 
#define K_J              74 
#define K_K              75 
#define K_L              76 
#define K_M              77 
#define K_N              78 
#define K_O              79 
#define K_P              80 
#define K_Q              81 
#define K_R              82 
#define K_S              83 
#define K_T              84 
#define K_U              85 
#define K_V              86 
#define K_W              87 
#define K_X              88 
#define K_Y              89 
#define K_Z              90 
#define K_LEFT_BRACKET   91   // [ // 
#define K_BACKSLASH      92   // \ // 
#define K_RIGHT_BRACKET  93   // ] // 
#define K_GRAVE_ACCENT   96   // ` // 
#define K_ESCAPE         256 
#define K_ENTER          257 
#define K_TAB            258 
#define K_BACKSPACE      259 
#define K_INSERT         260 
#define K_DELETE         261 
#define K_ARROW_RIGHT    262 
#define K_ARROW_LEFT     263 
#define K_ARROW_DOWN     264 
#define K_ARROW_UP       265 
#define K_PAGE_UP        266 
#define K_PAGE_DOWN      267 
#define K_HOME           268 
#define K_END            269 
#define K_CAPS_LOCK      280 
#define K_SCROLL_LOCK    281 
#define K_NUM_LOCK       282 
#define K_PRINT_SCREEN   283 
#define K_PAUSE          284 
#define K_F1             290 
#define K_F2             291 
#define K_F3             292 
#define K_F4             293 
#define K_F5             294 
#define K_F6             295 
#define K_F7             296 
#define K_F8             297 
#define K_F9             298 
#define K_F10            299 
#define K_F11            300 
#define K_F12            301 
#define K_F13            302 
#define K_F14            303 
#define K_F15            304 
#define K_F16            305 
#define K_F17            306 
#define K_F18            307 
#define K_F19            308 
#define K_F20            309 
#define K_F21            310 
#define K_F22            311 
#define K_F23            312 
#define K_F24            313 
#define K_F25            314 
#define K_KP_0           320 
#define K_KP_1           321 
#define K_KP_2           322 
#define K_KP_3           323 
#define K_KP_4           324 
#define K_KP_5           325 
#define K_KP_6           326 
#define K_KP_7           327 
#define K_KP_8           328 
#define K_KP_9           329 
#define K_KP_DECIMAL     330 
#define K_KP_DIVIDE      331 
#define K_KP_MULTIPLY    332 
#define K_KP_SUBTRACT    333 
#define K_KP_ADD         334 
#define K_KP_ENTER       335 
#define K_KP_EQUAL       336 
#define K_LEFT_SHIFT     340 
#define K_LEFT_CONTROL   341 
#define K_LEFT_ALT       342 
#define K_LEFT_SUPER     343 
#define K_RIGHT_SHIFT    344 
#define K_RIGHT_CONTROL  345 
#define K_RIGHT_ALT      346 
#define K_RIGHT_SUPER    347 
#define K_MENU           348 

// Clase color

typedef struct _Color {
    float r{}, g{}, b{}, a{};

    bool operator==(const _Color& c) const {
        return (
            r == c.r && 
            g == c.g && 
            b == c.b && 
            a == c.a
        );
    }
    
} _Color;

// Colores por defecto
#define LIGHTGRAY  _Color(200, 200, 200, 1.0f)
#define GRAY       _Color(130, 130, 130, 1.0f)
#define DARKGRAY   _Color( 80,  80,  80, 1.0f)
#define YELLOW     _Color(253, 249,   0, 1.0f)
#define GOLD       _Color(255, 203,   0, 1.0f)
#define ORANGE     _Color(255, 161,   0, 1.0f)
#define PINK       _Color(255, 109, 194, 1.0f)
#define RED        _Color(230,  41,  55, 1.0f)
#define MAROON     _Color(190,  33,  55, 1.0f)
#define GREEN      _Color(  0, 228,  48, 1.0f)
#define LIME       _Color(  0, 158,  47, 1.0f)
#define DARKGREEN  _Color(  0, 117,  44, 1.0f)
#define SKYBLUE    _Color(102, 191, 255, 1.0f)
#define BLUE       _Color(  0, 121, 241, 1.0f)
#define DARKBLUE   _Color(  0,  82, 172, 1.0f)
#define PURPLE     _Color(200, 122, 255, 1.0f)
#define VIOLET     _Color(135,  60, 190, 1.0f)
#define DARKPURPLE _Color(112,  31, 126, 1.0f)
#define BEIGE      _Color(211, 176, 131, 1.0f)
#define BROWN      _Color(127, 106,  79, 1.0f)
#define DARKBROWN  _Color( 76,  63,  47, 1.0f)
#define WHITE      _Color(255, 255, 255, 1.0f)
#define TRANS      _Color(  0,   0,   0, 1.0f)
#define BLANK      _Color(  0,   0,   0, 1.0f)
#define MAGENTA    _Color(255,   0, 255, 1.0f)
#define NICEWHITE  _Color(220, 220, 220, 1.0f)

// Vectores con unos operadores definidos

typedef struct Vec2 {
    float x, y;
    bool operator==(const Vec2& v) const {
        return (
            x == v.x && 
            y == v.y 
        );
    }
    bool operator!=(const Vec2& v) const {
        return (
            x != v.x || 
            y != v.y 
        );
    }
    Vec2 operator+(const Vec2& v) const {
        return Vec2(
            x + v.x,
            y + v.y
        );
    }
    Vec2 operator-(const Vec2& v) const {
        return Vec2(
            x - v.x,
            y - v.y
        );
    }
    Vec2 operator*(const float& f) const {
        return Vec2(
            x * f,
            y * f
        );
    }
    Vec2 operator/(const float& f) const {
        return Vec2(
            x / f,
            y / f
        );
    }
    Vec2 operator+=(const Vec2& v) {
        return Vec2(
            x += v.x,
            y += v.y
        );
    }
    Vec2 operator-=(const Vec2& v) {
        return Vec2(
            x -= v.x,
            y -= v.y
        );
    }
    Vec2 operator=(const glm::vec2& v) {
        return Vec2(
            x -= v.x,
            y -= v.y
        );
    }
} Vec2;

typedef struct Vec3 {
    float x, y, z;
    bool operator==(const Vec3& v) const {
        return (
            x == v.x && 
            y == v.y && 
            z == v.z 
        );
    }
    bool operator!=(const Vec3& v) const {
        return (
            x != v.x || 
            y != v.y || 
            z != v.z 
        );
    }Vec3 operator+(const Vec3& v) const {
        return Vec3(
            x + v.x,
            y + v.y, 
            z + v.z
        );
    }
    Vec3 operator-(const Vec3& v) const {
        return Vec3(
            x - v.x,
            y - v.y, 
            z - v.z
        );
    }
    Vec3 operator*(const float& f) const {
        return Vec3(
            x * f,
            y * f, 
            z * f
        );
    }
    Vec3 operator/(const float& f) const {
        return Vec3(
            x / f,
            y / f, 
            z / f
        );
    }
    Vec3 operator+=(const Vec3& v) {
        return Vec3(
            x += v.x,
            y += v.y, 
            z += v.z
        );
    }
    Vec3 operator-=(const Vec3& v) {
        return Vec3(
            x -= v.x,
            y -= v.y, 
            z -= v.z
        );
    }
    Vec3 operator=(const glm::vec3& v) {
        return Vec3(
            x -= v.x,
            y -= v.y, 
            z -= v.z
        );
    }
} Vec3;

typedef struct Vec4 {
    float x, y, z, w;
    bool operator==(const Vec4& v) const {
        return (
            x == v.x && 
            y == v.y && 
            z == v.z &&
            w == v.w
        );
    }
    bool operator!=(const Vec4& v) const {
        return (
            x != v.x || 
            y != v.y || 
            z != v.z ||
            w != v.w
        );
    }
    Vec4 operator+(const Vec4& v) const {
        return Vec4(
            x + v.x,
            y + v.y, 
            z + v.z,
            w + v.w
        );
    }
    Vec4 operator-(const Vec4& v) const {
        return Vec4(
            x - v.x,
            y - v.y, 
            z - v.z,
            w - v.w
        );
    }
    Vec4 operator*(const float& f) const {
        return Vec4(
            x * f,
            y * f, 
            z * f,
            w * f
        );
    }
    Vec4 operator/(const float& f) const {
        return Vec4(
            x / f,
            y / f, 
            z / f,
            w / f
        );
    }
    Vec4 operator+=(const Vec4& v) {
        return Vec4(
            x += v.x,
            y += v.y, 
            z += v.z,
            w += v.w
        );
    }
    Vec4 operator-=(const Vec4& v) {
        return Vec4(
            x -= v.x,
            y -= v.y, 
            z -= v.z,
            w -= v.w
        );
    }
    Vec4 operator=(const glm::vec4& v) {
        return Vec4(
            x -= v.x,
            y -= v.y, 
            z -= v.z,
            w -= v.w
        );
    }
} Vec4;

typedef struct _Rectangle {
    float x{}, y{}, width{}, height{};
} _Rectangle;

// De momento estas clases las tengo como compatibilidad futura con raylib, de moemnto no las hago uso. A futuro 
// les implementare la utilidad.

// BoundingBoxes

typedef struct _BoundingBox {
    Vec3 min{}, max{};
} _BoundingBox;

// Camaras

typedef struct _Camera {
    Vec3 position{}, target{}, up{};
    float fovy{};
    int projection{};
} _Camera;

// Raycasting

typedef struct _Ray {
    Vec3 position{}, direction{};
} _Ray;

typedef struct _RayCollision {
    Vec3 point{}, normal{};
    float distance{};        
    bool hit{};     
} _RayCollision;

// Texturas

enum TextureTypes{
    difuse, specular
};

typedef struct _Texture2D{
    unsigned int id{};
    int width{}, height{}, nComp{};
    std::string type{};
} _Texture2D;

static void replaceString(std::string& string, const std::string& replace, char ch) {
  std::size_t found = string.find_first_of(replace);

  while(found != std::string::npos){
    string[found] = ch;
    found = string.find_first_of(replace, found+1);
  }
}

struct InputStates{
    int lastState{}, actualState{};
};