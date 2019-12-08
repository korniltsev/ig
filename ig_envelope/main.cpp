#include <iostream>
#include <string>

#include <openssl/bio.h>
#include <openssl/pem.h>


std::string FBEnvelopeCreateEncryptedData(const std::string *pubKey, const std::string *pass, const std::string *time) {
    BIO *tmp_pubkey_buf = BIO_new_mem_buf(pubKey->c_str(), pubKey->length());
    EVP_PKEY * pubkey = PEM_read_bio_PUBKEY(tmp_pubkey_buf, 0, 0, 0);
    BIO_free(tmp_pubkey_buf);

    if (EVP_PKEY_base_id(pubkey) != 6) {
        std::cerr << "EVP_PKEY_base_id(pubkey) != 6" << std::endl;
        return "";
    }
    int key_size = EVP_PKEY_size(pubkey);


    unsigned long buflen = pass->length() + key_size + 32;
//    unsigned long buflen = 512;

//    unsigned char *a = new unsigned char[buflen];
    std::string res;
    res.resize(buflen);
    unsigned char *a = (unsigned char *)res.c_str();
    bzero(a, buflen);
    a[0] = 0x1;//version
    a[1] = 0xcd;
//    0xcd01
//    0x0


    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER *v12 = EVP_aes_256_gcm();
//    int lll = EVP_CIPHER_iv_length(v12);
    int ekl = 0;
    unsigned char *ek = a + 16;
    unsigned char *iv = a + 2;
    if (!EVP_SealInit(ctx, v12, &ek, &ekl, iv, &pubkey, 1)) {
        std::cerr << "EVP_SealInit" << std::endl;
        return "";
    }
    unsigned short *eklp = (unsigned short *) (a + 14);
    *eklp = (unsigned short) ekl;

//    *(_WORD *)(v11 + 13) = ekl;
//    p1 += ekl + 16;


    int outl;
    if ( !EVP_EncryptUpdate(ctx, 0, &outl, (const unsigned char *) time->c_str(), time->length()) ){
        std::cerr << "EVP_EncryptUpdate time" << std::endl;
        return "";
    }
    unsigned char * it = a + 16 + 256 + 16;                          // 16 + 256 + 16
    if ( !EVP_EncryptUpdate(ctx, it, &outl, (const unsigned char *) pass->c_str(), pass->length()) ){
        std::cerr << "EVP_EncryptUpdate pass" << std::endl;
        return "";
    }

    it += outl;
    if ( !EVP_SealFinal(ctx, it, &outl) ){
        std::cerr << "EVP_SealFinal time" << std::endl;
        return "";
    }
    it += outl;
    if ( !EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, a + 16 + 256) ){
        std::cerr << "EVP_CIPHER_CTX_ctrl EVP_CTRL_GCM_GET_TAG" << std::endl;
        return "";
    }
    return res;
}

std::string string_to_hex(const std::string& input)
{
    static const char* const lut = "0123456789abcdef";
    size_t len = input.length();

    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "usage: ./ig_envelope pub_key unixtimestap password";
        return -1;
    }
//    std::string publicKey = "-----BEGIN PUBLIC KEY-----\n"
//                     "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuztY8FoRTFEOf+TdLiTt\n"
//                     "e7qHAv5rgA2i9FD4b83fMF+xaymxoLRuNJN+QjrwvpnJmKCD14gw+l7Ld4Dy/DuE\n"
//                     "DbfZJrdQZBHOwkKtjt7d5iadWNJ7Ks9A3CMo0yRKrdPFSWlKmeARlNQkUqtbCfO7\n"
//                     "OjacvXWgIpijNGIEY8RGsEbVfgqJk+g8nAfbOLc6a0m31rBVeBzgHdaa1xSJ8bGq\n"
//                     "eln3mhxX56rjS8nKdi834YJSZWuqPvfYe+lEz6NeiME0J7tO4ylfyiOCNrrqwJrg\n"
//                     "0WY1Dx7x0yYj7k7SdQeKUEZgqc5AnV+c5D6J2SI9F2shelF5eof2NbIvNaMjJRD8\n"
//                     "oQIDAQAB\n"
//                     "-----END PUBLIC KEY-----\n";
//    std::string time = "1575751737";
//    std::string pass = "qwerty1234";
    std::string publicKey = argv[1];
    std::string time = argv[2];
    std::string pass = argv[3];
    std::string envelope = FBEnvelopeCreateEncryptedData(&publicKey, &pass, &time);
    if (envelope == "") {
        return -1;
    } else {
        std::string ehex = string_to_hex(envelope);
        std::cout << ehex << std::endl;
        return 0;
    }
}