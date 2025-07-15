import logging

import grpc
import geoinfo_pb2
import geoinfo_pb2_grpc


def run():
    with grpc.insecure_channel("localhost:50051") as channel:
        stub = geoinfo_pb2_grpc.CityFinderStub(channel)
        city = stub.FindByName(geoinfo_pb2.CityName(name="Basel"))
        print(f"CityFinder client received:\n{city}")

        dist = 60
        print(f"Cities within distance {dist} km of {city.name}, {city.country}")
        for i, cd in enumerate(stub.FindByLocationNear(
                geoinfo_pb2.CityDist(city=city, dist=dist))):
            print(f"{i} {cd.city.name:20} {cd.dist:.1f}")

if __name__ == "__main__":
    logging.basicConfig()
    run()
