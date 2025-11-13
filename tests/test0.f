// Remove non alpha characters from a string

void main() {
    int i, j;
    char c[62], s[62];

    c = "The quick brown fox jumps over the lazy dog.";
    j = 0;

    for (i = 0; i < 62; i = i + 1)
        if ((c[i] > 'a' && c[i] < 'z') || (c[i] > 'A' && c[i] < 'Z')) {
            s[j] = c[i];
            j = j + 1;
        }

    print("%s", s);
}