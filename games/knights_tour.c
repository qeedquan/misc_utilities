/* Solves the knight tours using backtracking */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>

#if __STDC_VERSION__ < 199901L
 #define restrict
 #define inline
#endif

#if defined(__GNUC__) || defined(__clang__)
#define USE_BUILTIN 1
#endif

#define nelem(x) (sizeof(x)/sizeof(x[0]))

/* defines */
#define BOARD 0xFFFFFFFFFFFFFFFFULL
#define SQ(X)  (1ULL << (X))
#define AFILE  0x0101010101010101ULL
#define BFILE  0x0202020202020202ULL
#define CFILE  0x0404040404040404ULL
#define DFILE  0x0808080808080808ULL
#define EFILE  0x1010101010101010ULL
#define FFILE  0x2020202020202020ULL
#define GFILE  0x4040404040404040ULL
#define HFILE  0x8080808080808080ULL

typedef unsigned int uint;
typedef uint64_t     board_t;

/* the struct in which we save our path that
 * the knight take to solve the tour */
typedef struct
{
    uint moves[64];
    uint nmoves;
} Solution;

/* the struct that holds everything */
typedef struct
{
    uint     nsolutions, pos;
    Solution cursolution;
    board_t  board, solved;
} KT;

/* data */
static board_t knight_moves[64];
static int     knight_popcount[64];

/* functions prototypes */
static void   init           (void);
static void   setup          (KT* restrict, const char*);
static size_t gen_moves      (KT* restrict, uint* restrict);
static void   solve          (KT* restrict);
static int    lsb            (board_t);
static void   make_move      (KT* restrict, uint);
static void   undo_move      (KT* restrict, uint);
static void   print_board    (KT * restrict, int);
static void   print_solution (Solution* restrict);
static int    popcount       (board_t);

int main(int argc, char *argv[])
{
    KT kt;
    time_t start;

    if (argc < 2 || (argv[1][0] < 'a' || argv[1][0] > 'h') ||
            (argv[1][1] <= '0' || argv[1][1] >= '9'))
    {
        printf("Usage: <square to solve>\n");
        return 1;
    }

    init();
    setup(&kt, argv[1]);
    printf("Solving\n");
    print_board(&kt, 1);
    start = time(NULL);
    solve(&kt);
    printf("Took %ld seconds.\n", time(NULL) - start);
    return 0;
}

/* return lsb of a bit, used for extracting move sequences
 * from the bitboard */
static int lsb(board_t v)
{
#if USE_BUILTIN
	return __builtin_ffsl(v) - 1;
#else

    static const uint8_t table[] =
    {
        0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    };
    uint i, s = 0;
    board_t b = v;

    do
    {
        i = b & 0xFF;

        if (i)
            return table[i] + s;

        b >>= 8;
        s += 8;
    }
    while (b);

    return 0;

#endif

}

static int popcount(board_t v)
{
#if USE_BUILTIN
	return __builtin_popcountl(v);
#else
    int ret = 0;
    board_t b = v;

    while (b)
    {
        b &= (b - 1);
        ret++;
    }

    return ret;
#endif
}

/* init stuff */
static void init(void)
{
    int i;

    /* pre-compute the knight moves
     * so we can generate knight moves quickly */
    for (i = 0; i < 64; i++)
    {
        knight_moves[i] = ((SQ(i) << 17) & ~(AFILE)) |
                          ((SQ(i) << 15) & ~(HFILE)) |
                          ((SQ(i) << 10) & ~(AFILE | BFILE)) |
                          ((SQ(i) << 6) & ~(GFILE | HFILE)) |
                          ((SQ(i) >> 17) & ~(HFILE)) |
                          ((SQ(i) >> 10) & ~(GFILE | HFILE)) |
                          ((SQ(i) >> 6) & ~(AFILE | BFILE)) |
                          ((SQ(i) >> 15) & ~(AFILE));

        knight_popcount[i] = popcount(knight_moves[i]);
    }
}

/* reset the board to default state */
static void setup(KT * restrict kt, const char *startpos)
{
    uint sq;

    sq = ((startpos[1] - '1') * 8) + (tolower(startpos[0]) - 'a');

    kt->nsolutions = 0;
    kt->pos = sq;
    kt->solved = SQ(sq);
    kt->board = SQ(sq);

    kt->cursolution.moves[0] = sq;
    kt->cursolution.nmoves = 1;
}

/* print out the solved board or the current board */
static void print_board(KT * restrict kt, int solved)
{
    board_t b = (solved) ? kt->solved : kt->board;
    board_t sq;
    int i, k;

    printf("\n");
    for (i = 56; i >= 0; i -= 8)
    {
        for (k = 0; k < 8; k++)
        {
            sq = SQ(i + k);
            if (b & sq)
                printf("N ");
            else
                printf(". ");
        }
        printf("\n");
    }
    printf("\n");
}

/* print out a solution */
static void print_solution(Solution* restrict s)
{
    uint sq;
    size_t i;
    for (i = 0; i < s->nmoves; i++)
    {
        sq = s->moves[i] & 0x3F;
        printf("Move %zu: N%c%d\n", i, (sq & 7) + 'a', (sq / 8) + 1);
    }
    printf("\n");
}

/* make a move */
static void make_move(KT* restrict kt, uint move)
{
    move &= 0x3F;
    kt->pos = move;
    kt->board = SQ(move);
    kt->solved |= SQ(move);
}

/* undo a move */
static void undo_move(KT* restrict kt, uint move)
{
    uint oldpos = move >> 8;
    kt->pos = oldpos;
    kt->board = SQ(oldpos);
    kt->solved ^= SQ(move & 0x3F);
}

/* generate a move */
static size_t gen_moves(KT* restrict kt, uint * restrict moves)
{
    int popcnt[16] = { -1 };
    size_t i = 0, k;
    board_t b = knight_moves[kt->pos] & ~kt->solved, sq;
    uint temp, oldpos = kt->pos;

    while (b)
    {
        sq = lsb(b);

        moves[i] = sq | (oldpos << 8);
        popcnt[i] = knight_popcount[sq];

        /* heuristic where moves are
         * sorted by least degrees of freedom */
        for (k = i; k > 0; k--)
        {
            if (popcnt[k] < popcnt[k - 1])
            {
                temp = moves[k];
                moves[k] = moves[k - 1];
                moves[k - 1] = temp;
                continue;
            }

            break;
        }

        i++;
        b &= (b - 1);
    }

    return i;
}

/* solve it using backtracking */
static void solve(KT* restrict kt)
{
    size_t i, nmoves;
    uint moves[16];

    if (kt->solved == BOARD)
    {
        printf("Solution #%d\n", ++kt->nsolutions);
        print_solution(&kt->cursolution);
        return;
    }

    nmoves = gen_moves(kt, moves);

    for (i = 0; i < nmoves; i++)
    {
        kt->cursolution.moves[kt->cursolution.nmoves++] = moves[i];
        make_move(kt, moves[i]);

        solve(kt);

        kt->cursolution.nmoves--;
        undo_move(kt, moves[i]);
    }
}
