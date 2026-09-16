void main() {
    int left = 1;
    int right = 2;
    boolean result = (left < right) == (left <= right);

    result = result != (left > right);
    result = result == (right >= left);
}
