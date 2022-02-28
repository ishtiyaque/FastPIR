#include "fastpirparams.hpp"

FastPIRParams::FastPIRParams(size_t num_obj, size_t obj_size)
{
    seal_params = seal::EncryptionParameters(seal::scheme_type::bfv);
    seal_params.set_poly_modulus_degree(POLY_MODULUS_DEGREE);
    seal_params.set_coeff_modulus(COEFF_MOD_ARR);
    seal_params.set_plain_modulus(PLAIN_MODULUS);

    this->num_obj = num_obj;
    this->obj_size = obj_size;

    //num_query_ciphertext = 2 * ceil(num_obj / (double)(POLY_MODULUS_DEGREE));
    num_query_ciphertext = ceil(num_obj / (double)(POLY_MODULUS_DEGREE/2));

    num_columns_per_obj = 2 * (ceil(((obj_size/2) * 8) / (float)(PLAIN_BIT)));
    num_columns_per_obj += num_columns_per_obj % 2;
    db_rows = ceil(num_obj / (double)POLY_MODULUS_DEGREE) * num_columns_per_obj;

    return;
}


size_t FastPIRParams::get_num_obj()
{
    return num_obj;
}

size_t FastPIRParams::get_obj_size()
{
    return obj_size;
}

uint32_t FastPIRParams::get_num_query_ciphertext()
{
    return num_query_ciphertext;
}

uint32_t FastPIRParams::get_num_columns_per_obj()
{
    return num_columns_per_obj;
}

uint32_t FastPIRParams::get_db_rows()
{
    return db_rows;
}

seal::EncryptionParameters FastPIRParams::get_seal_params()
{
    return seal_params;
}

size_t FastPIRParams::get_poly_modulus_degree()
{
    return seal_params.poly_modulus_degree();
}

size_t FastPIRParams::get_plain_modulus_size()
{
    return seal_params.plain_modulus().bit_count();
}
