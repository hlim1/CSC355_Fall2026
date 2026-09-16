#{
  Leading multiline comment.
  Tokens ignored here: int fake = 99; } this brace is not a close.
}#
void main() {
    int value = 1;
    #{
      This comment spans lines and hides tokens:
      if (value >= 1) { print("hidden"); }
      A lone # is allowed here.
    }#
    value = value + 1; #{ trailing multiline comment }#
    # single-line comments still work beside multiline comments
    print(value);
}
