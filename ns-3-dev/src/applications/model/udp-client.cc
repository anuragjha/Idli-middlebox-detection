/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007,2008,2009 INRIA, UDCAST
 *
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
 *
 * Author: Amine Ismail <amine.ismail@sophia.inria.fr>
 *                      <amine.ismail@udcast.com>
 */
#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "udp-client.h"
#include "seq-ts-header.h"
#include <cstdlib>
#include <cstdio>
#include <iostream> 
#include <sstream>  // for string streams 
#include <string>
#include <unistd.h> 
#include <fstream>
#include <cstdlib>
#include <random>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("UdpClient");

NS_OBJECT_ENSURE_REGISTERED (UdpClient);

TypeId
UdpClient::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UdpClient")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<UdpClient> ()
    .AddAttribute ("MaxPackets",
                   "The maximum number of packets the application will send",
                   UintegerValue (100),
                   MakeUintegerAccessor (&UdpClient::m_count),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("Interval",
                   "The time to wait between packets", TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&UdpClient::m_interval),
                   MakeTimeChecker ())
    .AddAttribute ("RemoteAddress",
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&UdpClient::m_peerAddress),
                   MakeAddressChecker ())
    .AddAttribute ("RemotePort", "The destination port of the outbound packets",
                   UintegerValue (100),
                   MakeUintegerAccessor (&UdpClient::m_peerPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PacketSize",
                   "Size of packets generated. The minimum packet size is 12 bytes which is the size of the header carrying the sequence number and the time stamp.",
                   UintegerValue (1024),
                   MakeUintegerAccessor (&UdpClient::m_size),
                   MakeUintegerChecker<uint32_t> (12,65507))
  ;
  return tid;
}

UdpClient::UdpClient ()
{
  NS_LOG_FUNCTION (this);
  m_sent = 0;
  m_socket = 0;
  m_sendEvent = EventId ();
}

UdpClient::~UdpClient ()
{
  NS_LOG_FUNCTION (this);
}

void
UdpClient::SetRemote (Address ip, uint16_t port)
{
  NS_LOG_FUNCTION (this << ip << port);
  m_peerAddress = ip;
  m_peerPort = port;
}

void
UdpClient::SetRemote (Address addr)
{
  NS_LOG_FUNCTION (this << addr);
  m_peerAddress = addr;
}

void
UdpClient::setIsHighEntropy (bool he){
        isHighEntropy = he;
}

void
UdpClient::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void
UdpClient::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (m_socket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      if (Ipv4Address::IsMatchingType(m_peerAddress) == true)
        {
          if (m_socket->Bind () == -1)
            {
              NS_FATAL_ERROR ("Failed to bind socket");
            }
          m_socket->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_peerAddress), m_peerPort));
        }
      else if (Ipv6Address::IsMatchingType(m_peerAddress) == true)
        {
          if (m_socket->Bind6 () == -1)
            {
              NS_FATAL_ERROR ("Failed to bind socket");
            }
          m_socket->Connect (Inet6SocketAddress (Ipv6Address::ConvertFrom(m_peerAddress), m_peerPort));
        }
      else if (InetSocketAddress::IsMatchingType (m_peerAddress) == true)
        {
          if (m_socket->Bind () == -1)
            {
              NS_FATAL_ERROR ("Failed to bind socket");
            }
          m_socket->Connect (m_peerAddress);
        }
      else if (Inet6SocketAddress::IsMatchingType (m_peerAddress) == true)
        {
          if (m_socket->Bind6 () == -1)
            {
              NS_FATAL_ERROR ("Failed to bind socket");
            }
          m_socket->Connect (m_peerAddress);
        }
      else
        {
          NS_ASSERT_MSG (false, "Incompatible address type: " << m_peerAddress);
        }
    }

  m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
  m_socket->SetAllowBroadcast (true);
  m_sendEvent = Simulator::Schedule (Seconds (0.0), &UdpClient::Send, this);
}

