void main() {
    int value = 1;
    boolean ready = false;

    int negative = -value;
    int nestedNegative = -(-value);
    boolean inverted = !ready;
    boolean nestedInverted = !!ready;

    print(negative, nestedNegative, inverted, nestedInverted);
}
