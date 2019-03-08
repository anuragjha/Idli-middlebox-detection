//cs621 - Idli - P1
#include "build/ns3/core-module.h"
#include "build/ns3/network-module.h"
#include "build/ns3/internet-module.h"
#include "build/ns3/point-to-point-module.h"
#include "build/ns3/applications-module.h"
#include <unistd.h>
#include <string>
#include <random>
#include <fstream>
#include <sstream> 
#include <stdlib.h>


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("cs621P1Idli");

void
generateRandomPayloadFile(void);

void
makeSimulation(std::string, std::string, bool, int);


int
main(int argc, char *argv[])
{
	Time::SetResolution(Time::NS);
    
    CommandLine cmd;
    std::string cfPath; 
    cmd.AddValue("cfPath", "config file path", cfPath);
    cmd.Parse (argc, argv);
    std::cout<<"config file:"<<cfPath<<std::endl;



	std::cout << "generating random data file..."<< "endl";
	generateRandomPayloadFile();
	
 //idli
	bool compression;
	for(int k = 1; k <= 2; k++) { // hack for changing compression value
		compression = false;
		if(k == 2) {
			compression = true;	
		}
		
		for(int i = 1; i<=10; i++)	{ //datarate variation
			//for(int j = 1; j<=2 ; j++) { //low entropy high entropy // hack in udp-server
				//creating high entropy payload file
			//	if(j==2) {
					// UdpClient::isHighEntropy = true;
					//generateRandomPayloadFile();
		
			//	} else {
			//		 UdpClient::isHighEntropy = false;
			//	}
				makeSimulation("EXP_"+std::to_string(i)+"_", std::string (std::to_string(i)+"Mbps"), compression, i);
			//}
			//sleep(1); //
		}
	}

	return 0;
}

void
makeSimulation(std::string pcapPrefix, std::string routersdataRate, bool compression, int round)
{
	//std::string configFile;
	//if (argc == 1) {
	//	configFile = argv[0];
	//} else {
	//	std::cout<<"Too many params";
	//	exit(EXIT_FAILURE);
	//	
	//}

	std::cout<<"cs621 Idli P1\n\n";
    //Time::SetResolution(Time::NS);
    LogComponentEnable ("PointToPointNetDevice", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);
//idli
Packet::EnablePrinting();
//Packet::EnableChecking();
//idli


    //creating 4 nodes
    NodeContainer nodes;
    nodes.Create(4);

    //internetstackhelper to install protocols(tcp,udp,ip,etc)
    InternetStackHelper stack;
    stack.Install (nodes);

    //creating pointtopoint helper - net device and channel
    //real world corresponds to ethernet card and network cables
    PointToPointHelper pointToPoint;

    pointToPoint.SetDeviceAttribute("DataRate", StringValue ("8Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue ("0.1ms"));
    //create netDeviceContainer - makes use of pointtopoint helper
    NetDeviceContainer ndc01 = pointToPoint.Install (nodes.Get (0), nodes.Get (1));

    pointToPoint.SetDeviceAttribute("DataRate", StringValue (routersdataRate));
    pointToPoint.SetChannelAttribute("Delay", StringValue ("0.1ms"));
    NetDeviceContainer ndc12 = pointToPoint.Install (nodes.Get (1), nodes.Get (2));

   	pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("8Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("0.1ms"));
    NetDeviceContainer ndc23 = pointToPoint.Install (nodes.Get (2), nodes.Get (3));

	//idli
	//Ptr<NetDevice> n = ndc12.Get(0);
        //Ptr<PointToPointNetDevice> pppNetDevice = n->GetObject<PointToPointNetDevice> ();
	Ptr<PointToPointNetDevice> ppp1 = DynamicCast<PointToPointNetDevice>(ndc12.Get (0));

	Ptr<PointToPointNetDevice> ppp2 = DynamicCast<PointToPointNetDevice> (ndc12.Get (1));
	if(compression == true) {
		ppp1->EnableCompression();	
		ppp2->EnableDecompression();
		
	}
	bool compress = ppp1->GetCompression();
	bool decompress = ppp2->GetDecompression();
	std::cout<<"Compress:"<<compress<<std::endl;
	std::cout<<"Decompress:"<<decompress<<std::endl;
	
	//idli
    //use ipv4addresshelper for allocation of ip address
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");


    //use ipv4interfacecontainer to associate netdevice and ipaddress
    Ipv4InterfaceContainer ifc01 = address.Assign(ndc01);
    Ipv4InterfaceContainer ifc12 = address.Assign(ndc12);
    Ipv4InterfaceContainer ifc23 = address.Assign(ndc23);

    // Create router nodes, initialize routing database and set up the routing
   // tables in the nodes.
   Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    //// now at this point we have network built,
    ////  at this point now we need applications to generate traffic

//////////////////////////////////////////////////
     	NS_LOG_INFO ("Create Application 1");

	

	//
	// Create one udpServer applications on node one.
	//
  	uint16_t port = 9;
  	UdpServerHelper server (port);
  	ApplicationContainer serverApps = server.Install (nodes.Get (3));

  	serverApps.Start (Seconds (1.0));
  	serverApps.Stop (Seconds (100.0));

	uint32_t MaxPacketSize = 1150;//5+7; //config file
   	Time interPacketInterval = Seconds (0.001); //config file
   	uint32_t maxPacketCount = 12000; //config file
 	UdpClientHelper client (ifc23.GetAddress(1), port);

 	client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
    	client.SetAttribute ("Interval", TimeValue (interPacketInterval));
  	client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  	ApplicationContainer clientApps = client.Install (nodes.Get (0));
	


  	clientApps.Start (Seconds (5.0));
  	clientApps.Stop (Seconds (100.0));
//////////////////////////////////////////////////
	//sleep(5);
	//NS_LOG_INFO ("Create Application 2");

  	//ApplicationContainer clientApps1 = client.Install (nodes.Get (0));
  	//clientApps1.Start (Seconds (4.0));
  	//clientApps1.Stop (Seconds (100.0));
//////////////////////////////////////////////////////////////////////

	//ascii and pcap generate
	AsciiTraceHelper ascii;
	pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("p1_"+pcapPrefix+".tr"));
	pointToPoint.EnablePcapAll ("p1"+pcapPrefix+"_dr_"+std::to_string(round)+"-");


    	//start and then destroy simulator
	Simulator::Run();
	Simulator::Destroy ();


	/**/
	//return 0;
}

void
generateRandomPayloadFile() {
std::uniform_int_distribution<int> d(0, 1);
        
        //std::stringstream strs;
        //strs.str( std::string() );
	//strs.clear();
	//int intStream[5];
	
	  std::stringstream s;
  
  //std::string header = s.str();

	std::ofstream randomPayloadFile;
  	randomPayloadFile.open ("randomPayload.txt");
  
        std::random_device rd2("/dev/random"); 
	for(int pc = 0; pc < 6000*1100 + 5; pc++) {
        	//for(int n = 0; n < 10; n++) {
                	//std::cout << d(rd2) << ' ';
                	//strs << d(rd2);
                	//intStream[i] = d(rd2);
                	s << d(rd2);
			//str << d(rd2);
        	//}
		  	
	}
	randomPayloadFile << s.str();
	s.str("");
	//strs.str( std::string() );
	//strs.clear();

  	randomPayloadFile.close();

}
