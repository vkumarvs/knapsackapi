#include <thread>
#include <memory>
#include <chrono>
#include <cstdlib>
#include <restbed>
#include "JsonBox.h"
#include <chrono>
#include <thread>
#include <unordered_map>

// Bi-dimensional knapsack problem.
#include <iterator>
#include <numeric>
#include <sstream>
#include "ortools/algorithms/knapsack_solver.h"
#include "ortools/base/logging.h"

using namespace std;
using namespace restbed;
using namespace std::chrono;

namespace {

    enum JobStatus {
        JOB_STARTED=1,
        JOB_SUBMITTED,
        JOB_FINISHED,
        JOB_MAX
    };

    class KnapSackProblems {
        public:
            KnapSackProblems():_submit(0),_finish(0),_totalValue(0),
            _jobEnumStatus(JobStatus::JOB_MAX), _jobId(0) {
            }

            ~KnapSackProblems() {cerr << "~KnapSackProblems called" <<endl;}
            vector<int64> &capacity() {return _capacities;}
            vector<vector<int64>>  &weights() {return _weights;}
            vector<int64> &values() {return _values;}
            vector<int> &packedItems() {return _packetItems;}
            void setComputedValue(int64_t value) {_totalValue = value;}
            string getJobStatusStr();
            void processJob();
            void setProblem(JsonBox::Value &json) {_json = json;}
            void setStatus(JobStatus x);
            void setJobId(uint64_t x) {_jobId = x;};
            JobStatus getEnumStatus() {return _jobEnumStatus;};

        private:
            void timeSinceEpoch(uint64_t &time) {
                system_clock::time_point tp = system_clock::now();
                time = tp.time_since_epoch().count()* system_clock::period::num / system_clock::period::den;
            }
            vector<int64> _capacities;
            vector<vector<int64>> _weights;
            vector<int64> _values;
            JsonBox::Value _json;
            JsonBox::Value _solution;
            uint64_t _start;
            uint64_t _submit;
            uint64_t _finish;
            //Solution matrices
            int64_t _totalValue;
            JobStatus _jobEnumStatus;
            string _jobStatus;
            uint64_t _jobId;
            vector<int> _packetItems;
    };

    //Job Id counter
    uint64_t _startId = 1001;
    uint64_t _endId = 50000;
    uint64_t _jobId = _startId;

    //Keep a job map
    unordered_map<uint64_t, KnapSackProblems*> _jobs;

    void handleJobSubmission(KnapSackProblems* job) {
        if(_jobId >= _endId) {
            _jobId = _startId;
            if(_jobs.count(_jobId)>0) {
		KnapSackProblems* temp =  _jobs[_jobId];
                _jobs.erase(_jobId);
		delete temp;
		temp = NULL;
                job->setJobId(_jobId);
                _jobs.insert((pair<uint64_t, KnapSackProblems*>(_jobId++, job)));
                job->setStatus(JobStatus::JOB_SUBMITTED);
            } else {
                //cerr <<"first time insertion in the map:" <<_jobId <<endl;
                job->setJobId(_jobId);
                _jobs.insert((pair<uint64_t, KnapSackProblems*>(_jobId++, job)));
                job->setStatus(JobStatus::JOB_SUBMITTED);
            }

        } else {
            //cerr <<"first time insertion in the map:" <<_jobId <<endl;
            job->setJobId(_jobId);
            _jobs.insert((pair<uint64_t, KnapSackProblems*>(_jobId++, job)));
            job->setStatus(JobStatus::JOB_SUBMITTED);
        }
    }
}


namespace operations_research {

    void KnapSackSolution(vector<int64> &capacities, vector<std::vector<int64>> &weights, vector<int64> &values, KnapSackProblems* problem) {
        // Instantiate the solver.
        KnapsackSolver solver(
                KnapsackSolver::KNAPSACK_MULTIDIMENSION_BRANCH_AND_BOUND_SOLVER,
                "KnapsackExample");

        solver.Init(values, weights, capacities);
        int64 computed_value = solver.Solve();

        problem->setComputedValue(computed_value);

        for (std::size_t i = 0; i < values.size(); ++i) {
            if (solver.BestSolutionContains(i)) problem->packedItems().push_back(i);
        }
    }
}

void KnapSackProblems::setStatus(JobStatus x) {
    _jobEnumStatus = x;
    switch(x) {
        case JOB_STARTED:
            _jobStatus = "started";
            timeSinceEpoch(_start);
            break;
        case JOB_SUBMITTED:
            _jobStatus = "submitted";
            timeSinceEpoch(_submit);
            break;
        case JOB_FINISHED:
            _jobStatus = "completed";
            timeSinceEpoch(_finish);
            break;
        default:
            _jobStatus = "unknown";
            break;
    }
}

void KnapSackProblems::processJob() {
    setStatus(JobStatus::JOB_STARTED);
    operations_research::KnapSackSolution(_capacities, _weights, _values, this);
    setStatus(JobStatus::JOB_FINISHED);
}

