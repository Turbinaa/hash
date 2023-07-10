# Brute-force SHA-256 guesser
*Note: This app is not meant to be used for efficient hash guessing, if you need to guess simple hashes there are probably better alternatives*
## Features
- Multithreaded (NO THREAD SAFETY)
- Designed for linux operating systems
- Up to 32 threads
- *Shouldn't* contain memory leaks
## Limitations
- Like i said there's no thread safety
- Can have problems with guessing certain hahses, didn't test this app much
## Installation
You can find executable at bin/ directory after running `make`
## Example command / How to use
`hashguesser --verbose --threads 4 $(cat /path/to/hash)`

### Where
- `--verbose` or `-v` prints hashes  that program tries to guess (Not recommended since i/o is slow)
- `--threads` or `-t` specifies the number of *threads* for program to use *default:* 1
- Last argument is **plain** text for hash you trying to "decrypt". Can be either plain pasted hash or text file with a single line in it.
## Credits
I used sha-256 algorithm provided by [this repository](https://github.com/B-Con/crypto-algorithms) made by Brad Conte.

THERE'S NO LICENSE