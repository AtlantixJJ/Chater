#ifndef CRYPTO_FILE_H_
#define CRYPTO_FILE_H_

#include "hex.h"
#include <string.h>

class CryptoFile
{
public:
    CryptoFile() {f = enc = NULL;}

    char* encodeFile(std::string fname);
    char* decodeString(std::string seq, std::string fname);
    char* decodeString(const char *seq, int seq_size, std::string fname);

    void encode();
    void decode(const char *seq, int seq_size);
    void clear();

public:
    CryptoPP::HexEncoder hex_encoder;
    CryptoPP::HexDecoder hex_decoder;

    int file_size, enc_size, dec_size;
    char *f, *enc;
};

#endif