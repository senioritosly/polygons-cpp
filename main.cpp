#include <iostream>
#include <fstream>
#include <vector>
#include "color.h"
#include "framebuffer.h"

int main() {
    framebuffer.assign(screenWidth * screenHeight, clearColor);

    render();

    return 0;
}
