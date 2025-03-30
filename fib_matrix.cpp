#include <iostream>

class matrix
{
    public:
        long long a, b, c, d;

        matrix operator * (matrix m) {
            matrix res;
            res.a = a * m.a +  b * m.c;
            res.b = a * m.b +  b * m.d;
            res.c = c * m.a +  d * m.c;
            res.d = c * m.b +  d * m.d;
            return res;
        }
};

matrix pow (matrix mat, int exp)
{
    matrix res;
    res.a = res.c = 1; res.b = res.d = 0;
    while (exp > 0) {
        if (exp & 1)
            res = res * mat;
        exp >>= 1;
        mat = mat * mat;
    }
    return res;
}

int main ()
{
    matrix base {1, 1, 1, 0};
    int n;
    std::cin >> n;
    std::cout << pow(base, n).b << std::endl;
    return 0;
}