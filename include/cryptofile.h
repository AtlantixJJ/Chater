#ifndef CRYPTO_FILE_H_
#define CRYPTO_FILE_H_

#include "base64.h"
#include "hex.h"
#include "json/json.h"
#include <string.h>
#include <fstream>
using namespace std;

class CryptoFile
{
public:
    CryptoFile():fin() {f = enc = NULL;}

    char* encodeFile(std::string fname);
    char* decodeString(std::string seq);
    char* decodeString(const char *seq, int seq_size);
    void  saveToFile(std::string fname);

    int getEncSize()const {return enc_size;}
    int getDecSize()const {return dec_size;}    

    void encode();
    void decode(const char *seq, int seq_size);
    void clear();


    int getFileSize() const {return file_size;}
    Json::Value getFileSegment(int pos, int block_size);
    string decodeFileSegment(const string &segment);
    void writeFileSegment(const string &seg, int pos, int block_size);
    void openOutputFile(std::string fname);
    void openFile(std::string fname);
    void closeFile();
    void closeOutputFile();

public:
    std::ifstream fin;
    std::ofstream fout;
    CryptoPP::HexEncoder hex_encoder;
    CryptoPP::HexDecoder hex_decoder;

    int file_size, enc_size, dec_size;
    char *f, *enc;
};

#endif