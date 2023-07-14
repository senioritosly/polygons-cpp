//
// Created by mario on 13/07/2023.
//

#pragma once

class Color {
public:
    unsigned char r;  // Valor de rojo (0-255)
    unsigned char g;  // Valor de verde (0-255)
    unsigned char b;  // Valor de azul (0-255)

    Color(unsigned char red, unsigned char green, unsigned char blue) {
        r = red;
        g = green;
        b = blue;
    }
};