int identity(int value)
{
    return value;
}

int plus_one(int value)
{
    return value + 1;
}

void main()
{
    int numbers[2];
    numbers[0] = 4;
    int result = plus_one(identity(numbers[0]));
}
