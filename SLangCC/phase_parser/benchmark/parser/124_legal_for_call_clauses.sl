void tick() {
    return;
}

void main() {
    int i = 0;

    for (tick(); i < 2; tick()) {
        i++;
    }

    for (i+1; i < 2; i++) {
        i++;
    }

    for ((i+1); i < 2; i++) {
        i++;
    }

    for (i+1; (i < 2); i++) {
        i++;
    }

    for (i+1; i < 2; (i++)) {
        i++;
    }
}
