import pytest
import requests
import json
from time import sleep

url = "http://localhost:6543/knapsack"

class TestMySet:
    def test_case_1(self):
        data = {"problem": {"capacity": 60, "weights": [10, 20, 33], "values": [10, 3, 30]}}
        #Submit the problem
        res = requests.post(url, json=data, headers={'Content-Type': 'application/json'})
        assert res.status_code == 200
        body = json.loads(res.content)
        jobId = body["task"];

        sleep(0.1)
        #Get the solution
        get_url = url;
        get_url = get_url+"/"+jobId;
        res = requests.get(get_url)
        body = json.loads(res.content)
        #solution expects total_value = 40 and packed_items=[0,2]
        assert '40' == body["solution"]["total_value"]
        assert [0,2] == body["solution"]["packed_items"]

    def test_case_15(self):
        data = {"problem": {"capacity": 750, "weights": [70 , 73 , 77 , 80 , 82 , 87 , 90 , 94 , 98 ,106 ,110 ,113 ,115 ,118 ,120], "values": [135 ,139 ,149 ,150 ,156 ,163 ,173 ,184 ,192 ,201 ,210 ,214 ,221 ,229 ,240]}}
        #Submit the problem
        res = requests.post(url, json=data, headers={'Content-Type': 'application/json'})
        assert res.status_code == 200
        body = json.loads(res.content)
        jobId = body["task"];

        sleep(0.1)
        #Get the solution
        get_url = url;
        get_url = get_url+"/"+jobId;
        res = requests.get(get_url)
        body = json.loads(res.content)
        #solution expects total_value = 40 and packed_items=[0,2]
        assert '1458' == body["solution"]["total_value"]
        assert [0, 2, 4, 6, 7, 8, 13, 14] == body["solution"]["packed_items"]

    def test_case_50_1(self):
        '''
        set of 50 objects for bins of capacity 850
        '''
        data = {"problem": {"capacity": 850, "weights": [7, 0, 30, 22, 80, 94, 11, 81, 70, 64, 59, 18, 0, 36, 3, 8, 15, 42, 9, 0, 42, 47, 52, 32, 26, 48, 55, 6, 29, 84, 2, 4, 18, 56, 7, 29, 93, 44, 71, 3, 86, 66, 31, 65, 0, 79, 20, 65, 52, 13], "values": [360, 83, 59, 130, 431, 67, 230, 52, 93, 125, 670, 892, 600, 38, 48, 147, 78, 256, 63, 17, 120, 164, 432, 35, 92, 110, 22, 42, 50, 323, 514, 28, 87, 73, 78, 15, 26, 78, 210, 36, 85, 189, 274, 43, 33, 10, 19, 389, 276, 312]}}
        #Submit the problem
        res = requests.post(url, json=data, headers={'Content-Type': 'application/json'})
        assert res.status_code == 200
        body = json.loads(res.content)
        jobId = body["task"];

        sleep(0.1)
        #Get the solution
        get_url = url;
        get_url = get_url+"/"+jobId;
        res = requests.get(get_url)
        body = json.loads(res.content)
        #solution expects total_value = 7534
        assert '7534' == body["solution"]["total_value"]
        assert [0, 1, 3, 4, 6, 10, 11, 12, 14, 15, 16, 17, 18, 19, 21, 22, 24, 27, 28, 29, 30, 31, 32, 34, 38, 39, 41, 42, 44, 47, 48, 49] == body["solution"]["packed_items"]
    def test_case_50_2(self):
        '''
        set of 50 objects for bins of capacity 850
        '''
        data = {"problem": {"capacity": 850, "weights": [360, 83, 59,  130, 431, 67, 230, 52,  93,  125, 670, 892, 600, 38,  48, 147, 78,  256, 63, 17,  120, 164, 432, 35,  92,  110, 22,  42, 50,  323, 514, 28, 87,  73,  78,  15,  26,  78,  210, 36,  85, 189, 274, 43,  33, 10,  19,  389, 276, 312], "values": [7,  0,  30, 22, 80, 94, 11, 81, 70, 64, 59, 18, 0,  36, 3,  8,  15, 42, 9,  0,  42, 47, 52, 32, 26, 48, 55, 6,  29, 84, 2,  4,  18, 56, 7,  29, 93, 44, 71, 3,  86, 66, 31, 65, 0,  79, 20, 65, 52, 13]}}
        #Submit the problem
        res = requests.post(url, json=data, headers={'Content-Type': 'application/json'})
        assert res.status_code == 200
        body = json.loads(res.content)
        jobId = body["task"];

        sleep(0.1)
        #Get the solution
        get_url = url;
        get_url = get_url+"/"+jobId;
        res = requests.get(get_url)
        body = json.loads(res.content)
        #solution expects total_value = 934 and packed_items=[2, 5, 7, 8, 9, 13, 23, 26, 33, 35, 36, 37, 40, 43, 45, 46]
        assert '934' == body["solution"]["total_value"]
        assert [2, 5, 7, 8, 9, 13, 23, 26, 33, 35, 36, 37, 40, 43, 45, 46] == body["solution"]["packed_items"]

pytest.main()

