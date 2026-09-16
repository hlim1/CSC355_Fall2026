# factorial-like sample
int fact(int n) {
  int result = 1;
  while (n > 1) {
    result = result * n;
    n--;
  }
  return result;
}
print(fact(5));
