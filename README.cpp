
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/traffic-control-module.h"
using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("Lab-Program-1");
int main (int argc, char *argv[])
{
std::string socketType= "ns3::TcpSocketFactory";;
CommandLine cmd;
cmd.Parse (argc, argv);
Time::SetResolution (Time::NS);
LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
NodeContainer nodes;
nodes.Create (4); //4 point-to-point nodes are created
InternetStackHelper stack;
stack.Install (nodes);
//TCP-IP layer functionality configured on all nodes
//Bandwidth and delay set for the point-to-point channel. Vary these parameters
//to see the variation in number of packets sent/received/dropped.
PointToPointHelper p2p1;
p2p1.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
p2p1.SetChannelAttribute ("Delay", StringValue ("1ms"));
//Set the base address for the first network (nodes n0 and n1)
Ipv4AddressHelper address;
address.SetBase ("10.1.1.0", "255.255.255.0");

NetDeviceContainer devices;
devices = p2p1.Install (nodes.Get (0), nodes.Get (2));
Ipv4InterfaceContainer interfaces = address.Assign (devices);
p2p1.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
p2p1.SetChannelAttribute ("Delay", StringValue ("1ms"));
address.SetBase("10.1.2.0", "255.255.255.0");
devices = p2p1.Install (nodes.Get (2), nodes.Get (3));
interfaces = address.Assign (devices);
//UDP between n1 and n3
p2p1.SetDeviceAttribute ("DataRate", StringValue ("15Mbps"));
p2p1.SetChannelAttribute ("Delay", StringValue ("10ms"));
address.SetBase ("10.1.3.0", "255.255.255.0");
devices=p2p1.Install(nodes.Get(1),nodes.Get(2));
interfaces = address.Assign (devices);
p2p1.SetDeviceAttribute ("DataRate", StringValue ("15Mbps"));
p2p1.SetChannelAttribute ("Delay", StringValue ("10ms"));
devices = p2p1.Install (nodes.Get (2), nodes.Get (3));
address.SetBase("10.1.4.0", "255.255.255.0");
interfaces = address.Assign (devices);
 UdpEchoServerHelper echoServer (9);
ApplicationContainer serverApps = echoServer.Install (nodes.Get (3));
 serverApps.Start (Seconds (0.0));

 serverApps.Stop (Seconds (30.0));
 UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
 echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
 echoClient.SetAttribute ("Interval", TimeValue (Seconds (2.0)));
 echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
 ApplicationContainer clientApps = echoClient.Install (nodes.Get (1));
 clientApps.Start (Seconds (1.0));
 clientApps.Stop (Seconds (30.0));
//RateErrorModel allows us to introduce errors into a Channel at a given rate.
//Vary the error rate value to see the variation in number of packets dropped.
Ptr<RateErrorModel>em = CreateObject<RateErrorModel> ();
em –›SetAttribute ("ErrorRate", DoubleValue (0.00001));
devices.Get (1) –›SetAttribute ("ReceiveErrorModel", PointerValue (em));
//create routing table at all nodes
Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
uint32_t payloadSize = 1448;
OnOffHelper onoff (socketType, Ipv4Address::GetAny ());
//Generate traffic by using On Off application.
onoff.SetAttribute ("OnTime",
StringValue("ns3::ConstantRandomVariable[Constant=1]"));
onoff.SetAttribute ("OffTime",
StringValue("ns3::ConstantRandomVariable[Constant=0]"));
onoff.SetAttribute ("PacketSize", UintegerValue (payloadSize));
onoff.SetAttribute ("DataRate", StringValue ("50Mbps")); //bit/s
uint16_t port = 7;
//Install receiver (for packet sink) on node 2
Address localAddress1 (InetSocketAddress (Ipv4Address::GetAny (), port));
PacketSinkHelper packetSinkHelper1 (socketType, localAddress1);

ApplicationContainer sinkApp1 = packetSinkHelper1.Install (nodes.Get (3));
sinkApp1.Start (Seconds (0.0));
sinkApp1.Stop (Seconds (10));
//Install sender app on node 0
ApplicationContainer apps;
AddressValue remoteAddress (InetSocketAddress (interfaces.GetAddress (1),
port));
onoff.SetAttribute ("Remote", remoteAddress);
apps.Add (onoff.Install (nodes.Get (0)));
apps.Start (Seconds (4.0));
apps.Stop (Seconds (10));
Simulator::Stop (Seconds (30));
AsciiTraceHelper ascii;
p2p1.EnableAsciiAll (ascii.CreateFileStream ("lab1.tr"));
//Run the simulator
Simulator::Run ();
Simulator::Destroy ();
return 0;
}
