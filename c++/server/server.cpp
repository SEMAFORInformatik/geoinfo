#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include "geoinfo.grpc.pb.h"

#include "absl/flags/parse.h"
#include "absl/flags/flag.h"
#include "absl/log/initialize.h"
#include "absl/log/log.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using geoinfo::CityFinder;
using geoinfo::CityName;
using geoinfo::City;
using geoinfo::CityDist;

std::vector<std::string> split(std::string &str, char delim){
  std::string s;
  std::stringstream ss(str);
  std::vector<std::string> svec;
  while (getline(ss, s, delim)){
    svec.push_back(s);
  }
  return svec;
}

double distance(const City &from, const City &to){
  if((std::pow(from.lat() - to.lat(), 2) + std::pow(from.lon() - to.lon(),2)) < 1e-3)
    return 0;
  // convert coordinates into radian
  double k = M_PI/180;
  double a = from.lat()*k;
  double b = from.lon()*k;
  double c = to.lat()*k;
  double d = to.lon()*k;

  double x = sin(a)*sin(c)+
    cos(a)*cos(c)*cos(b-d);
  double radius = 6371.007176; // in km
  if (x > 1)
    return radius*acos(1.);
  return radius*acos(x);
}

bool distcomp(const CityDist *a, const CityDist *b){
  return a->dist() < b->dist();
};

class CityFinderImpl final : public CityFinder::Service {
 public:
  explicit CityFinderImpl(const std::string& db) {
    LoadDb(db);
  }
  virtual Status FindByName(ServerContext* context,
                            const CityName* request,
                            City* response){
    for(auto c: city_list){
      if(c->name() == request->name()){
        *response = *c;
        return Status::OK;
      }
    }
    return Status(::grpc::StatusCode::NOT_FOUND, "");
  }

  virtual Status FindByLocationNear(ServerContext* context,
                                    const CityDist* request,
                                    ServerWriter<CityDist>* writer){
    std::vector<CityDist *> neighbours;
    const City &loc = (request->city());
    for(auto c: city_list){
      auto d = distance(loc, *c);
      if( d > 0 && d < request->dist() ){
        CityDist *cd = new CityDist();
        cd->set_allocated_city(c);
        cd->set_dist(d);
        neighbours.push_back(cd);
      }
    }
    std::sort(neighbours.begin(), neighbours.end(), distcomp);
    for (const CityDist* n : neighbours) {
        writer->Write(*n);
    }
    return Status::OK;
  }

  void LoadDb(const std::string& db_path){
    std::string line;
    city_list.clear();

    std::ifstream inputFile(db_path);
    while(getline(inputFile, line)){
      auto recs = split(line, '\t');
      City *city = new City();
      city->set_name(recs[0]);
      city->set_country(recs[1]);
      city->set_population(std::stol(recs[2]));
      city->set_lon(std::stod(recs[3]));
      city->set_lat(std::stod(recs[4]));
      city_list.push_back(city);
    }
    LOG(INFO) << "DB parsed, loaded " << city_list.size() << " cities.";
    std::cout << "DB parsed, loaded " << city_list.size() << " cities."
              << std::endl;
  }
private:
  std::vector<City *> city_list;
};

void RunServer(const std::string& db_path) {
  std::string server_address("0.0.0.0:50051");
  CityFinderImpl service(db_path);

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();
  // Expect only arg: path/to/cities.txt.
  std::string db = argv[1];
  RunServer(db);

  return 0;
}
