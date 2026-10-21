int foo(int x, int y) {
    int z = x + y;
    return z * 2;
}

void main() {
    int left = 1, right = 2, total = left + right;

    int result = foo(total, 3);

    print(total, result);

    string first = "a", second = "b";

    print(total, first, second, result);
}
