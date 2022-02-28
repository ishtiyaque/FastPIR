#include "server.hpp"

Server::Server(FastPIRParams params)
{
    context = new seal::SEALContext(params.get_seal_params());
    N = params.get_poly_modulus_degree();
    plain_bit_count = params.get_plain_modulus_size();

    evaluator = new seal::Evaluator(*context);
    batch_encoder = new seal::BatchEncoder(*context);

    this->num_obj = params.get_num_obj();
    this->obj_size = params.get_obj_size();

    num_query_ciphertext = params.get_num_query_ciphertext();
    num_columns_per_obj = params.get_num_columns_per_obj();
    db_rows = params.get_db_rows();
    db_preprocessed = false;
}

void Server::set_client_galois_keys(uint32_t client_id, seal::GaloisKeys gal_keys)
{
    client_galois_keys[client_id] = gal_keys;
}

void Server::encode_db(std::vector<std::vector<uint64_t>> db)
{
    encoded_db = std::vector<seal::Plaintext>(db.size());
    for (int i = 0; i < db.size(); i++)
    {
        batch_encoder->encode(db[i], encoded_db[i]);
    }
}

void Server::set_db(std::vector<std::vector<unsigned char> > db)
{
    assert(db.size() == num_obj);
    std::vector<std::vector<uint64_t> > extended_db(db_rows);
    for(int i = 0; i < db_rows;i++) {
        extended_db[i] = std::vector<uint64_t>(N, 1ULL);
    }
    int row_size = N/2;

    for(int i = 0; i < num_obj;i++) {
        std::vector<uint64_t> temp = encode(db[i]);

        int row = (i / row_size);
            int col = (i % row_size);
            for (int j = 0; j < num_columns_per_obj / 2; j++)
            {
                extended_db[row][col] = temp[j];
                extended_db[row][col+row_size] = temp[j+(num_columns_per_obj / 2)];
                row += num_query_ciphertext;
            }

    }   
    encode_db(extended_db);
    return;
}


void Server::preprocess_db()
{
    if (encoded_db.size() == 0)
    {
        std::cout << "db not set! preprocess failed!" <<std::endl;
        exit(1);
    }
    if (db_preprocessed)
        return;
    auto pid = context->first_parms_id();
    for (int i = 0; i < encoded_db.size(); i++)
    {
        evaluator->transform_to_ntt_inplace(encoded_db[i], pid);
    }
    db_preprocessed = true;
}

PIRResponse Server::get_response(uint32_t client_id, PIRQuery query)
{
    if (query.size() != num_query_ciphertext)
    {
        std::cout << "query size doesn't match" <<std::endl;
        exit(1);
    }
    seal::Ciphertext result;
    preprocess_query(query);
    if (!db_preprocessed)
    {
        preprocess_db();
    }

    seal::GaloisKeys gal_keys = client_galois_keys[client_id];
    seal::Ciphertext response = get_sum(query, gal_keys, 0, num_columns_per_obj / 2 - 1);
    return response;
}

seal::Ciphertext Server::get_sum(std::vector<seal::Ciphertext> &query, seal::GaloisKeys &gal_keys, uint32_t start, uint32_t end)
{
    seal::Ciphertext result;

    if (start != end)
    {
        int count = (end - start) + 1;
        int next_power_of_two = get_next_power_of_two(count);
        int mid = next_power_of_two / 2;
        seal::Ciphertext left_sum = get_sum(query, gal_keys, start, start + mid - 1);
        seal::Ciphertext right_sum = get_sum(query, gal_keys, start + mid, end);
        evaluator->rotate_rows_inplace(right_sum, -mid, gal_keys);
        evaluator->add_inplace(left_sum, right_sum);
        return left_sum;
    }
    else
    {

        seal::Ciphertext column_sum;
        seal::Ciphertext temp_ct;
        evaluator->multiply_plain(query[0], encoded_db[num_query_ciphertext * start], column_sum);

        for (int j = 1; j < num_query_ciphertext; j++)
        {
            evaluator->multiply_plain(query[j], encoded_db[num_query_ciphertext * start + j], temp_ct);
            evaluator->add_inplace(column_sum, temp_ct);
        }
        evaluator->transform_from_ntt_inplace(column_sum);
        return column_sum;
    }
}

uint32_t Server::get_next_power_of_two(uint32_t number)
{
    if (!(number & (number - 1)))
    {
        return number;
    }

    uint32_t number_of_bits = get_number_of_bits(number);
    return (1 << number_of_bits);
}

void Server::preprocess_query(std::vector<seal::Ciphertext> &query)
{
    for (int i = 0; i < query.size(); i++)
    {
        evaluator->transform_to_ntt_inplace(query[i]);
    }

    return;
}

uint32_t Server::get_number_of_bits(uint64_t number)
{
    uint32_t count = 0;
    while (number)
    {
        count++;
        number /= 2;
    }
    return count;
}

std::vector<uint64_t> Server::encode(std::vector<unsigned char> str){
    std::vector<uint64_t> res;
    std::string bit_str;
    int plain_data_bits = plain_bit_count - 1;
    int n = str.size();
    int remain = ((n/2)*8)%plain_data_bits;
    for(int iter = 0; iter < 2;iter++) {
        int start_byte = iter * (n/2);
        for (int i=0; i<n/2; i++) {
            bit_str += std::bitset<8>(str[start_byte + i]).to_string();
        }
        if (remain != 0){
            for (int i=0; i<(plain_data_bits - remain); i++)
                bit_str += "1";
        }
    }   
    for (int i=0; i<bit_str.length(); i+=plain_data_bits)
        res.push_back((uint64_t)std::stoi(bit_str.substr(i,plain_data_bits), nullptr, 2));

    return res;
}