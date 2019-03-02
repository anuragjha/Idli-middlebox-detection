/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <fstream>
#include <iostream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"
#include "zlib.h"
#include "ns3/ipv4-flow-classifier.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("UdpExample2");

int 
main (int argc, char *argv[]) 
{

        CommandLine cmd;
        cmd.Parse (argc, argv);
  
        Time::SetResolution (Time::NS);

        LogComponentEnable ("PointToPointNetDevice1", LOG_LEVEL_INFO);
        LogComponentEnable ("UdpClient1", LOG_LEVEL_INFO);
        LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);

        NodeContainer hosts;

        hosts.Create(4);

        InternetStackHelper stack;
        stack.Install(hosts);

        PointToPointHelper p2p;

        Ipv4AddressHelper address;

        NodeContainer n1;
        n1.Add(hosts.Get(0));
        n1.Add(hosts.Get(1));

        NodeContainer n2;
        n2.Add(hosts.Get(1));
        n2.Add(hosts.Get(2));
 
        NodeContainer n3;
        n3.Add(hosts.Get(2));
        n3.Add(hosts.Get(3));

        p2p.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
        p2p.SetChannelAttribute ("Delay", StringValue ("1000ms"));
        NetDeviceContainer subnet1Devices = p2p.Install(n1);
        NetDeviceContainer subnet2Devices = p2p.Install(n2);
        NetDeviceContainer subnet3Devices = p2p.Install(n3);

        //Ptr<NetDevice> n = subnet2Devices.Get(0);
        //Ptr<PointToPointNetDevice> pppNetDevice = n->GetObject<PointToPointNetDevice> ();
        //Ptr<Queue<Packet>> q = pppNetDevice->GetQueue();
        //Ptr<Packet> p = q->Dequeue ();

        //uint32_t qSize=q->GetCurrentSize ().GetValue ();
        //std::cout<<"Queue size of is "<<qSize<<" at"<<Simulator::Now().GetSeconds()<<std::endl;
        //std::cout<<"packet p is "<<p<<" at"<<Simulator::Now().GetSeconds()<<std::endl;
        
        address.SetBase("10.1.1.0","255.255.255.0");

        Ipv4InterfaceContainer subnet1Interfaces = address.Assign(subnet1Devices);

        address.SetBase("10.1.2.0","255.255.255.0");

        Ipv4InterfaceContainer subnet2Interfaces = address.Assign(subnet2Devices);

        address.SetBase("10.1.3.0","255.255.255.0");

        Ipv4InterfaceContainer subnet3Interfaces = address.Assign(subnet3Devices);

        Ipv4GlobalRoutingHelper::PopulateRoutingTables();

        UdpServerHelper server(9);
        ApplicationContainer serverApp;
        serverApp = server.Install(hosts.Get(3));
        serverApp.Start(Seconds(1.0));
        serverApp.Stop(Seconds(10.0));

        uint32_t packetSize = 1100;
        uint32_t maxPacketCount = 100;
        Time interPacketInterval = Seconds (1.);
        UdpClientHelper client(subnet3Interfaces.GetAddress(1),9);
        ApplicationContainer clientApp;
        client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
        client.SetAttribute ("Interval", TimeValue (interPacketInterval));
        client.SetAttribute ("PacketSize", UintegerValue (packetSize));
        clientApp=client.Install(hosts.Get(0)); 
        clientApp.Start(Seconds(2.0));
        clientApp.Stop(Seconds(9.0));  
        Simulator::Run();
        Simulator::Destroy();
        //NS_LOG_INFO ("Done.");
        return 0;
}
