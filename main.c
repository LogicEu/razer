#include <photon.h>
#include <imgtool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    uint8_t r, g, b;
    r = g = b = 155;
    if (argc > 1) r = atoi(argv[1]);
    if (argc > 2) g = atoi(argv[2]);
    if (argc > 3) b = atoi(argv[3]);
    uint8_t color[] = {r, g, b, 255};
    uint8_t blue[] = {0, 0, 255, 255};
    uint8_t red[] = {255, 0, 0, 255};

    char path[256] = "output.png", open[256];
    ivec2 res = ivec2_new(800, 600);
    int channels = 4;

    bmp_t bmp = bmp_color(res.x, res.y, channels, &color[0]);
    Circle c = circle_new(vec2_new(res.x / 2, res.y / 2), 10.0f);
    Tri2D t = tri2D_new(
        vec2_new(300, 200),
        vec2_new(500, 200),
        vec2_new(400, 500)
    );

    for (unsigned int y = 0; y < bmp.height; y++) {
        for (unsigned int x = 0; x < bmp.width; x++) {
            vec2 p = {x, y};
            if (tri2D_point_overlap(&t, p)) memcpy(px_at(&bmp, x, y), &blue[0], bmp.channels);
            if (circle_point_overlap(&c, p)) memcpy(px_at(&bmp, x, y), &red[0], bmp.channels);
        }
    }
    
    bmp_write(path, &bmp);
    bmp_free(&bmp);

#ifdef __APPLE__
    strcpy(open, "open ");
#else
    strcpy(open, "xdg-open ");
#endif
    strcat(open, path);
    system(open);
    return 0;
}
