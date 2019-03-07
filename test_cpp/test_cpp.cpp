// test_cpp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <was/storage_account.h>
#include <was/blob.h>
#include <iostream>


int main()
{
    const utility::string_t storage_connection_string(U("DefaultEndpointsProtocol=http;AccountName=tankstdcppt;AccountKey=wLdyhxv9ph6TQryld3TWfOwqSxx/7HLAcqgZX++MgPxXQZVpHyctCp3P1M/VG1NDdybHbzffzTraIY4Y5W2sQw==;EndpointSuffix=core.windows.net"));
    azure::storage::cloud_storage_account storage_account = azure::storage::cloud_storage_account::parse(storage_connection_string);

    // Create the blob client.
    azure::storage::cloud_blob_client blob_client = storage_account.create_cloud_blob_client();

    // Retrieve a reference to a container.
    azure::storage::cloud_blob_container container = blob_client.get_container_reference(U("container"));

    azure::storage::cloud_blob l_blob = container.get_blob_reference(_XPLATSTR("Nicholas"));

    concurrency::streams::container_buffer<std::vector<uint8_t>> output_buffer;
    try 
    {
        l_blob.download_to_stream(output_buffer.create_ostream());
    }
    catch (const azure::storage::storage_exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}

