//
// Created by mario on 13/07/2023.
//

#pragma once

// Variable global clearColor
Color clearColor(0, 0, 0);  // Color negro por defecto
Color currentColor(255, 255, 255);

// Variable global framebuffer
std::vector<Color> framebuffer;

struct Vertex2 {
    int x;
    int y;

    Vertex2 (int xPos, int yPos) {
        x = xPos;
        y = yPos;
    }
};

// Altura y Ancho
int screenWidth = 800;
int screenHeight = 600;

// Función clear
void clear() {
    for (auto& pixel : framebuffer) {
        pixel = clearColor;
    }
}

void drawPoint (const Vertex2& vertex) {
    int index = vertex.y * screenWidth + vertex.x;
    if (index >= 0 && index < framebuffer.size()) {
        framebuffer[index] = currentColor;
    }
}

// Función renderBuffer
// Función renderBuffer
void renderBuffer() {
    std::ofstream file("out.bmp", std::ios::binary);
    if (!file) {
        std::cout << "Error al abrir el archivo out.bmp" << std::endl;
        return;
    }

    int width = screenWidth;
    int height = screenHeight;
    int imageSize = width * height * 3;
    int fileSize = 54 + imageSize;
    int bitsPerPixel = 24;  // Bits por píxel (3 bytes: RGB)

    const int unused = 0;
    const int pixelDataOffset = 54;
    const int headerSize = 40;
    const short planes = 1;
    const int compression = 0;
    const int horizontalRes = 2835;
    const int verticalRes = 2835;
    const int colors = 0;
    const int importantColors = 0;

    // Encabezado del archivo BMP
    file << "BM";                                        // Tipo de archivo (2 bytes)
    file.write(reinterpret_cast<const char*>(&fileSize), 4);         // Tamaño del archivo (4 bytes)
    file.write(reinterpret_cast<const char*>(&unused), 4);           // Datos reservados (4 bytes)
    file.write(reinterpret_cast<const char*>(&pixelDataOffset), 4);  // Offset de los datos de píxeles (4 bytes)
    file.write(reinterpret_cast<const char*>(&headerSize), 4);       // Tamaño de la cabecera (4 bytes)
    file.write(reinterpret_cast<const char*>(&width), 4);            // Ancho de la imagen (4 bytes)
    file.write(reinterpret_cast<const char*>(&height), 4);           // Alto de la imagen (4 bytes)
    file.write(reinterpret_cast<const char*>(&planes), 2);           // Planos (2 bytes)
    file.write(reinterpret_cast<const char*>(&bitsPerPixel), 2);     // Bits por píxel (2 bytes)
    file.write(reinterpret_cast<const char*>(&compression), 4);      // Compresión (4 bytes)
    file.write(reinterpret_cast<const char*>(&imageSize), 4);        // Tamaño de la imagen (4 bytes)
    file.write(reinterpret_cast<const char*>(&horizontalRes), 4);    // Resolución horizontal (4 bytes)
    file.write(reinterpret_cast<const char*>(&verticalRes), 4);      // Resolución vertical (4 bytes)
    file.write(reinterpret_cast<const char*>(&colors), 4);           // Número de colores (4 bytes)
    file.write(reinterpret_cast<const char*>(&importantColors), 4);  // Colores importantes (4 bytes)

    // Datos de píxeles
    for (const auto& color : framebuffer) {
        file.write(reinterpret_cast<const char*>(&color.b), 1);  // Componente azul
        file.write(reinterpret_cast<const char*>(&color.g), 1);  // Componente verde
        file.write(reinterpret_cast<const char*>(&color.r), 1);  // Componente rojo
    }

    file.close();
    std::cout << "Archivo out.bmp generado exitosamente." << std::endl;
}

