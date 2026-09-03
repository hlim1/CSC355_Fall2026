int factorial(int value)
{
    if (value <= 1) {
        return 1;
    }

    return value * factorial(value - 1);
}

void main()
{
    int result = factorial(5);
    print("factorial: ", result);
}
