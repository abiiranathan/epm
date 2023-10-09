# epm

A simple encrypted password manager written in c++.

The passwords are encrypted using `AES-128-ECB` provided by OpenSSL 3 library and stored in a file called `epm.bin` in the system's configuration directory. The file is not encrypted, but the passwords are.

On Linux, the configuration directory is `~/.config/epm/` and on Windows it is `%APPDATA%\epm\`. On MacOS, it is `~/Library/Application Support/epm/`.

To encrypt the passwords, a secret key is used. The key is stored in the system's configuration directory in a file called `epm.key`. The key is encrypted using [Argon2](https://doc.libsodium.org/password_hashing) from [libsodium](https://doc.libsodium.org/) with a user-provided password.

## Installation

Download pre-compiled binaries from the [releases](https://github.com/abiiranathan/epm/releases) or build it yourself.

Using make:

1. Clone the repository.
2. `cd epm`
3. `make`
4. `./emp keygen`

### Using CMake:

1. Clone the repository.
2. `cd epm`
3. `mkdir build`
4. `cd build`
5. `cmake ..`
6. `make`
7. `./emp keygen`

### Usage

1. Generate a secret key.
   `./emp keygen` then enter your password to initialize.
2. Add password to store.
   `./emp add https://google.com password`
3. Retrieve password for account
   `./emp get https://google.com`
4. View available account names.
   `./emp list`
5. Delete an account from the password store.
   `./emp delete <name>`

Type `./emp help` for more information.

#### Dependencies

- [OpenSSL 3](https://www.openssl.org/)
- [libsodium](https://doc.libsodium.org/)

Install the dependencies using your package manager.

On Linux:

```bash
sudo apt install libssl-dev libsodium-dev
```

On MacOS:

```bash
brew install openssl libsodium
```

On Windows:

```bash
vcpkg install openssl libsodium
```

Licence: MIT
