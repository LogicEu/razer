#include <photon.h>
#include <mass.h>
#include <imgtool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static array_t* mesh;
static uint8_t blue[] = {0, 0, 255, 255};
static uint8_t red[] = {0, 255, 0, 255};
float n;

static inline uint8_t ulerp(uint8_t a, uint8_t b, float t)
{
    return (int)lerpf((float)a, (float)b, t);
}

void px_circle(bmp_t* bmp, Circle c)
{
    uint8_t* px = bmp->pixels;
    for (int y = 0; y < bmp->height; y++) {
        for (int x = 0; x < bmp->width; x++) {
            vec2 p = {x, y};
            bool hit = circle_point_overlap(c, p);
            *px = *px * !hit + 255 * hit;
            px += bmp->channels;
        }
    }

}

void px_line(bmp_t* bmp, vec2 p1, vec2 p2)
{
    vec2 d = vec2_sub(p2, p1);
    float length = vec2_mag(d);
    float rad = vec2_to_rad(d);
    float dx = cos(rad), dy = sin(rad);

    for (int i = 0; i < (int)length; i++) {
        p1.x += dx;
        p1.y += dy;
        int x = (int)p1.x, y = (int)p1.y;
        if (x < 0 || y < 0 || x > bmp->width || y > bmp->height) continue;
        memcpy(px_at(bmp, x, y), &red[0], bmp->channels);
    }
}

void px_raster(bmp_t* bmp, unsigned int x, unsigned int y, int samples)
{       
    int overlap = 0;
    Tri2D* t = mesh->data;
    for (Tri2D* end = t + mesh->used; t != end; t++) {
        for (int sy = 0; sy < samples; sy++) {
            for (int sx = 0; sx < samples; sx++) {
                vec2 p = {(float)x + (float)sx * n, (float)y + (float)sy * n};
                overlap += tri2D_point_overlap(t, p);
            }
        }
    }

    uint8_t* px = px_at(bmp, x, y);
    for (int i = 0; i < 4; i++) {
        px[i] = ulerp(px[i], blue[i], (float)overlap * (n / (float)samples));
    }
}

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
    int channels = 4, samples = 4;
    n = 1.0 / (float)samples;

    bmp_t bmp = bmp_color(res.x, res.y, channels, &color[0]);
    Tri2D t = tri2D_new(
        vec2_new(300, 200),
        vec2_new(500, 200),
        vec2_new(400, 500)
    );
    mesh = array_new(10, sizeof(Tri2D));
    array_push(mesh, &t);

    for (unsigned int y = 0; y < bmp.height; y++) {
        for (unsigned int x = 0; x < bmp.width; x++) {
            px_raster(&bmp, x, y, samples);
        }
    }
     
    vec2 pa = vec2_new(0, 0), pb = vec2_new(500, 300), p;
    px_line(&bmp, pa, pb);
    if (line2D_intersect(pa, pb, t.a, t.b, &p)) {
        px_circle(&bmp, circle_new(p, 10.0));
    }
    if (line2D_intersect(pa, pb, t.a, t.c, &p)) {
        px_circle(&bmp, circle_new(p, 10.0));
    }
    if (line2D_intersect(pa, pb, t.b, t.c, &p)) {
        px_circle(&bmp, circle_new(p, 10.0));
    }

    Ray2D ray = ray2D_new(vec2_new(600, 400), vec2_norm(vec2_sub(vec2_new(-600, -400), p)));
    Hit2D hit;
    px_circle(&bmp, circle_new(ray.orig, 10.0));
    if (tri2D_hit(&t, &ray, &hit)) {   
        vec2 P = ray2D_at(&ray, hit.t);
        px_circle(&bmp, circle_new(P, 10.0));
        px_line(&bmp, ray.orig, P);
        px_line(&bmp, P, vec2_add(P, vec2_mult(hit.normal, 20.0)));
        px_line(&bmp, P, vec2_add(P, vec2_mult(vec2_reflect(ray.dir, hit.normal), 40.0)));
    }
    px_line(&bmp, t.a, t.b);
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
