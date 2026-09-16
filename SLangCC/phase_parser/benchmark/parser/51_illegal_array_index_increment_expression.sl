void main() {
    int i = 0;
    int values[3];
    values[i++] = 1;    # index = 0
    values[++i] = 2;    # index = 2
    values[1] = 3;
    values[i--] = 4;    # index = 2
    values[--i] = 5;    # index = 0
}
