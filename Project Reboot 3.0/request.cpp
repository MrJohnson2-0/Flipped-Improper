#include "request.h"

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

int ParseSnowIndexFromResponse(const std::string& response) {
    try {
        return std::stoi(response);
    }
    catch (const std::exception& e) {
        return -1;
    }
}

void SendPostRequest(const std::string& url, const std::string& postData, int& snowIndex) {
    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        else {
            snowIndex = ParseSnowIndexFromResponse(response);
            std::cout << "Received Snow Index from server: " << snowIndex << std::endl;
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}
