#include <iostream>
#include <stdlib.h>
#include <vector>
#include <cstdlib>



#include <azure/storage/files/shares.hpp>

using namespace Azure::Storage::Files::Shares;
using namespace std::chrono;


ShareFileClient upload(ShareClient shareClient, std::string user1, std::string user2, std::vector<uint8_t> content) {


    std::vector<std::string> stringythingy = { user1, user2 };
    sort(stringythingy.begin(), stringythingy.end());

    std::string fileName = stringythingy[0] + "+" + stringythingy[1];
    
    uint8_t* fileContent;
    std::cout << content.size();
    fileContent = (uint8_t*)calloc(content.size(), sizeof(uint8_t));

    for (int i = 0; i < content.size(); i++) {
        fileContent[i] = (const uint8_t)content[i];
    }

    // Create the ShareFileClient
    ShareFileClient fileClient = shareClient.GetRootDirectoryClient().GetFileClient(fileName);
    // Upload the file
    if (fileContent != NULL) {
        fileClient.UploadFrom(fileContent, content.size() );
    }
    else {
        std::cout << "\ncould not allocate memory\n";
    }
    return fileClient;
}

std::string download(ShareFileClient fileClient) {

    auto properties = fileClient.GetProperties().Value;

    auto output = properties.LastModified;
    std::cout << output.ToString();

    std::vector<uint8_t> fileDownloaded(properties.FileSize);
    fileClient.DownloadTo(fileDownloaded.data(), fileDownloaded.size());
    return ( std::string(fileDownloaded.begin(), fileDownloaded.end()) );
}


int main() {

    // Retrieve the connection string for use with the application. The storage
        // connection string is stored in an environment variable on the machine
        // running the application called AZURE_STORAGE_CONNECTION_STRING.
        // Note that _MSC_VER is set when using MSVC compiler.
    
    static const char* AZURE_STORAGE_CONNECTION_STRING = "AZURE_STORAGE_CONNECTION_STRING";
#if !defined(_MSC_VER)
    const char* connectionString = std::getenv(AZURE_STORAGE_CONNECTION_STRING);
#else
    // Use getenv_s for MSVC
    size_t requiredSize;
    getenv_s(&requiredSize, NULL, NULL, AZURE_STORAGE_CONNECTION_STRING);
    if (requiredSize == 0) {
        throw std::runtime_error("missing connection string from env.");
    }
    std::vector<char> value(requiredSize);
    getenv_s(&requiredSize, value.data(), value.size(), AZURE_STORAGE_CONNECTION_STRING);
    std::string connectionStringStr = std::string(value.begin(), value.end());
    const char* connectionString = connectionStringStr.c_str();
#endif



    // Initialize a new instance of ShareClient
    auto shareClient = ShareClient::CreateFromConnectionString(connectionString, "sample-share");
    // Create the files share. This will do nothing if the files share already exists.

    shareClient.CreateIfNotExists();

    std::string word = "[object Object]";
    std::vector<char> words(word.begin(), word.end());
    std::vector<uint8_t> converted;

    for (int i = 0; i < words.size(); i++) {
        converted.push_back(words[i]);
    }
    


    ShareFileClient fileClient = upload(shareClient, "hello", "hi", converted);
    
    std::cout << download(fileClient);

}


