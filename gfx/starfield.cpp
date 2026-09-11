// a classic starfield effect, based on benny starfield code to try out some c++14 features
// g++ -Wall -pedantic -std=c++14 -o starfield starfield.cpp `pkg-config --cflags --libs sdl2`

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <cstdint>
#include <unistd.h>
#include <iostream>
#include <random>
#include <vector>
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
struct vec3_t
{
    T x, y, z;

    vec3_t() : x(0), y(0), z(0) {}
    vec3_t(T x, T y, T z) : x(x), y(y), z(z) {}
};

typedef vec3_t<float> vec3;

struct rgb
{
    uint8_t r, g, b;
};

struct Starfield
{
private:
    void init_star(size_t i)
    {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(0, 1);
        
        stars[i].x = 2 * (dis(gen) - 0.5f) * spread;
        stars[i].y = 2 * (dis(gen) - 0.5f) * spread;
        stars[i].z = 2 * (dis(gen) - 0.5f) * spread;

        colors[i].r = rand() & 0xff;
        colors[i].g = rand() & 0xff;
        colors[i].b = rand() & 0xff;
    }

public:
    float spread;
    float speed;
    vector<vec3> stars;
    vector<rgb> colors;

    Starfield(float spread, float speed, size_t numstars) 
        : spread(spread), speed(speed)
    {
        stars.resize(numstars);
        colors.resize(numstars);
        for (size_t i = 0; i < stars.size(); i++)
            init_star(i);
    }

    void update_and_render(SDL_Renderer *renderer, float dt)
    {
        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);
        
        auto hw = w / 2.0f;
        auto hh = h / 2.0f;
        for (size_t i = 0; i < stars.size(); i++)
        {
            stars[i].z -= dt * speed;
            if (stars[i].z <= 0)
                init_star(i);

            int x = stars[i].x/stars[i].z * hw + hw;
            int y = stars[i].y/stars[i].z * hh + hh;

            if (x < 0 || x >= w || y < 0 || y >= h)
                init_star(i);
            else
            {
                SDL_SetRenderDrawColor(renderer, colors[i].r, colors[i].g, colors[i].b, 255);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }
};

struct Options
{
    static constexpr auto default_width = 800;
    static constexpr auto default_height = 600;
    static constexpr auto default_stars = 2048;
    static constexpr auto default_spread = 64.0f;
    static constexpr auto default_speed = 20.0f;

    int width;
    int height;

    float spread;
    float speed;
    
    size_t numstars;

    Options(int argc, char *argv[])
    {
        width = default_width;
        height = default_height;
        numstars = default_stars;
        spread = default_spread;
        speed = default_speed;
        parse(argc, argv);
    }

    void parse(int argc, char *argv[])
    {
        int opt;
        while ((opt = getopt(argc, argv, "w:h:n:s:p:")) != -1)
        {
            switch (opt)
            {
                case 'w':
                    width = atoi(optarg);
                    break;
                
                case 'h':
                    height = atoi(optarg);
                    break;

                case 'n':
                    numstars = atol(optarg);
                    break;

                case 's':
                    speed = atof(optarg);
                    break;

                case 'p':
                    spread = atof(optarg);
                    break;

                default:
                    fprintf(stderr, "usage:\n");
                    fprintf(stderr, "\t-w width default=%d\n", default_width);
                    fprintf(stderr, "\t-h height default=%d\n", default_height);
                    fprintf(stderr, "\t-n number of stars=%d\n", default_stars);
                    fprintf(stderr, "\t-s speed=%f\n", default_speed);
                    fprintf(stderr, "\t-p spread=%f\n", default_spread);
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
            check(numstars, default_stars, "stars");
            check(speed, default_speed, "speed");
            check(spread, default_spread, "spread");
        }
    }
};

int main(int argc, char *argv[])
{
    srand(time(NULL));
    Options opt(argc, argv);

    SDL_Window *window;
    SDL_Renderer *renderer;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
        fatal("sdl: %s", SDL_GetError());

    if (SDL_CreateWindowAndRenderer(opt.width, opt.height, SDL_WINDOW_RESIZABLE, &window, &renderer) < 0)
        fatal("sdl: %s", SDL_GetError());

    Starfield starfield(opt.spread, opt.speed, opt.numstars);

    SDL_SetWindowTitle(window, "Starfield");

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

                        case SDLK_LEFT:
                            starfield.speed = max(0.0f, starfield.speed - 5.0f);
                            break;

                        case SDLK_RIGHT:
                            starfield.speed += 5.0f;
                            break;
                    }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        auto dt = ((float)tick / 1e2f) * 0.1;
        starfield.update_and_render(renderer, dt);
        SDL_RenderPresent(renderer);

        if (tick < 1000/60)
            SDL_Delay(1000/60 - tick);
    }

    return 0;
}
