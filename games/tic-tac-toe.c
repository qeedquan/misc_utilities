/* tic tac toe using negamax, should play
 * perfectly */

#include <stdio.h>
#include <string.h>

typedef unsigned int uint;

#define nelem(x) (sizeof(x)/sizeof(x[0]))

enum
{
    X = 0, O = 1, oo = 1
};

typedef struct
{
    uint pv[9];
    uint side[2];
    uint player;
    uint human_turn;
} board_t;

/* gen moves */
static uint gen_moves(board_t *b, uint *moves, uint ptm);
static void make_move(board_t *b, uint move, uint *undo, uint ptm);
static void undo_move(board_t *b, uint move, uint ptm);

/* eval and search */

static int eval(board_t *b, uint ptm);
static int search(board_t *b, int depth, uint ptm);

/* ui */
static void print_board(board_t *b);
static int  play(board_t *b);
static void setup(board_t *b);

int main(void)
{
    char buffer[8];
    int outcome;
    board_t b;

    for (;;)
    {
        setup(&b);
        outcome = play(&b);
        if (outcome == 1)
            printf("\nCongratulations! You win!\n");
        else if (outcome == -1)
            printf("\nYou lose! Better luck next time!\n");
        else
            printf("\nThe game ended in a draw.\n");
        
        printf("Want to play again? (Y/N)\n");
        fgets(buffer, nelem(buffer), stdin);
        
        if (buffer[0] != 'y' && buffer[0] != 'Y')
            break;
    }
    printf("\nThanks for playing!\n");
    return 0;
}

static void setup(board_t *b)
{
    char c[8];
    printf("X (1st) or O (2nd): ");
    fgets(c, sizeof(c) / sizeof(c[0]), stdin);

    memset(b, 0, sizeof(*b));
    if (c[0] == 'X' || c[0] == 'x' || c[0] == '1')
        b->human_turn = 0;
    else
        b->human_turn = 1;
    b->player = 0;

    printf("\n");
}

static int play(board_t * b)
{
    char buffer[8];
    uint undo, choice;
    int score, depth;

    for (depth = 9;;)
    {
        print_board(b);
        if (b->human_turn == b->player)
        {
            printf("Make your move: ");
            fgets(buffer, 8, stdin);
            sscanf(buffer, "%u", &choice);
            if (choice < 1 || choice > 9 ||
                    ((b->side[!b->player] | b->side[b->player]) &
                     (1 << (choice - 1))))
            {
                printf("\nInvalid Move! Try again!\n\n");
                continue;
            }
            b->side[b->player] |= 1 << (choice - 1);
        }
        else
        {
            printf("Pondering...\n");
            search(b, depth, b->player);
            make_move(b, b->pv[depth - 1], &undo, b->player);
        }

        score = eval(b, b->player);
        if (score == oo || score == -oo)
        {
            print_board(b);
            if (b->human_turn == b->player)
                return 1;
            else
                return -1;
        }
        else if (((b->side[b->player] |
                   b->side[!b->player]) & 0x1FF) == 0x1FF)
        {
            print_board(b);
            return 0;
        }

        b->player = !b->player;
        depth--;
    }
    return 0;
}

static void print_board(board_t *b)
{
    uint i, bit;
    for (i = 0; i < 9; i++)
    {
        bit = 1 << i;
        if (b->side[0] & bit)
            printf("X ");
        else if (b->side[1] & bit)
            printf("O ");
        else
            printf(". ");
        if ((i % 3) == 2)
            printf("\n");
    }
    printf("\n");
}

static uint gen_moves(board_t *b, uint *moves, uint ptm)
{
    uint i, v, lsb;

    v = 0x1FF & ~(b->side[!ptm] | b->side[ptm]);

    for (lsb = 1, i = 0; v; v &= v - 1)
    {
        while (!(lsb & v))
            lsb <<= 1;
        moves[i++] = lsb;
    }
    return i;
}

static void make_move(board_t *b, uint move, uint *undo, uint ptm)
{
    *undo = b->side[ptm];
    b->side[ptm] |= move;
}

static void undo_move(board_t *b, uint move, uint ptm)
{
    b->side[ptm] = move;
}

static int eval(board_t *b, uint ptm)
{
    static const uint wins[] =
    {
        0x07, 0x38, 0x1C0,
        0x49, 0x92, 0x124,
        0x54, 0x111
    };

    uint i;

    for (i = 0; i < nelem(wins); i++)
    {
        if ((b->side[ptm] & wins[i]) == wins[i])
            return oo;
        else if ((b->side[!ptm] & wins[i]) == wins[i])
            return -oo;
    }
    return 0;
}

static int search(board_t *b, int depth, uint ptm)
{
    uint i, moves[9], nmoves, undo;
    int score, best, temp;

    score = eval(b, ptm);

    if (score == oo || score == -oo)
        return score;

    if (depth == 0)
        return score;

    nmoves = gen_moves(b, moves, ptm);

    best = -oo;
    b->pv[depth - 1] = moves[0];
    for (i = 0; i < nmoves; i++)
    {
        make_move(b, moves[i], &undo, ptm);
        score = -search(b, depth - 1, !ptm);
        if (score > best)
        {
            best = score;
            b->pv[depth - 1] = moves[i];
        }
        else if (score == best)
        {
            /* this is to immediately play the winning move
             * rather than prolong the game, since the eval
             * function only returns winning or drawn, and
             * nothing in between */
            temp = eval(b, ptm);
            if (temp == -oo || temp == oo)
            {
                b->pv[depth - 1] = moves[i];
                undo_move(b, undo, ptm);
                break;
            }
        }

        undo_move(b, undo, ptm);
    }

    return best;
}
