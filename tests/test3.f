// Makes a pyramid

void main() {
    int i, j, n; // n is initialized to 0 by default

    while (n < 1 || n > 9) {
        print("Enter a single digit number: ");
        n = getchar() - '0';
    }

    for (i = 1; i <= n; i = i + 1) {
        for (j = i; j < n; j = j + 1) {
            print("  ");
        }

        for (j = 0; j < i*2-1; j = j + 1) {
            print("\xdb\xdb");
        }
        print("\n");
    }
}