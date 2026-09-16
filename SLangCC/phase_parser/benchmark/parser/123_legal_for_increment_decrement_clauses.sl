void main() {
    int i = 0;
    int total = 0;

    for (++i; i < 3; i++) {
        total += i;
    }

    for (--i; i > 0; i--) {
        total += i;
    }

    for (i++; i > 0; i++) {
        total += i;
    }

    for (i--; i > 0; i--) {
        total += i;
    }
}
