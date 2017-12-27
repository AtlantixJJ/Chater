#include "hex.h"
#include <fstream>
#include <string.h>
#include <iostream>
using namespace std;

#define MAX_FILE 40960000

int main ()
{
    string fname, newfname;
    int file_size = 0, enc_size = 0, dec_size = 0;

    cin >> fname;
    newfname = fname + "_new";
    ifstream fin(fname, ios::binary);
    ofstream fout(newfname, ios::binary);

    fin.seekg(0, ios::end);
    file_size = fin.tellg();
    fin.seekg(0, ios::beg);

    cout << "Size: " << file_size << endl;

    char *f = new char[file_size + 1024];
    char *enc = new char[file_size * 2];
    char *dec = new char[file_size + 1024];

    if(!fin) cout << "open failed" << endl;

    //fin.read((char*)&file_size, sizeof(int));
    fin.read(f, sizeof(char) * MAX_FILE);
    fin.close();

    CryptoPP::HexEncoder hex_encoder;
    CryptoPP::HexDecoder hex_decoder;

    hex_encoder.Put((unsigned char*)f, sizeof(char) * file_size);
    hex_encoder.MessageEnd();
    enc_size = hex_encoder.MaxRetrievable();
    hex_encoder.Get((unsigned char*)enc, enc_size * sizeof(char));

    hex_decoder.Put((unsigned char*)enc, sizeof(char) * enc_size);
    hex_decoder.MessageEnd();
    dec_size = hex_decoder.MaxRetrievable();
    hex_decoder.Get((unsigned char*)dec, dec_size * sizeof(char));

    cout << enc_size << " " << dec_size << " " << file_size << endl;

    fout.write(dec, sizeof(char) * dec_size);
    //fout.flush();
    fout.close();

    return 0;
}