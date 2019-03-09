
```
Candidate Name:           Vipin Kumar
Contact Point:            Farhana Mohammed

# Knapsack Optimizer Service

Solution Structure
    ├── api.tar                      # Tar file containing api for container image
    ├── Dockerfile                   # Dockerfile to containerized the API
    ├── maerskApi.cpp                # API Code File
    ├── test_knapsack.py             # Python Pytest Test File
    ├── README.md                    # Project Information.


Problem Statement: Develop a REST API for solving knapsack problems.

Rest API endpoints:

POST 'http://localhost:6543/knapsack/knapsack'
content: 'application/json' with JSON knapsack problem specification
output: 'json' with JSON knapsack object

GET 'http://localhost:6543/knapsack/<id>'
output: 'json' with JSON knapsack object


# problem specification
{
    "problem": {
        "capacity": # non-negative integer
        "weights": # array of non-negative integers
        "values": # array of non-negative integers, as many as weights
    }
}

# knapsack object
{
    "task": # Task ID (ASCII string)
    "status": # one of "submitted", "started", "completed"
    "timestamps": {
        "submitted": # unix/epoch time
        "started": # unix/epoch time or null if not started
        "completed": # unix/epoch time or null if not completed
    }
    "problem": # problem specification as above (including capacity, weights, values)
    "solution": {  # if completed
        "packed_items" : # array of integers (indices to weights and values)
        "total_value": # sum of value of packed_items
    }
}
```
**Solution:**

**How to run containerized solution (Ubuntu 18.04 container):**
```
Clone the repo on your machine:
    git clone git@github.com:vkumarvs/knapsackapi.git

Build the container image :
   docker image build -t my-knapsack .

Run the service:
    docker container run -it -p 6543:6543 --name knapsack my-knapsack
```
**Testing:**
```
    Please make sure you have python 3 installed to run the test cases.
    Go into the cloned repo directory and run the below command.
        pytest test_knapsack.py
    test_knapsack.py has test cases and a few of the problem statement are taken from
    "https://developers.google.com/optimization/bin/knapsack" to verify the implemented solution.
```
**How to compile code from source:**

```
This code depends on Google Optimization Tools c++ library.
For further instruction how to install it on your desired platform please refer
to https://developers.google.com/optimization/install/cpp/linux

RestBed SDK dependency: RestBed SDK needs to be installed.
Please refer this https://github.com/Corvusoft/restbed/tree/master.

JsonBox : This library is an excellent tool to parse and write json data, code
has dependency on this. Please refer this to install https://github.com/anhero/JsonBox

All above dependency has been compiled and collected for you for Ubuntu 18.04
platform.
    Enter into your cloned repo directory
    Untar api.tar
    Execute compile.sh
    Before running your compiled solution, make sure you have set LD_LIBRARY_PATH to <repo directory>/lib
    Execute ./maerskApi
```

**Design:**
```
This API execute the knapsack jobs asynchronously. The reason behind to choose
C++ to implement the API is mostly performance.
I choose C++ as API backened and Python as a test framework to test the API.
```

**Limitation:**
```
This code has a hardcoded limit of running 50k jobs concurrently. This limit
comes from the time limit provided to implement the solution (One Day).

To make this code production ready, one need to think about running the solution
in multiple containers depending on the load requirements, Kubernates cluster can
manage scaling up and down the containers.
The distribution of the load between multiple containers needs to be taken care
of.
The collection of API performance and other metrices needs to be implemented.
Graceful job removal and API shutdown needs to be implemented.
```







