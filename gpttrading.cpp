#include <iostream>
#include <string>
#include <map>
#include <cstdio> // For popen and pclose
#include <curl/curl.h>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp) {
    size_t totalSize = size * nmemb;
    userp->append((char *)contents, totalSize);
    return totalSize;
}

int main() {
    // Load previous prices from file
    std::map<std::string, double> lastTradePrices;
    std::ifstream priceFile("prices.json");
    if (priceFile.is_open()) {
        json priceData;
        priceFile >> priceData;
        for (auto& [symbol, price] : priceData.items()) {
            lastTradePrices[symbol] = price;
        }
        priceFile.close();
    }
    std::vector<std::string> symbols = {
        "AAPL", "MSFT", "AMZN", "FB", "GOOGL",
        "GOOG", "TSLA", "NVDA", "BRK.B", "JPM",
        "JNJ", "V", "PG", "HD", "UNH",
        "MA", "DIS", "BAC", "VZ", "XOM"
    };

    CURL *curl = curl_easy_init();

    if (curl) {
        for (const std::string &symbol : symbols) {
            std::string responseString;
            std::string url = "https://data.alpaca.markets/v2/stocks/" + symbol + "/quotes/latest";
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "APCA-API-KEY-ID: PKCOP7T7NCOBVZTCTBMILSDSEK");
            headers = curl_slist_append(headers, "APCA-API-SECRET-KEY: A8WHZLj7YsNuxpqF3rj64Sfu5pSsCx9VwAy2DV6NRPGS");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            } else {
                std::cout << "Raw response: " << responseString << std::endl;
                try {
                    json jsonResponse = json::parse(responseString);
                    if (jsonResponse.contains("quote") && !jsonResponse["quote"].is_null()) {
                        std::cout << "Symbol: " << symbol << std::endl;
                        double latestPrice = jsonResponse["quote"]["ap"];
                        std::cout << "Latest price: " << latestPrice << std::endl;

                        if (lastTradePrices.find(symbol) != lastTradePrices.end()) {
                            double lastTradePrice = lastTradePrices[symbol];
                            double percentageChange = 100.0 * (latestPrice - lastTradePrice) / lastTradePrice;

                            std::string command = "python openai_advisor.py " + std::to_string(percentageChange) + " " + std::to_string(latestPrice);
                            FILE* pipe = popen(command.c_str(), "r");
                            if (!pipe) throw std::runtime_error("popen() failed!");
                            char buffer[128];
                            std::string result = "";
                            while (!feof(pipe)) {
                                if (fgets(buffer, 128, pipe) != NULL)
                                    result += buffer;
                            }
                            pclose(pipe);

                            std::cout << "LLM recommendation for " << symbol << ": " << result << std::endl;
                        }
                        lastTradePrices[symbol] = latestPrice;
                    } else {
                        std::cout << "No valid data for " << symbol << std::endl;
                    }
                } catch (const std::exception& e) {
                    std::cout << "Error parsing JSON for " << symbol << ": " << e.what() << std::endl;
                }
            }
            curl_slist_free_all(headers);
        }
        curl_easy_cleanup(curl);
    }

    // Save prices to file
    std::ofstream outFile("prices.json");
    json priceData;
    for (const auto& [symbol, price] : lastTradePrices) {
        priceData[symbol] = price;
    }
    outFile << priceData.dump(4);
    outFile.close();

    return 0;
}

