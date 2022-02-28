#ifndef FASTPIR_CLIENT_H
#define FASTPIR_CLIENT_H

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <unistd.h>
#include <bitset>

#include "seal/seal.h"
#include "fastpirparams.hpp"

class Client
{

public:
    Client(FastPIRParams parms);
    PIRQuery gen_query(uint32_t index);
    std::vector<unsigned char> decode_response(PIRResponse response, uint32_t index);
    seal::GaloisKeys get_galois_keys();

private:
    seal::SEALContext *context;
    seal::KeyGenerator *keygen;
    seal::SecretKey secret_key;
    seal::Encryptor *encryptor;
    seal::Decryptor *decryptor;
    seal::BatchEncoder *batch_encoder;
    seal::GaloisKeys gal_keys;
    uint32_t num_obj;
    uint32_t obj_size;
    uint32_t N; //poly modulus degree
    uint32_t plain_bit_count;
    uint32_t num_columns_per_obj;
    uint32_t num_query_ciphertext;

    std::vector<uint64_t> rotate_plain(std::vector<uint64_t> original, int index);
    std::vector<unsigned char> decode(std::vector<uint64_t> v);
};

#endif