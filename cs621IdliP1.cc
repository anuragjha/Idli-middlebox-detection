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

NS_LOG_COMPONENT_DEFINE ("cs621P1Idli - NS3 Compression Detection Application");

void
generateRandomPayloadFile(void);

Time
makeSimulation(std::string, std::string, bool, bool, int);

void writeToFile(int, int64_t);


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
	std::cout<<"Random bits generation complete" << std::endl;
}


int
main(int argc, char *argv[])
{

	std::cout<<"cs621 Idli P1\n\n";
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
			std::cout<<"protocol:" << protocol << std::endl<< std::endl;

			//generateRandomPayloadFile();	/// remove this comment when project ready idli

			
			for(int i = minDataRate; i<=maxDataRate; i++)	{ // data rate change here

				Time lowEDT = makeSimulation("p1_", std::string (std::to_string(i)+"Mbps"), compressionFlag, false, i);
				Time highEDT = makeSimulation("p1_", std::string (std::to_string(i)+"Mbps"), compressionFlag, true, i);
				int64_t delta = highEDT.GetMilliSeconds() - lowEDT.GetMilliSeconds();
				writeToFile(i, delta);
	
				if (highEDT.GetMilliSeconds() - lowEDT.GetMilliSeconds() >= 100) {
						std::cout << "Compression detected at datarate "<< std::to_string(i)<<" Mbps, coz delta time is "<< delta <<" ms"<< "\n\n";
				} else {
						std::cout << "Not detected at datarate "<< std::to_string(i)<<" Mbps, coz delta time is "<< delta <<" ms"<< "\n\n";
				}

			}
		}		
	}
	return 0;
}


void writeToFile(int i, int64_t delta) {
		std::ofstream ofs;
		ofs.open ("Final.txt", std::ofstream::out | std::ofstream::app);

		ofs << std::to_string(i) << " "<< std::to_string(delta) <<std::endl;

		ofs.close();
}


Time
makeSimulation(std::string pcapPrefix, std::string routersdataRate, bool compressionFlag, bool highEntropy, int round){

	std::string entStr;
	std::string drStr;	
	if (highEntropy == true) {
		entStr = "High Entropy";
	} else {
		entStr = "Low Entropy";
	}


	std::cout<<"Simulation - "<<"\tData Rate : "<<std::to_string(round)<< "\t"<<entStr<<"\n";

	//LogComponentEnable ("PointToPointNetDevice", LOG_LEVEL_INFO);
	//LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
	//LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);
	//idlilate
	Config::SetDefault("ns3::QueueBase::MaxSize",StringValue("6000p"));
	//Packet::EnablePrinting();

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
	pointToPoint.SetChannelAttribute("Delay", StringValue ("0ms"));

	//create netDeviceContainer - makes use of pointtopoint helper
	NetDeviceContainer ndc01 = pointToPoint.Install (nodes.Get (0), nodes.Get (1));

	pointToPoint.SetDeviceAttribute("DataRate", StringValue (routersdataRate));
	pointToPoint.SetChannelAttribute("Delay", StringValue ("0ms"));

	NetDeviceContainer ndc12 = pointToPoint.Install (nodes.Get (1), nodes.Get (2));

	pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("8Mbps"));
	pointToPoint.SetChannelAttribute ("Delay", StringValue ("0ms"));

	NetDeviceContainer ndc23 = pointToPoint.Install (nodes.Get (2), nodes.Get (3));

	Ptr<PointToPointNetDevice> ppp1 = DynamicCast<PointToPointNetDevice>(ndc12.Get (0));

	Ptr<PointToPointNetDevice> ppp2 = DynamicCast<PointToPointNetDevice> (ndc12.Get (1));

	std::string isComp;
	if(compressionFlag == true) {
		ppp1->EnableCompression();	
		ppp2->EnableDecompression();
		isComp = "CY";
	} else {
		isComp = "CN";
	}


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
	NS_LOG_INFO ("Creating Application 1");

	//
	// Create one udpServer applications on node one.
	//
	uint16_t port = 9;
	UdpServerHelper server (port);
	ApplicationContainer serverApps = server.Install (nodes.Get (3));

	serverApps.Start (Seconds (1.0));
	serverApps.Stop (Seconds (40000.0));

	uint32_t MaxPacketSize = 1150;//5+7; //config file
	Time interPacketInterval = Seconds (0.00);
	uint32_t maxPacketCount = 6000; //config file
	UdpClientHelper client (ifc23.GetAddress(1), port);

	client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
	client.SetAttribute ("Interval", TimeValue (interPacketInterval));
	client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
	
//idli1
	std::string isHigh;
	if(highEntropy == true) {
		client.SetAttribute ("IsHighEntropy", UintegerValue (1));
		isHigh = "h";
	}else{
		client.SetAttribute ("IsHighEntropy", UintegerValue (0));
		isHigh = "l";
	}

	ApplicationContainer clientApps = client.Install (nodes.Get (0));

	clientApps.Start (Seconds (2.0));
	clientApps.Stop (Seconds (40000.0));


	pointToPoint.EnablePcapAll (pcapPrefix+isComp+"_"+"_dr_"+std::to_string(round)+"_ent_"+isHigh +"-");


	//start and then destroy simulator
	Simulator::Run();
	Simulator::Destroy ();

	return server.GetServer()->deltaTime;
}


