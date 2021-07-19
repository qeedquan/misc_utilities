// 7x7 Tentaizu solver using backtracking
// expects a text file format like this:

/*
1 <- number of boards to read in after this

. means empty square
a number represents how many stars to expect near it, so 1 means 1 star near it

.01.2..
0..2...
.2..12.
1.3..2.
.2.112.
12.....
.1..1.1
*/

import java.lang.*;
import java.math.*;
import java.io.*;
import java.util.*;

// class that we use to hold the square
class TentaizuSquare
{
    public char             piece;
    public int              cx, cy;
    public int              star_ctr, dep_ctr, dis_ctr;
    public TentaizuSquare[] dep_list, dis_list;

    TentaizuSquare(int size, int cx_, int cy_)
    {
        dis_list = new TentaizuSquare[size];
        dep_list = new TentaizuSquare[size];
        reset();
        cx = cx_;
        cy = cy_;
    }

    void reset()
    {
        dis_ctr  =  0;
        dep_ctr  =  0;
        star_ctr = -1;
        piece    = '.';
    }
}

public class tentaizu
{
    // default constants
    private static final String       input_file  = "tentaizu.in";
    private static final int          BOARD_SIZE  = 7;
    private static final int          NUM_STARS   = 10;
    private static final int          MAX_MOVES   = BOARD_SIZE * BOARD_SIZE;
    
    // store square
    private static TentaizuSquare[][] square;

    // location to the hint squares 
    private static TentaizuSquare[]   hintsq_list, zerosq_list, freesq_list;
    private static int                hintsq_ctr,  zerosq_ctr,  freesq_ctr;

    // keep track of how many stars 
    private static int                nstars;
    private static boolean            found_solution;

    public static void main(String[] args)
    {
        int               i, x, y, test_cases;
        Scanner           ifp;

        // open the file
        try
        {
            String filename = input_file;
            if (args.length >= 1)
                filename = args[0];

            ifp = new Scanner(new File(filename), "UTF-8");
        }
        catch (FileNotFoundException e)
        {
            System.out.println("can't load file: " + e.getMessage());
            return;
        }

        // allocate memory 
        square = new TentaizuSquare[BOARD_SIZE][BOARD_SIZE];
        for (x = 0; x < BOARD_SIZE; x++)
        {
            for (y = 0; y < BOARD_SIZE; y++)
                square[x][y] = new TentaizuSquare(8, x, y);
        }

        hintsq_list = new TentaizuSquare[MAX_MOVES]; 
        zerosq_list = new TentaizuSquare[MAX_MOVES];
        freesq_list = new TentaizuSquare[MAX_MOVES];
        
        // get num test cases
        test_cases = ifp.nextInt();

        for (i = 0; i < test_cases; i++)
        {
            System.out.println("Tentaizu Board #" + (i + 1) + ": ");

            // get the board data from file
            if (get_board(ifp) < 0)
            {
                System.out.println("Invalid Board! ");
                System.out.println("");
                continue;
            }

            // solve it
            solve();
        }

        // cleanup
        ifp.close();
    }

    // print the board 
    private static void print_board()
    {
        int x, y;
        for (x = 0; x < BOARD_SIZE; x++)
        {
            for (y = 0; y < BOARD_SIZE; y++)
                System.out.print(square[x][y].piece);
            
            System.out.println("");
        }

        System.out.println("");
    }

    // get the board data from the file
    private static int get_board(Scanner ifp)
    {
        String s;
        char   c;
        int    x, y;

        hintsq_ctr = 0;
        zerosq_ctr = 0;
        
        for (x = 0; x < BOARD_SIZE; x++)
        {
            s = ifp.next();
            for (y = 0; y < BOARD_SIZE; y++)
            {
                square[x][y].reset();
                square[x][y].piece = c = s.charAt(y);

                // sanity test
                if (c != '.' && c != '*' && (c < '0' || c > '8'))
                {
                    System.out.println("Invalid character in board: " + c);
                    return -1;
                }

                // save the hint square location for quick lookup
                if (c >= '0')
                {
                    square[x][y].star_ctr = c - '0';

                    if (square[x][y].star_ctr > 0)
                        hintsq_list[hintsq_ctr++] = square[x][y];
                    else
                        zerosq_list[zerosq_ctr++] = square[x][y];
                }
            }
        }

        return 0;
    }

    // check to see if it is a valid move
    private static boolean is_valid_move(int x, int y)
    {
        // out of bounds
        if (x < 0 || y < 0 || x >= BOARD_SIZE || y >= BOARD_SIZE)
            return false;

        // check if the square is a hint square, a star square, or a no entry square
        if (Character.isDigit(square[x][y].piece) || square[x][y].piece == '*' 
                || square[x][y].dis_ctr > 0)
            return false;

        return true;
    }

