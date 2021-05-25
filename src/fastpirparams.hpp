#ifndef FASTPIR_PARAMS_H
#define FASTPIR_PARAMS_H

#include "bfvparams.h"
#include "seal/seal.h"

typedef  std::vector<seal::Ciphertext> PIRQuery;
typedef  seal::Ciphertext PIRResponse;


class FastPIRParams {
public:
    FastPIRParams(size_t num_obj, size_t obj_size);
    size_t get_num_obj();
    size_t get_obj_size();
    uint32_t get_num_query_ciphertext();
    uint32_t get_num_columns_per_obj();
    uint32_t get_db_rows();

    seal::EncryptionParameters get_seal_params();
    size_t get_poly_modulus_degree();
    size_t get_plain_modulus_size();
private:
    seal::EncryptionParameters seal_params;
    size_t num_obj;
    size_t obj_size;

    uint32_t num_query_ciphertext;
    uint32_t num_columns_per_obj;
    uint32_t db_rows;
};

#endif