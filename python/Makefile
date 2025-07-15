GENSRCS=geoinfo_pb2.py geoinfo_pb2.pyi geoinfo_pb2_grpc.py
$(GENSRCS): ../java/src/main/proto/geoinfo.proto
	python -m grpc_tools.protoc -I../java/src/main/proto --python_out=. --pyi_out=. \
       --grpc_python_out=. $<

clean:
	$(RM) $(GENSRCS)