    // generate moves given a square
    private static int gen_moves_square(TentaizuSquare sq, TentaizuSquare[] moves)
    {
        int     i, k, x, y;
        
        // the offset from this square to a generated move square
        int[][] offsets = 
        {
            { -1, -1 }, { 0, -1 }, { 1, -1 },
            { -1,  0 }, { 1,  0 },
            { -1,  1 }, { 0,  1 }, { 1,  1 },
        };

        for (i = k = 0; i < offsets.length; i++)
        {
            x = sq.cx + offsets[i][0];
            y = sq.cy + offsets[i][1];

            if (is_valid_move(x, y))
                moves[k++] = square[x][y];
        }

        return k;
    }

    // generate moves for backtracking
    private static int gen_moves(TentaizuSquare[] moves)
    {
        int i, x, y;

        // go through all the hint square, looking for one
        // that has star counter > 0, so we can generate moves
        // for that hint square
        for (i = 0; i < hintsq_ctr; i++)
        {
            if (hintsq_list[i].star_ctr > 0)
                return gen_moves_square(hintsq_list[i], moves);
        }

        // this means that the hint squares are all filled
        // but we haven't placed enough stars yet, find free squares
        // to place stars on
        for (i = x = 0; x < freesq_ctr; x++)
        {
            if (is_valid_move(freesq_list[x].cx, freesq_list[x].cy))
                moves[i++] = freesq_list[x];
        }

        return i;
    }
    
    // make a move
    private static void make_move(TentaizuSquare to)
    {
        int i;

        // set it to a star
        to.piece = '*';
        nstars++;

        // decrement the stars counter for any adjacent squares
        for (i = 0; i < to.dep_ctr; i++)
            to.dep_list[i].star_ctr--;
    }

    // undo a move
    private static void undo_move(TentaizuSquare to)
    {
        int i;

        // reset the piece
        to.piece = '.';
        nstars--;

        for (i = 0; i < to.dep_ctr; i++)
            to.dep_list[i].star_ctr++;
    }

    // check for conflicts, 
    private static boolean is_conflict()
    {
        int i;

        // check if it is >= max stars we
        // are willing to accept the = is there
        // because we have is_solution() running
        // before this one
        if (nstars >= NUM_STARS)
            return true;

        for (i = 0; i < hintsq_ctr; i++)
        {
            if (hintsq_list[i].star_ctr < 0)
                return true;
        }

        return false;
    }

    // check if we have a valid solution
    private static boolean is_solution()
    {
        int i;

        // reach the stars we needed?
        if (nstars != NUM_STARS)
            return false;

        // all the hint square constraint is filled?
        for (i = 0; i < hintsq_ctr; i++)
            if (hintsq_list[i].star_ctr != 0)
                return false;

        return true;
    }

    // setup stuff to solve
    private static void solve()
    {
        TentaizuSquare[] moves;
        int i, k, j, x, y, nmoves;
        
        moves          = new TentaizuSquare[MAX_MOVES];
        nstars         = 0;
        found_solution = false;
        
        // build the constraint table
        
        // first do the one zeroes hint square
        for (k = 0; k < zerosq_ctr; k++)
        {
            nmoves = gen_moves_square(zerosq_list[k], moves);
            for (j = 0; j < nmoves; j++)
                moves[j].dis_list[moves[j].dis_ctr++] = zerosq_list[k];
        }

        // then do the one with non zeroes
        for (k = 0; k < hintsq_ctr; k++)
        {
            nmoves = gen_moves_square(hintsq_list[k], moves);
            for (j = 0; j < nmoves; j++)
                moves[j].dep_list[moves[j].dep_ctr++] = hintsq_list[k];
        }


        freesq_ctr = 0;
        for (x = 0; x < BOARD_SIZE; x++)
        {
            for (y = 0; y < BOARD_SIZE; y++)
            {
                // this is stars already filled for you, just take it 
                // as is and use it
                if (square[x][y].piece == '*')
                {
                    nstars++;
                    for (k = 0; k < hintsq_ctr; k++)
                    {
                        if (Math.abs(x - hintsq_list[k].cx) <= 1 && 
                                Math.abs(y - hintsq_list[k].cy) <= 1)
                            hintsq_list[k].star_ctr--;
                    }
                }
    
                // then make a list of free squares for generating
                // moves if we don't have a free hint square left        
                else if (is_valid_move(x, y) && square[x][y].dep_ctr == 0)
                    freesq_list[freesq_ctr++] = square[x][y];
            }
        }

        solve_rec();
    }

    // solve it using backtracking
    private static void solve_rec()
    {
        TentaizuSquare[] moves;
        int i, nmove;

        // found a solution
        if (is_solution())
        {
            found_solution = true;
            print_board();
            return;
        }

        // if there is a conflict, backtrack
        if (is_conflict())
            return;

        // generate all the moves
        moves = new TentaizuSquare[MAX_MOVES];
        nmove = gen_moves(moves);
      
        // go through them one by one
        for (i = 0; i < nmove; i++)
        {
            make_move(moves[i]);
            solve_rec();
            if (found_solution)
                return;
            undo_move(moves[i]);
        }
    }
}
