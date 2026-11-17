void main()
{
    int outer = 0;
    int inner = 0;
    while (outer < 3) {
        inner = 0;
        while (inner < 3) {
            if (inner == 1) {
                inner++;
                continue;
            }
            print(outer, inner);
            inner++;
        }
        outer++;
    }
}
