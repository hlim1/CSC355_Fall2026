int main() {
    int i = 10;
    int k = 2;
    double d = 4.5;
    character c = 'z';
    string text = "hello";
    boolean flag = true;

    double r = (double)i / (double)k;

    string bad1 = (string)i;
    string bad2 = (string)d;
    int bad3 = (int)c;
    double bad4 = (double)c;
    character bad5 = (character)text;
    int bad6 = (int)flag;

    return 0;
}
