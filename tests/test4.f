// Converts hex to decimal

int hex2int(char hex) {
    int i, digit;
    digit = -1;

    if (hex >= '0' && hex <= '9') {
        digit = hex - '0';
    }
    else {
        if (hex >= 'a' && hex <= 'f') {
            digit = hex - 'a' + 10;
        }
        else {
            if (hex >= 'A' && hex <= 'F') {
                digit = hex - 'A' + 10;
            }
        }
    }
    return digit;
}

void main() {
    char hex[4];
    int i, digit, number;

    hex = "feed";
    number = 0;
    digit = 0;

    for (i = 0; i < 4 && digit > -1; i = i + 1) {
        digit = hex2int(hex[i]);
        if (digit > -1) {
            number = number * 16 + digit;
        }
    }
    
    if (digit > -1) {
        print("Hex: 0x%s is %d decimal\n", hex, number);
    }
}