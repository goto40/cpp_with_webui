# triggers the C++ code via Python

import requests
import json 

num=123;
res = requests.get(f"http://localhost:18080/compute1/{num}")
response = json.loads(res.text)
print(response)

assert response["result"] == num*2
print("test passed")