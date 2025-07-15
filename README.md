Demo Project for grpc
=============================

This project demonstrates the implementation
of grpc clients and server using the programming languages:

* c++: grpc_server
* java: ch.semafor.geoinfo.CityFinder
* python: city_finder_client.py

The server loads the list of cities with
their geo locations included in the text file cities.txt
and offers 2 query methods:

  City *FindByName(const std::string &name)
  std::vector<CityDist> FindByLocationNear(const City &loc, double dist)

The proto file can be found in java/src/main/proto.

Prerequisites
-----------------

* c++: cmake grpc
* java: mvn
* python: grpcio