string KnapSackProblems::getJobStatusStr() {
    stringstream stream;

    if(_jobEnumStatus == JOB_MAX) {
        JsonBox::Value rsp;
        rsp["task"] = JsonBox::Value("{}");
        rsp.writeToStream(stream);
        return stream.str();
    }

    JsonBox::Object time;
    if(_submit > 0){
        time["submitted"] = JsonBox::Value(::to_string(_submit));
    } else {
        time["submitted"] = JsonBox::Value("null");
    }
    if(_start > 0){
        time["started"] = JsonBox::Value(::to_string(_start));
    } else {
        time["started"] = JsonBox::Value("null");
    }

    if(_finish > 0){
        time["completed"] = JsonBox::Value(::to_string(_finish));
    } else {
        time["completed"] = JsonBox::Value("null");
    }

    JsonBox::Array a1;
    for(auto x:_packetItems) {
        a1.push_back(x);
    }

    JsonBox::Object solution;
    solution["packed_items"] = a1;
    solution["total_value"] = JsonBox::Value(::to_string(_totalValue));

    JsonBox::Value rsp;
    rsp["task"] = JsonBox::Value(::to_string(_jobId));

    rsp["status"] = JsonBox::Value(_jobStatus);

    rsp["timestamps"] = time;
    rsp["problem"] = _json["problem"];

    if(_jobEnumStatus != JOB_FINISHED) {
        rsp["solution"] = JsonBox::Value("{}");
    } else {
        rsp["solution"] = solution;
    }

    rsp.writeToStream(stream);
    return stream.str();
}

void get_method_handler( const shared_ptr< Session > session )
{
    const auto request = session->get_request( );
    const auto directories = String::split( request->get_path( ), '/' );
    uint64_t jobId = 0;
    std::string::size_type sz;
    for(auto x:directories) {
        if(x != "knapsack") {
            jobId =
                jobId = ::stoi (x,&sz);
            //     cerr<<"jobID:"<<jobId<<"\t";
        }
    }
    KnapSackProblems* temp = NULL;
    unordered_map<uint64_t, KnapSackProblems*>::iterator it;
    it =  _jobs.find(jobId);
    if(it != _jobs.end()) {
        temp = it->second;
        string response_body = temp->getJobStatusStr();
        session->close(OK, response_body, { { "Content-Length", std::to_string(response_body.length()) }, {"Content-Type", "application/json"} });
    } else {
        KnapSackProblems temp;
        string response_body = temp.getJobStatusStr();
        session->close(OK, response_body, { { "Content-Length", std::to_string(response_body.length()) }, {"Content-Type", "application/json"} });
    }
}

void post_method_handler( const shared_ptr<Session> session ) {
    const auto request = session->get_request( );

    int content_length = request->get_header( "Content-Length", 0 );
    session->fetch(content_length, [ ](const shared_ptr< Session >& session, const Bytes& body) {

            JsonBox::Value json;
            json.loadFromString(string(body.begin(), body.end()));

            KnapSackProblems *problem = new KnapSackProblems();
            if(json["problem"]["capacity"].isInteger() && json["problem"]["capacity"].getInteger() > 0) {
            int capacity = json["problem"]["capacity"].getInteger();
            problem->capacity().push_back(capacity);
            } else {
            string result = "Wrong capacity Value";
            session->close( OK, result, { { "Content-Length", ::to_string(result.length()) }, { "Connection", "close" } } );
            }

            if(json["problem"]["values"].isArray()) {
            JsonBox::Array arr = json["problem"]["values"].getArray();
            for(auto x:arr) {
            if(x.isInteger()) {
            problem->values().push_back(x.getInteger());
            }
            }
            }

            if(json["problem"]["weights"].isArray()) {
                JsonBox::Array arr = json["problem"]["weights"].getArray();
                vector<int64> weight;
                for(auto x:arr) {
                    if(x.isInteger()) {
                        weight.push_back(x.getInteger());
                    }
                  //  cerr << x.getInteger() << "\t";
                }
                problem->weights().push_back(weight);
            }
            problem->setProblem(json);
            handleJobSubmission(problem);
            //this_thread::sleep_for(chrono::milliseconds(100));
            string response_body = problem->getJobStatusStr();
            session->close(OK, response_body, { { "Content-Length", std::to_string(response_body.length()) }, {"Content-Type", "application/json"} });
    });
}

void service_ready_handler( Service& ) {
    cerr<<"Hey! The service is up and running." <<endl;
}

void executeJobs() {
    while(1) {
        for(auto x:_jobs) {
            if(x.second->getEnumStatus() == JOB_SUBMITTED) {
                (x.second)->processJob();
                //cerr <<"Post Job executed:" <<endl;
            }
        }
        //sleep for some time to avoid looping
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

void apiListener() {
    auto postResource = make_shared< Resource >();
    postResource->set_paths( { "/knapsack", "/knapsack/{id: [0-9]*}" } );
    postResource->set_method_handler("POST", post_method_handler);
    postResource->set_method_handler("GET", get_method_handler);

    auto settings = make_shared<Settings>();
    settings->set_port(6543);

    auto service = make_shared<Service>();
    service->publish(postResource);
    service->set_ready_handler(service_ready_handler);
    service->start(settings);
}

int main(int argc, char **argv) {

    //Create a Listener Thread
    thread listener(apiListener);

    //Create a job execution Thread
    thread jobs(executeJobs);

    listener.join();
    jobs.join();
    return EXIT_SUCCESS;
}
