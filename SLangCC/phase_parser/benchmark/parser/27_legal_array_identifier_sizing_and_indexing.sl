void main() {
    int size = 3;
    int index = 0;

    int numbers[size];
    int totals[3];

    numbers[0] = 1;
    numbers[index] = numbers[0] + 1;
    totals[index] = numbers[index];
    totals[2] = totals[index] + numbers[0];
}
