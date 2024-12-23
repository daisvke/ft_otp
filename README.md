# ft_otp

## Description
This is a program that allows you to store
an initial password in file, and that is capable of generating a new one time password
every time it is requested.<br />

## Commands
```
// Install
make

// Generate the key
./ft_otp -g <hex_key>

The program receives as argument a hexadecimal key of at least 64 characters.
The program stores this key safely in a file called ft_otp.key,
which is encrypted with AES encryption using Crypto++.

// Generate the temporary password
./ft_otp -k ft_otp.key

The program generates a new temporary password based on the encrypted key
given as argument and prints it on the standard output.

// Check what TOTP code you should get
oathtool --totp -v $(cat key.hex)
```

• The one-time password is randomly generated by the HOTP algorithm (RFC 4226), and always contains the
same format, i.e. 6 digits.

## Documentation
* https://www.cryptopp.com/wiki/Advanced_Encryption_Standard
* https://www.ietf.org/rfc/rfc4226.txteee
