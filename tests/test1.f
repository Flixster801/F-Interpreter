void main() {
    int n;

    print("Enter a single digit integer between 0 and 9: ");

    n = getchar();

    while (n < 48 || n > 57) {
        print("'%s' is invalid. Enter a single digit integer between 0 and 9: ", n);
        n = getchar();
    }

    n = n - 48;

    print("The sum of the first %d squares is %d\n", n, n * (n + 1) * (2*n + 1) / 6);
}