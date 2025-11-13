// Determines if number is prime

int isPrime(int n) {
    // Check divisibility from 2 to N-1
    int i;
    for (i = 2; i < n; i = i + 1) {
        // If n is divisible by i, it is not a prime number
        if (n % i == 0) {
            return 0;
        }
    }

    // If no divisors were found, N is a prime number
    return 1;
}

void main() {
    int n;
    n = 10;
    print("Is %d prime?\n", n);

    
    // Check if the number is prime
    if (isPrime(n)) {
        print("Yes\n");
    }
    else {
        print("No\n");
    }
}