// Función para dibujar una línea utilizando el algoritmo de Bresenham
void drawLine(const Vertex2& start, const Vertex2& end) {
    int x1 = start.x;
    int y1 = start.y;
    int x2 = end.x;
    int y2 = end.y;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int err = dx - dy;

    while (true) {
        Vertex2 currentPoint(x1, y1);
        drawPoint(currentPoint);

        if (x1 == x2 && y1 == y2) {
            break;
        }

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void drawPolygon(const std::vector<Vertex2>& points) {
    int numPoints = points.size();
    if (numPoints < 2) {
        return;  // No se puede dibujar un polígono con menos de 2 puntos
    }

    // Dibujar líneas entre los puntos
    for (int i = 0; i < numPoints - 1; ++i) {
        drawLine(points[i], points[i + 1]);
    }

    // Dibujar línea desde el último punto al primer punto
    drawLine(points[numPoints - 1], points[0]);
}

void fillPolygon(const std::vector<Vertex2>& vertices) {
    int numVertices = vertices.size();
    if (numVertices < 3) {
        return;  // No se puede rellenar un polígono con menos de 3 vértices
    }

    // Encontrar los límites superior e inferior del polígono
    int minY = vertices[0].y;
    int maxY = vertices[0].y;
    for (int i = 1; i < numVertices; ++i) {
        if (vertices[i].y < minY) {
            minY = vertices[i].y;
        }
        if (vertices[i].y > maxY) {
            maxY = vertices[i].y;
        }
    }

    // Iterar por cada línea horizontal dentro de los límites del polígono
    for (int y = minY; y <= maxY; ++y) {
        std::vector<int> intersectionPoints;

        // Encontrar los puntos de intersección entre la línea horizontal y los segmentos del polígono
        for (int i = 0; i < numVertices; ++i) {
            int j = (i + 1) % numVertices;
            int x1 = vertices[i].x;
            int y1 = vertices[i].y;
            int x2 = vertices[j].x;
            int y2 = vertices[j].y;

            if ((y1 <= y && y < y2) || (y2 <= y && y < y1)) {
                // Calcular el punto de intersección en la línea horizontal
                float x = static_cast<float>((y - y1) * (x2 - x1)) / (y2 - y1) + x1;
                intersectionPoints.push_back(static_cast<int>(x));
            }
        }

        // Ordenar los puntos de intersección de menor a mayor
        std::sort(intersectionPoints.begin(), intersectionPoints.end());

        // Rellenar el espacio entre los pares de puntos de intersección
        for (int i = 0; i < intersectionPoints.size(); i += 2) {
            int startX = intersectionPoints[i];
            int endX = intersectionPoints[i + 1];

            // Dibujar una línea horizontal en el framebuffer entre startX y endX
            for (int x = startX; x <= endX; ++x) {
                drawPoint(Vertex2(x, y));
            }
        }
    }
}

// Función render
void render() {
    clear();

    // Dibujar polígono (ejemplo)
//    std::vector<Vertex2> polygonPoints;
//    polygonPoints.push_back(Vertex2(100, 200));
//    polygonPoints.push_back(Vertex2(200, 300));
//    polygonPoints.push_back(Vertex2(300, 400));
//    polygonPoints.push_back(Vertex2(400, 300));
//    drawPolygon(polygonPoints);
//    fillPolygon(polygonPoints);

    std::vector<Vertex2> polygon1Points;
    polygon1Points.push_back(Vertex2(165, 380));
    polygon1Points.push_back(Vertex2(185, 360));
    polygon1Points.push_back(Vertex2(180, 330));
    polygon1Points.push_back(Vertex2(207, 345));
    polygon1Points.push_back(Vertex2(233, 330));
    polygon1Points.push_back(Vertex2(230, 360));
    polygon1Points.push_back(Vertex2(250, 380));
    polygon1Points.push_back(Vertex2(220, 385));
    polygon1Points.push_back(Vertex2(205, 410));
    polygon1Points.push_back(Vertex2(193, 383));
    currentColor = Color(255, 255, 0);  // Relleno azul
    fillPolygon(polygon1Points);

//    // Dibujar polígono 2
    std::vector<Vertex2> polygon2Points;
    polygon2Points.push_back(Vertex2(321, 335));
    polygon2Points.push_back(Vertex2(288, 286));
    polygon2Points.push_back(Vertex2(339, 251));
    polygon2Points.push_back(Vertex2(374, 302));
    currentColor = Color(255, 255, 255);  // Línea blanca
    drawPolygon(polygon2Points);
    currentColor = Color(0, 0, 255);  // Relleno azul
    fillPolygon(polygon2Points);

//    // Dibujar polígono 3
//    std::vector<Vertex2> polygon3Points;
//    polygon3Points.push_back(Vertex2(377, 249));
//    polygon3Points.push_back(Vertex2(411, 197));
//    polygon3Points.push_back(Vertex2(436, 249));
//    currentColor = Color(255, 255, 255);  // Línea blanca
//    drawPolygon(polygon3Points);
//    currentColor = Color(255, 0, 0);  // Relleno rojo
//    fillPolygon(polygon3Points);

    // Dibujar polígono 4
    std::vector<Vertex2> polygon4Points;
    polygon4Points.push_back(Vertex2(413, 177));
    polygon4Points.push_back(Vertex2(448, 159));
    polygon4Points.push_back(Vertex2(502, 88));
    polygon4Points.push_back(Vertex2(553, 53));
    polygon4Points.push_back(Vertex2(535, 36));
    polygon4Points.push_back(Vertex2(676, 37));
    polygon4Points.push_back(Vertex2(660, 52));
    polygon4Points.push_back(Vertex2(750, 145));
    polygon4Points.push_back(Vertex2(761, 179));
    polygon4Points.push_back(Vertex2(672, 192));
    polygon4Points.push_back(Vertex2(659, 214));
    polygon4Points.push_back(Vertex2(615, 214));
    polygon4Points.push_back(Vertex2(632, 230));
    polygon4Points.push_back(Vertex2(580, 230));
    polygon4Points.push_back(Vertex2(597, 215));
    polygon4Points.push_back(Vertex2(552, 214));
    polygon4Points.push_back(Vertex2(517, 144));
    polygon4Points.push_back(Vertex2(466, 180));
    currentColor = Color(255, 255, 255);  // Línea blanca
    drawPolygon(polygon4Points);
    currentColor = Color(0, 255, 0);  // Relleno verde
    fillPolygon(polygon4Points);

    // Dibujar polígono 5
    std::vector<Vertex2> polygon5Points;
    polygon5Points.push_back(Vertex2(682, 175));
    polygon5Points.push_back(Vertex2(708, 120));
    polygon5Points.push_back(Vertex2(735, 148));
    polygon5Points.push_back(Vertex2(739, 170));
    currentColor = Color(255, 255, 255);  // Línea blanca
    drawPolygon(polygon5Points);
    currentColor = Color(0, 0, 0);  // Relleno negro
    fillPolygon(polygon5Points);

    renderBuffer();
}
