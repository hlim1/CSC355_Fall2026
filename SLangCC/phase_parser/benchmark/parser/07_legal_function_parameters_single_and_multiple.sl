int identity(int value) {
    return value;
}

int add(int left, int right) {
    return left + right;
}

void main() {
    int result = add(identity(1), 2);
    print(result);
}
