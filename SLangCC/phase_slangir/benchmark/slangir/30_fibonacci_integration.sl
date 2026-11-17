int fibonacci(int limit)
{
    int previous = 0;
    int current = 1;
    for (int index = 0; index < limit; index++) {
        int next = previous + current;
        previous = current;
        current = next;
    }
    return previous;
}

void main()
{
    int result = fibonacci(10);
    if (result > 0) {
        print("fibonacci", result);
    } else {
        print("invalid");
    }
}
