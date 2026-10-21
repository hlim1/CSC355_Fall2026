int make_int()
{
    return 7;
}

void main()
{
    int value = 1;
    int copied = value;

    int numbers[2];
    numbers[0] = copied;
    int from_array = numbers[0];

    int from_call = make_int();
}
