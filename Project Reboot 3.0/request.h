#pragma once

#include <iostream>
#include <curl/curl.h>
#include <string>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);

int ParseSnowIndexFromResponse(const std::string& response);

void SendPostRequest(const std::string& url, const std::string& postData, int& snowIndex);
