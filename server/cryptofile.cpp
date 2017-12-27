#include "cryptofile.h"
#include <fstream>
#include <iostream>
using namespace std;
using namespace CryptoPP;
Json::Value CryptoFile::getFileSegment(int position, int block_size)
{
    fin.seekg(position, ios::beg);
    
    char* buffer = new char[block_size];
    fin.read(buffer, sizeof(byte) * block_size);

    string encoded;//,a;
    //if (position == 0) cout << "BUF: " <<  buffer << endl;
    //cout << "BUF : " << string(buffer) << endl;
    StringSource((byte*)buffer, block_size, true, new Base64Encoder(new StringSink(encoded), false));
    //StringSource(encoded, true, new Base64Decoder(new StringSink(a)));
    //string a = decodeFileSegment(encoded);
    //cout << "SEG : " << a << endl;
    //if (position == 0) cout << "ENC: " << encoded << endl;
    delete [] buffer;

    Json::Value node;

    node["size"] = Json::Value(file_size);
    node["data"] = Json::Value(encoded  );
    node["eof"]  = Json::Value(fin.eof());
    return node;
}

string CryptoFile::decodeFileSegment(const string &segment)
{
    string decoded;
    
    StringSource(segment, true, new Base64Decoder(new StringSink(decoded)));

    return decoded;
}

void CryptoFile::writeFileSegment(const string &seg, int pos, int block_size)
{
    //mcout << pos <<endl;
    //cout << "SEG: "<< seg << endl;
    string a = decodeFileSegment(seg);
    //cout << "DEC: "<< a << endl;
    fout.write(a.c_str(), a.length());
    //cout << a << endl;
    fout.flush();
}

void CryptoFile::closeFile()
{
    fin.close();
}

void CryptoFile::closeOutputFile()
{
    fout.close();
}

void CryptoFile::openFile(std::string fname)
{
    fin.open(fname, ios::binary|ios::in);
    fin.seekg(0, ios::end);
    file_size = fin.tellg();
    fin.seekg(0, ios::beg);
}

void CryptoFile::openOutputFile(std::string fname)
{
    fout.open(fname, ios::binary|ios::out);
}

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

char* CryptoFile::decodeString(std::string seq)
{
    return decodeString(seq.c_str(), seq.length());
}

char* CryptoFile::decodeString(const char *seq, int seq_size)
{
    decode(seq, seq_size);
}

void CryptoFile::saveToFile(std::string fname)
{
    std::ofstream fout(fname, ios::binary);
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