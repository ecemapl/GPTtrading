import requests
import json
import sys

def get_advice(percentage_change, current_price):
    # Prepare the input for LLM based on the received data
    prompt = f"""The current price is {current_price}. 
It has changed by {percentage_change}% since the last trade. 
According to a simple trading strategy, if the price has increased by more than 5% since the last trade, it should be sold.
If it has decreased by more than 5%, it should be bought.
Otherwise, the position should be held.
Based on this information and strategy, should the stock be bought, sold, or held?"""

    # Make a request to LM Studio
    response = requests.post(
        "http://127.0.0.1:1234/v1/completions",
        headers={"Content-Type": "application/json"},
        json={
            "prompt": prompt,
            "max_tokens": 50,
            "temperature": 0.7
        }
    )

    # Return the generated advice
    return response.json()["choices"][0]["text"].strip()


if __name__ == "__main__":
    # Read input data from command line arguments
    percentage_change = float(sys.argv[1])
    current_price = float(sys.argv[2])
    
    # Get the advice based on the input data
    advice = get_advice(percentage_change, current_price)

    # Print the advice to the console
    print(advice)

