
#include <iostream>
#include <unistd.h>
#include <chrono>

#include "bfvparams.h"
#include "fastpirparams.hpp"
#include "client.hpp"
#include "server.hpp"

void print_usage();
std::vector<std::vector<unsigned char>> populate_db(size_t num_obj, size_t obj_size);

int main(int argc, char *argv[])
{
    size_t obj_size = 0; // Size of each object in bytes
    size_t num_obj = 0;  // Total number of objects in DB

    int option;
    const char *optstring = "n:s:";
    while ((option = getopt(argc, argv, optstring)) != -1)
    {
        switch (option)
        {
        case 'n':
            num_obj = std::stoi(optarg);
            break;
        case 's':
            obj_size = std::stoi(optarg);
            break;
        case '?':
            print_usage();
            return 1;
        }
    }

    if (!num_obj || !obj_size)
    {
        print_usage();
        return 1;
    }
    if(obj_size%2 == 1) {
        obj_size++;
        std::cout<<"FastPIR expects even obj_size; padding obj_size to "<<obj_size<<" bytes"<<std::endl<<std::endl;
    }

    srand(time(NULL));
    std::chrono::high_resolution_clock::time_point time_start, time_end;

    FastPIRParams params(num_obj, obj_size);
    int desired_index = rand()%num_obj;
    std::cout<<"Retrieving element at index "<<desired_index<<std::endl<<std::endl;

    Server server(params);
    Client client(params);

    std::vector<std::vector<unsigned char>> db = populate_db(num_obj, obj_size);
    server.set_db(db);

    time_start = std::chrono::high_resolution_clock::now();
    server.preprocess_db();
    time_end = std::chrono::high_resolution_clock::now();
    auto db_preprocess_time = (std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start)).count();
    std::cout<<"Completed preprocessing db!"<<std::endl<<std::endl;


    server.set_client_galois_keys(0, client.get_galois_keys());

    time_start = std::chrono::high_resolution_clock::now();
    PIRQuery query = client.gen_query(desired_index);
    time_end = std::chrono::high_resolution_clock::now();
    auto query_time = (std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start)).count();
    std::cout<<"Generated PIR query!"<<std::endl;
    std::cout<<"Query size: "<<query.size() * query[0].size() * query[0].coeff_modulus_size() * POLY_MODULUS_DEGREE * 8<<" bytes"<<std::endl<<std::endl;

    std::cout<<"Generating PIR response..."<<std::endl;
    time_start = std::chrono::high_resolution_clock::now();
    PIRResponse response = server.get_response(0, query);
    time_end = std::chrono::high_resolution_clock::now();
    auto response_time = (std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start)).count();
    std::cout<<"PIR response generated!"<<std::endl;
    std::cout<<"Response size: "<<response.size() * response.coeff_modulus_size() * POLY_MODULUS_DEGREE * 8<<" bytes"<<std::endl<<std::endl;

    time_start = std::chrono::high_resolution_clock::now();
    std::vector<unsigned char> decoded_response = client.decode_response(response, desired_index);
    time_end = std::chrono::high_resolution_clock::now();
    auto decode_time = (std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start)).count();
    std::cout<<"Decoded PIR response!"<<std::endl<<std::endl;

    bool incorrect_result = false;
 
    for (int i = 0; i < obj_size ; i++)
    {
        if ((db[desired_index][i] != decoded_response[i]))
        {
            incorrect_result = true;
            break;
        }
    }

    if (incorrect_result)
    {
        std::cout << "PIR Result is incorrect!" << std::endl<<std::endl;
    }
    else
    {
        std::cout << "PIR result correct!" << std::endl<<std::endl;
    }

    std::cout << "DB preprocessing time (us): " << db_preprocess_time << std::endl;
    std::cout << "Query generation time (us): " << query_time << std::endl;
    std::cout << "Response generation time (us): " << response_time << std::endl;
    std::cout << "Response decode time (us): "<< decode_time << std::endl;
}

void print_usage()
{
    std::cout << "usage: main -n <number of objects> -s <object size in bytes>" << std::endl;
}


//Populate DB with random elements
//Overload this function to populate DB differently (e.g. from file)
std::vector<std::vector<unsigned char>> populate_db(size_t num_obj, size_t obj_size) {
    std::vector<std::vector<unsigned char>> db(num_obj);
    for (int i = 0; i < num_obj; i++)
    {
        db[i] = std::vector<unsigned char>(obj_size);
        for (int j = 0; j < obj_size; j++)
        {
            db[i][j] = rand() % 0xFF;
        }
    }
    return db;
}