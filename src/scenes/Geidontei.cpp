#include "Geidontei.hpp"
#include "psyqo/font.hh"

mi::Scenes::Geidontei::Geidontei(psyqo::Font<> textFont) {
    this->_font = textFont;
}

void mi::Scenes::Geidontei::frame() {
    if (_anim == 0) {
        _direction = true;
    } else if (_anim == 255) {
        _direction = false;
    }
    
    psyqo::Color bg {{
        .r = _anim, 
        .g = 64, 
        .b = 91
    }};
    
    gpu().clear(bg);
    
    if (_direction) {
        _anim++;
    } else {
        _anim--;
    }

    psyqo::Color c = {{
        .r = 255, 
        .g = 255, 
        .b = uint8_t(255 - _anim)
    }};

    this->_font.print(gpu(), "Hello World!", {{.x = 16, .y = 32}}, c);
}