#ifndef FASTPIR_SERVER_H
#define FASTPIR_SERVER_H

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <unistd.h>
#include <bitset>
#include<cassert>
#include "seal/seal.h"
#include "fastpirparams.hpp"

class Server
{

public:
    Server(FastPIRParams parms);
    void set_client_galois_keys(uint32_t client_id, seal::GaloisKeys gal_keys);
    void set_db(std::vector<std::vector<unsigned char>> db);
    void preprocess_db();
    PIRResponse get_response(uint32_t client_id, PIRQuery query);

private:
    seal::SEALContext *context;
    seal::Evaluator *evaluator;
    seal::BatchEncoder *batch_encoder;
    std::map<uint32_t, seal::GaloisKeys> client_galois_keys;
    std::vector<seal::Plaintext> encoded_db;
    uint32_t num_obj;
    uint32_t obj_size;
    uint32_t num_columns_per_obj;
    uint32_t num_query_ciphertext;
    uint32_t N;
    uint32_t plain_bit_count;
    uint32_t db_rows;
    bool db_preprocessed;

    void encode_db(std::vector<std::vector<uint64_t>> db);
    void preprocess_query(std::vector<seal::Ciphertext> &query);
    std::vector<uint64_t> encode(std::vector<unsigned char> str);
    seal::Ciphertext get_sum(std::vector<seal::Ciphertext> &query, seal::GaloisKeys &gal_keys, uint32_t start, uint32_t end);
    uint32_t get_next_power_of_two(uint32_t number);
    uint32_t get_number_of_bits(uint64_t number);
};

#endif