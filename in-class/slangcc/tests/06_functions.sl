int square(int x)
{
    return x * x;
}

int sumOfSquares(int a, int b)
{
    return square(a) + square(b);
}

void main()
{
    int answer = sumOfSquares(3, 4);
    print(answer);
}
