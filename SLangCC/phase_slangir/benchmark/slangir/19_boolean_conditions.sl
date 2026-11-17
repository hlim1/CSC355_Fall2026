void main()
{
    boolean enabled = true;
    boolean blocked = false;
    if (enabled && !blocked) {
        print("enabled");
    }
    if (!enabled || blocked) {
        print("disabled");
    }
}
