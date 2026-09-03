# A single-line SLang comment
void main()
{
    int x = 10;  # comments may appear after code

    #{
       This is a multiline comment.
       It is ignored by the lexer.
    }#

    print(x);
}
