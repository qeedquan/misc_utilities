// recursion trees, implemented using the tutorial of http://lodev.org/cgtutor/recursiontrees.html
// g++ -Wall -pedantic -std=c++14 -o recursion-trees recursion-trees.cpp `pkg-config --cflags --libs sdl2`

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <cstdint>
#include <iostream>
#include <unistd.h>
#include <SDL.h>

using namespace std;

[[ noreturn ]] void fatal(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}

template <typename T>
struct vec2_t
{
    T x, y;

    vec2_t() : x(0), y(0) {}
    vec2_t(T x, T y) : x(x), y(y) {}
};

typedef vec2_t<double> vec2;
typedef vec2_t<int> vec2i;

struct Tree
{
    int max_depth;
    double angle;
    double shrink;
    bool freeze;

    Tree(int max_depth, double angle, double shrink) 
        : max_depth(max_depth), angle(angle), shrink(shrink), freeze(false) {}

    void update_and_render(SDL_Renderer *renderer, float dt)
    {
        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);

        recursion(renderer, vec2(w/2, h-1), vec2(0, -1), h/2.3, 0);
        if (!freeze)
            angle += 1.5*dt;
    }

    void recursion(SDL_Renderer *renderer, vec2 pos, vec2 dir, double size, int n)
    {
        vec2i p(pos.x, pos.y);
        vec2i q(pos.x + size*dir.x, pos.y + size*dir.y);

        SDL_SetRenderDrawColor(renderer, 128, 96, 64, 255);
        SDL_RenderDrawLine(renderer, p.x, p.y, q.x, q.y);
       
        if (n >= max_depth)
            return;

        vec2 pos2(pos.x + size*dir.x, pos.y + size*dir.y);
        auto size2 = size / shrink;
        auto n2 = n + 1;

        auto ci = cos(angle);
        auto si = sin(angle);
        vec2 dir2(
            ci * dir.x  + si * dir.y,
           -si * dir.x  + ci * dir.y
        );
        recursion(renderer, pos2, dir2, size2, n2);

        si = -si;
        dir2 = vec2(
            ci * dir.x + si * dir.y,
           -si * dir.x + ci * dir.y
        );
        recursion(renderer, pos2, dir2, size2, n2);
    }
};

struct Options
{
    static constexpr auto default_width = 800;
    static constexpr auto default_height = 600;
    static constexpr auto default_max_depth = 8;
    static constexpr auto default_angle = 0.2 * M_PI;
    static constexpr auto default_shrink = 1.8;

    int width;
    int height;

    int max_depth;
    double angle;
    double shrink;

    Options(int argc, char *argv[])
    {
        width = default_width;
        height = default_height;
        max_depth = default_max_depth;
        angle = default_angle;
        shrink = default_shrink;
        parse(argc, argv);
    }

    void parse(int argc, char *argv[])
    {
        int opt;
        while ((opt = getopt(argc, argv, "w:h:d:a:s:")) != -1)
        {
            switch (opt)
            {
                case 'w':
                    width = atoi(optarg);
                    break;
                
                case 'h':
                    height = atoi(optarg);
                    break;

                case 'd':
                    max_depth = atoi(optarg);
                    break;

                case 'a':
                    angle = atof(optarg);
                    break;

                case 's':
                    shrink = atof(optarg);
                    break;

                default:
                    fprintf(stderr, "usage:\n");
                    fprintf(stderr, "\t-w width default=%d\n", default_width);
                    fprintf(stderr, "\t-h height default=%d\n", default_height);
                    fprintf(stderr, "\t-d max depth=%d\n", default_max_depth);
                    fprintf(stderr, "\t-a angle=%lf\n", default_angle);
                    fprintf(stderr, "\t-s shrink=%lf\n", default_shrink);
                    exit(1);
            }

            auto check = [](auto &x, auto def, const auto &what)
            {
                if (x <= 0)
                {
                    cout << "invalid value for " << what << ", using default of " << def << endl;
                    x = def;
                }
            };

            check(width, default_width, "width");
            check(height, default_height, "height");
            check(max_depth, default_max_depth, "max depth");
            check(angle, default_angle, "angle");
            check(shrink, default_shrink, "shrink");
        }
    }
};

int main(int argc, char *argv[])
{
    Options opt(argc, argv);

    SDL_Window *window;
    SDL_Renderer *renderer;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
        fatal("sdl: %s", SDL_GetError());

    if (SDL_CreateWindowAndRenderer(opt.width, opt.height, SDL_WINDOW_RESIZABLE, &window, &renderer) < 0)
        fatal("sdl: %s", SDL_GetError());

    Tree tree(opt.max_depth, opt.angle, opt.shrink);

    SDL_SetWindowTitle(window, "Recursion Trees");

    auto then = SDL_GetTicks();
    for (;;)
    {
        auto now = SDL_GetTicks();
        auto tick = now - then;
        then = now;

        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
                case SDL_QUIT:
                    exit(0);

                case SDL_KEYDOWN:
                    switch (ev.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            exit(0);

                        case SDLK_SPACE:
                            tree.freeze = !tree.freeze;
                            break;
                    }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        auto dt = ((float)tick / 1e2f) * 0.1;
        tree.update_and_render(renderer, dt);
        SDL_RenderPresent(renderer);

        if (tick < 1000/60)
            SDL_Delay(1000/60 - tick);
    }

    return 0;
}
