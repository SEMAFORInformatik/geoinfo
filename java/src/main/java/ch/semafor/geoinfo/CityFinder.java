package ch.semafor.geoinfo;

import ch.semafor.geoinfo.CityFinderGrpc.CityFinderBlockingStub;
import io.grpc.Channel;
import io.grpc.Grpc;
import io.grpc.InsecureChannelCredentials;
import io.grpc.LoadBalancerRegistry;
import io.grpc.ManagedChannel;
import java.util.Iterator;

public class CityFinder {
	  private final CityFinderBlockingStub blockingStub;

	  /** Construct client for accessing Geoinfo server using the existing channel. */
	  public CityFinder(Channel channel) {
	    blockingStub = CityFinderGrpc.newBlockingStub(channel);
	  }
	  public void listNeighbours(String cityName, Double dist) {
		  CityName request = CityName.newBuilder().setName(cityName).build();
		  City city = blockingStub.findByName(request);
		  System.out.printf("Cities within distance %5.2f km of %s, %s:%n",
				  dist, city.getName(), city.getCountry());
		  CityDist cd = CityDist.newBuilder().setCity(city).setDist(dist).build();
		  Iterator<CityDist> citydists = blockingStub.findByLocationNear(cd);
		  for (int i = 1; citydists.hasNext(); i++) {
		        CityDist citydist = citydists.next();
		        System.out.printf("%d %-15s %8.2f%n", i, citydist.getCity().getName(), citydist.getDist());
		  }
	  }

	public static void main(String[] args) {
	    String target = "localhost:50051";
	    if (args.length > 0) {
	      if ("--help".equals(args[0])) {
	        System.err.println("Usage: [target]");
	        System.err.println("");
	        System.err.println("  target  The server to connect to. Defaults to " + target);
	        System.exit(1);
	      }
	      target = args[0];
	    }
	    ManagedChannel channel = Grpc.newChannelBuilder(
	    		target, InsecureChannelCredentials.create())
	            .build();
	    CityFinder cityFinder = new CityFinder(channel);
	    cityFinder.listNeighbours("Basel", 60.0);
	}
}
