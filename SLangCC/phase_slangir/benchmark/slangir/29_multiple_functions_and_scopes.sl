int adjust(int value)
{
    int result = value + 1;
    return result;
}

int combine(int value)
{
    int result = adjust(value) * 2;
    return result;
}

void main()
{
    int result = combine(4);
    print(result);
}