void
UdpClient::StopApplication (void)
{
  NS_LOG_FUNCTION (this);
  Simulator::Cancel (m_sendEvent);
}

void
UdpClient::Send (void)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (m_sendEvent.IsExpired ());

// idli 

std::cout <<"XXXXXXXXXXXXXXXXXXXXXXX------"<<counter<<"-------XXXXXXXXXXXXXXXXXXXXXXX"<<std::endl;

  
  std::string payload;

        // if (counter == (m_count/2)+1) { //checking to reach 6000 on m_sent
         //       std::cout <<std::endl<<std::endl<< "!!! SLEEPING NOW !!!!"<<std::endl<<std::endl;
         //     sleep(4); 
        
              //isHighEntropy = false; ///idliidliidli
            
       // }
        counter = counter +1; //increasing counter
        
  
        if(isHighEntropy == false) { //for less than 6000
                //creating all 0s payload
                for(int n = 0; n<1100; n++) { //1100 range
                        payload += std::to_string(0);      
                }
              
                //SendHelper(payload);
        } 
         //ugly hack
        if (counter > (m_count/2)+1 ) {
           if(isHighEntropy == false) {
               sleep(2);
                }
             isHighEntropy = true; 
                  
        }


        if (isHighEntropy == true) { //for greater than 600
                payload = readRandomPayload();
        
                std::cout<<"Payload created!!"<<payload.length()<<std::endl;
         
        }

        SendHelper(payload);
        
   //}
 
} //send

std::string
UdpClient::readRandomPayload() {

  std::cout<<"In readRandomPayload()";
  //int length = 1100;
  //har *str = new char;  
  char *buffer = NULL;
  unsigned int length = 1100;
  std::ifstream is ("randomPayload.txt", std::ifstream::binary);
  if (is) {
    // get length of file:
    is.seekg (payloadStartPosition, is.beg);
    payloadStartPosition += 1100;
   // int size = is.tellg();
   // is.seekg (0, is.beg);

    // allocate memory:
    buffer = new char [length];

    // read data as a block:
    is.read (buffer,length);

    is.close();
    // print content:
        std::cout<<"Buffer content:";
    std::cout.write (buffer,length);
  }
  //memset(str, '\0', length + 1);

  std::string s(buffer, length);
  delete (buffer);
  
  return s;  
}


void
UdpClient::SendHelper (std::string payload) {


SeqTsHeader seqTs;
  seqTs.SetSeq (m_sent);

 // Ptr<Packet> p = Create<Packet> (m_size-(8+4)); // 8+4 : the size of the seqTs header

Ptr<Packet> p;

          //std::cout<<"payload:"<<payload<<std::endl;
   p = Create<Packet> (reinterpret_cast<const uint8_t*> (payload.c_str()),payload.length());   
   std::cout << "Packet initial - at client side : " << std::endl<< *p<<"end" <<std::endl;


// idli


  p->AddHeader (seqTs);

  std::stringstream peerAddressStringStream;
  if (Ipv4Address::IsMatchingType (m_peerAddress))
    {
      peerAddressStringStream << Ipv4Address::ConvertFrom (m_peerAddress);
    }
  else if (Ipv6Address::IsMatchingType (m_peerAddress))
    {
      peerAddressStringStream << Ipv6Address::ConvertFrom (m_peerAddress);
    }

  if ((m_socket->Send (p)) >= 0)
    {
      ++m_sent;
      NS_LOG_INFO ("TraceDelay TX " << m_size << " bytes to "
                                    << peerAddressStringStream.str () << " Uid: "
                                    << p->GetUid () << " Time: "
                                    << (Simulator::Now ()).GetSeconds ());

    }
  else
    {
      NS_LOG_INFO ("Error while sending " << m_size << " bytes to "
                                          << peerAddressStringStream.str ());
    }

  if (m_sent < m_count)
    {
      m_sendEvent = Simulator::Schedule (m_interval, &UdpClient::Send, this);
    }













} //sendhelper

} // Namespace ns3
