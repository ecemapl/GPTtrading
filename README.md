# AI Trading Bot

A trading bot powered by local LLM (via LM Studio) and the Alpaca trading API.

## Overview

This bot fetches real-time stock information using the Alpaca API, tracks price changes over time, and seeks AI-powered trading advice from a local LLM running in LM Studio. It's an integration of local AI technology and stock trading for decision-making assistance.

## Features

- **Real-time Data**: Fetches current stock prices for 20 predefined stock symbols
- **Price Persistence**: Saves and loads price history between runs
- **AI Decision Assistance**: Uses local LLM to get buy/sell/hold advice based on price changes
- **Simple Trading Strategy**: Recommends selling if price increased >5%, buying if decreased >5%, otherwise hold

## Technologies

- **Alpaca API v2**: For fetching real-time stock data
- **LM Studio**: Local LLM server for AI trading advice
- **C++17**: Primary programming language for the bot
- **Python**: Used for interfacing with LM Studio API
- **nlohmann/json**: C++ library for JSON handling
- **curl**: HTTP client library for API calls

## Prerequisites

### 1. LM Studio Setup
1. Download and install [LM Studio](https://lmstudio.ai/)
2. Download any compatible model (e.g., Llama, Mistral, etc.)
3. Start LM Studio and load your chosen model
4. Start the local server (default: `http://127.0.0.1:1234`)

### 2. Alpaca API Account
1. Create a free account at [Alpaca Markets](https://alpaca.markets/)
2. Get your API keys from the dashboard
3. Update `gpttrading.cpp` with your keys:
   ```cpp
   headers = curl_slist_append(headers, "APCA-API-KEY-ID: YOUR_KEY_HERE");
   headers = curl_slist_append(headers, "APCA-API-SECRET-KEY: YOUR_SECRET_HERE");
   ```

### 3. System Dependencies
- **macOS**: `brew install curl nlohmann-json`
- **Ubuntu**: `sudo apt install libcurl4-openssl-dev nlohmann-json3-dev`
- **Python**: `pip install requests`

## Setup and Usage

1. **Clone the repository**
   ```bash
   git clone <repository-url>
   cd GPTtrading
   ```

2. **Update API keys** in `gpttrading.cpp`

3. **Start LM Studio**
   - Launch LM Studio
   - Load a model
   - Start local server on port 1234

4. **Compile the program**
   ```bash
   g++ -std=c++17 -I./json-develop/include -o gpttrading gpttrading.cpp -lcurl
   ```

5. **Run the bot**
   ```bash
   ./gpttrading
   ```

## How it Works

1. **Load Previous Prices**: Reads `prices.json` to get last known prices
2. **Fetch Current Data**: Gets real-time stock quotes from Alpaca API v2
3. **Calculate Changes**: Compares current prices with previous prices
4. **AI Analysis**: For significant changes, calls local LM Studio for trading advice
5. **Save Prices**: Updates `prices.json` with current prices for next run

## Trading Strategy

- **Sell**: If price increased >5% since last check
- **Buy**: If price decreased >5% since last check  
- **Hold**: Otherwise

## Monitored Stocks

AAPL, MSFT, AMZN, FB, GOOGL, GOOG, TSLA, NVDA, BRK.B, JPM, JNJ, V, PG, HD, UNH, MA, DIS, BAC, VZ, XOM

## Files

- `gpttrading.cpp`: Main C++ program
- `openai_advisor.py`: Python script for LM Studio communication
- `prices.json`: Price history storage (auto-generated)

## Troubleshooting

- **LM Studio connection failed**: Ensure LM Studio server is running on port 1234
- **Alpaca API errors**: Verify your API keys are correct and active
- **Compilation errors**: Check that all dependencies are installed
- **No recommendations**: Run twice - first run establishes baseline prices

