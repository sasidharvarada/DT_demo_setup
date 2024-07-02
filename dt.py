from flask import Flask, jsonify
import requests
import csv
import time

app = Flask(_name_)

url = "http://dev-onem2m.iiit.ac.in:443/~/in-cse/in-name/AE-CM/CM-MG99-00/Data/la"

headers = {
    'X-M2M-Origin': 'Tue_20_12_22:Tue_20_12_22',
    'Accept': 'application/json'
}

@app.route('/', methods=['POST'])
def get_data():
    try:
        response = requests.get(url, headers=headers)

        if response.status_code == 200:
            new_data = response.json()
            con_data = new_data["m2m:cin"]["con"]
            
            # Save con_data to dt.csv
            save_to_csv(con_data)

            return jsonify({'con_data': con_data})
        else:
            return jsonify({'error': f"Failed to fetch data (HTTP {response.status_code})"})

    except requests.exceptions.RequestException as e:
        return jsonify({'error': f"Request failed: {str(e)}"})

def save_to_csv(data):
    with open("/home/rishi/dt.csv", "a", newline='') as dt_file:
        csv_writer = csv.writer(dt_file)
        csv_writer.writerow([data])


if _name_ == '_main_':
    app.run(host='10.4.21.192', port=7006)