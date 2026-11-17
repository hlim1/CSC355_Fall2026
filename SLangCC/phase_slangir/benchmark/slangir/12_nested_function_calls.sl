int add(int left, int right)
{
    return left + right;
}

int twice(int value)
{
    return value * 2;
}

void main()
{
    int result = add(twice(3), add(1, 2));
    print(result);
}
