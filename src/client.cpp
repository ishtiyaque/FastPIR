#include "client.hpp"

Client::Client(FastPIRParams params)
{
    this->num_obj = params.get_num_obj();
    this->obj_size = params.get_obj_size();

    N = params.get_poly_modulus_degree();
    num_columns_per_obj = params.get_num_columns_per_obj();
    plain_bit_count = params.get_plain_modulus_size();
    num_query_ciphertext = params.get_num_query_ciphertext();

    context = new seal::SEALContext(params.get_seal_params());
    keygen = new seal::KeyGenerator(*context);
    secret_key = keygen->secret_key();
    encryptor = new seal::Encryptor(*context, secret_key);
    decryptor = new seal::Decryptor(*context, secret_key);
    batch_encoder = new seal::BatchEncoder(*context);

    std::vector<int> steps;
    for (int i = 1; i < (num_columns_per_obj / 2); i *= 2)
    {
        steps.push_back(-i);
    }
    keygen->create_galois_keys(steps, gal_keys);

    return;
}

PIRQuery Client::gen_query(uint32_t index)
{
    std::vector<seal::Ciphertext> query(num_query_ciphertext);
    seal::Plaintext pt;
    size_t slot_count = batch_encoder->slot_count();
    size_t row_size = slot_count / 2;

    for (int i = 0; i < num_query_ciphertext; i++)
    {
        std::vector<uint64_t> pod_matrix(slot_count, 0ULL);
        if ((index / row_size) == i)
        {
            pod_matrix[index % row_size] = 1;
            pod_matrix[row_size + (index % row_size)] = 1;
        }
        batch_encoder->encode(pod_matrix, pt);
        encryptor->encrypt_symmetric(pt, query[i]);
    }

    return query;
}

std::vector<unsigned char> Client::decode_response(PIRResponse response, uint32_t index)
{
    seal::Plaintext pt;
    std::vector<uint64_t> decoded_response;
    size_t row_size = N / 2;
    decryptor->decrypt(response, pt);
    batch_encoder->decode(pt, decoded_response);

    decoded_response = rotate_plain(decoded_response, index % row_size);

    return decode(decoded_response);
}

seal::GaloisKeys Client::get_galois_keys()
{
    return gal_keys;
}

std::vector<uint64_t> Client::rotate_plain(std::vector<uint64_t> original, int index)
{
    int sz = original.size();
    int row_count = sz / 2;
    std::vector<uint64_t> result(sz);
    for (int i = 0; i < row_count; i++)
    {
        result[i] = original[(index + i) % row_count];
        result[row_count + i] = original[row_count + ((index + i) % row_count)];
    }

    return result;
}

std::vector<unsigned char> Client::decode(std::vector<uint64_t> v)
{
    int n = v.size();
    const int plain_data_bits = plain_bit_count - 1;
    std::vector<unsigned char> res(obj_size);
    std::string bit_str;
    for (auto item : v)
    {
        bit_str += std::bitset<PLAIN_BIT>(item).to_string();
    }

    for (int i = 0; i < N / 2; i++)
    {
    }
    for (int i = 0; i < (obj_size / 2); i++)
    {
        res[i] = std::bitset<8>(bit_str.substr(i * 8, 8)).to_ulong();
        res[i + (obj_size / 2)] = std::bitset<8>(bit_str.substr((plain_data_bits * n / 2) + i * 8, 8)).to_ulong();
    }

    return res;
}