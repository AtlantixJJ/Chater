#include "cryptofile.h"
#include <fstream>
using namespace std;

char* CryptoFile::encodeFile(string fname)
{
    std::ifstream fin(fname);

    fin.seekg(0, ios::end);
    file_size = fin.tellg();
    fin.seekg(0, ios::beg);

    f = new char[file_size + 1024];
    fin.read(f, sizeof(char) * file_size);

    encode();

    return enc;
}

char* CryptoFile::decodeString(std::string seq, std::string fname)
{
    return decodeString(seq.c_str(), seq.length(), fname);
}

char* CryptoFile::decodeString(const char *seq, int seq_size, std::string fname)
{
    std::ofstream fout(fname, ios::binary);
    decode(seq, seq_size);
    fout.write(f, sizeof(char) * dec_size);
    fout.close();
}

void CryptoFile::encode()
{
    hex_encoder.Put((unsigned char*)f, sizeof(char) * file_size);
    hex_encoder.MessageEnd();
    enc_size = hex_encoder.MaxRetrievable();
    enc = new char[file_size * 2 + 1024];
    hex_encoder.Get((unsigned char*)enc, enc_size * sizeof(char));
}

void CryptoFile::decode(const char *seq, int seq_size)
{
    hex_decoder.Put((unsigned char*)seq, sizeof(char) * seq_size);
    hex_decoder.MessageEnd();
    dec_size = hex_decoder.MaxRetrievable();
    f = new char[dec_size];
    hex_decoder.Get((unsigned char*)f, dec_size * sizeof(char));
}

void CryptoFile::clear()
{
    if (enc != NULL)
    {
        delete [] enc;
        enc = NULL;
    }

    if (f != NULL)
    {
        delete [] f;
        f = NULL;
    }
}