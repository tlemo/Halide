#include "Halide.h"

using namespace Halide;

int main(int argc, char **argv) {
    // Make sure a tuple-valued associative reduction can be
    // horizontally vectorized.

    {
        // Tuple addition
        Func in;
        Var x;
        in(x) = {x, 2 * x};

        Func f;
        f() = {0, 0};

        RDom r(1, 100);
        f() = {f()[0] + in(r)[0], f()[1] + in(r)[1]};

        in.compute_root();
        f.update().atomic().vectorize(r, 8);  //.parallel(r);

        f.realize();
    }

    return 0;

    {
        // Complex multiplication is associative. Let's multiply a bunch
        // of complex numbers together.
        Func in;
        Var x;
        in(x) = {x, x};

        Func f;
        f() = {1, 0};

        RDom r(1, 100);
        Expr a_real = f()[0];
        Expr a_imag = f()[1];
        Expr b_real = in(r)[0];
        Expr b_imag = in(r)[1];
        f() = {a_real * b_real - a_imag * b_imag,
               a_real * b_imag + b_real * a_imag};

        in.compute_root();
        f.update().atomic().vectorize(r, 8);

        // Sadly, this won't actually vectorize, because it's not
        // expressible as a horizontal reduction op on a single
        // vector. You'd need to rfactor. We can at least check we get
        // the right value back though.
        f.realize();
    }

    printf("Success!\n");
    return 0;
}
