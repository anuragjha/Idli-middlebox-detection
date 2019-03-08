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
#include <istream>
#include <sstream> 
#include <stdlib.h>
#include <tuple>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("cs621P1Idli");

void
generateRandomPayloadFile(void);

void
makeSimulation(std::string, std::string, bool, int);

std::tuple<int, int, int, std::string>
readConfigParamaters(std::string cfFileName){
	std::ifstream cfFile (cfFileName);
	if(cfFile.is_open()){
		std::stringstream cfStream;
		cfStream << cfFile.rdbuf();
		std::string cfString = cfStream.str();
		std::istringstream iss(cfString);
		std::vector<std::string> results((std::istream_iterator<std::string>(iss)),
                                 std::istream_iterator<std::string>());
    	cfFile.close();
    	
    	return std::make_tuple(atoi(results[0].c_str()), atoi(results[1].c_str()), atoi(results[2].c_str()), results[3] );		
	}else {
		std::cout << "Unable to open file" << std::endl;
		return std::make_tuple(-1, -1, -1, "0x0021");
	}
}


void
generateRandomPayloadFile() {
	std::uniform_int_distribution<int> d(0, 1);
    std::stringstream s;

	std::ofstream randomPayloadFile;
     
    std::cout<<"Generating random bits and outputing to file" << std::endl;
  	randomPayloadFile.open ("randomPayload.txt");
  
    std::random_device rd2("/dev/random"); 
	for(int pc = 0; pc < 6000*1100 + 5; pc++) {
        s << d(rd2);
	}
	randomPayloadFile << s.str();
	s.str("");
  	randomPayloadFile.close();
  	std::cout<<"Random bits generatio complete" << std::endl;
}


int
main(int argc, char *argv[])
{
	Time::SetResolution(Time::NS);
    
    CommandLine cmd;
    std::string cfFileName; 
    cmd.AddValue("cfFileName", "config file name", cfFileName);
    cmd.Parse (argc, argv);
    if(cfFileName.empty()){
    	std::cout<<"Config file not given: command is ./waf --run 'cs621P1Idli --cfFileName=config.txt'" << std::endl;
    }else{
    	std::cout<<"Reading config file" << std::endl;
    	std::tuple<int, int, int, std::string> configParameters = readConfigParamaters(cfFileName);
    	if(std::get<0>(configParameters)!= -1){
    		bool compressionFlag;
    		if(std::get<0>(configParameters)==1){
    			compressionFlag = true; 
    		}else{
    			compressionFlag = false;
    		}

    		int minDataRate = std::get<1>(configParameters);
    		int maxDataRate = std::get<2>(configParameters); 
    		std::string protocol = std::get<3>(configParameters);
    		std::cout<<"Config Parameters" << std::endl;
    		std::cout<<"Compression:" << compressionFlag << std::endl;
    		std::cout<<"minDataRate:" << minDataRate << std::endl;
    		std::cout<<"maxDataRate:" << maxDataRate << std::endl;
    		std::cout<<"protocol:" << protocol << std::endl;

    		generateRandomPayloadFile();	
    		
    		// Initiate simulation with 30 second interval between two packet trains
    		
			for(int i = 1; i<=10; i++)	{
				makeSimulation("EXP_"+std::to_string(i)+"_", std::string (std::to_string(i)+"Mbps"), compressionFlag, i);
			}
    	}		
    }
 	return 0;
}

void
makeSimulation(std::string pcapPrefix, std::string routersdataRate, bool compression, int round){
	std::cout<<"cs621 Idli P1\n\n";
    //Time::SetResolution(Time::NS);
    LogComponentEnable ("PointToPointNetDevice", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
    LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);

	Packet::EnablePrinting();

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

	//ascii and pcap generate
	AsciiTraceHelper ascii;
	pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("p1_"+pcapPrefix+".tr"));
	pointToPoint.EnablePcapAll ("p1"+pcapPrefix+"_dr_"+std::to_string(round)+"-");


	//start and then destroy simulator
	Simulator::Run();
	Simulator::Destroy ();
}


