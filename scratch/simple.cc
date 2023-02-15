#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/mpi-interface.h"
#include "ns3/applications-module.h"

namespace ns3 {

int
main(int argc, char* argv[])
{
  // setting default parameters for PointToPoint links and channels
  Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Gbps"));
  Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("1ms"));
  Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("10"));

  bool nullmsg = false;

  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
  CommandLine cmd;
  cmd.AddValue("nullmsg", "Enable the use of null-message synchronization", nullmsg);
  cmd.Parse(argc, argv);

  // Distributed simulation setup; by default use granted time window algorithm.
  if (nullmsg) {
    GlobalValue::Bind("SimulatorImplementationType",
                      StringValue("ns3::NullMessageSimulatorImpl"));
  }
  else {
    GlobalValue::Bind("SimulatorImplementationType",
                      StringValue("ns3::DistributedSimulatorImpl"));
  }

  // Enable parallel simulator with the command line arguments
  MpiInterface::Enable(&argc, &argv);

  uint32_t systemId = MpiInterface::GetSystemId();
  uint32_t systemCount = MpiInterface::GetSize();

  if (systemCount != 2)  {
    std::cout << "Simulation will run on a single processor only" << std::endl
              << "To run using MPI, run" << std::endl
              << "  mpirun -np 2 ./waf --run=ndn-simple-mpi" << std::endl;
  }

  // Creating nodes

  // consumer node is associated with system id 0
  Ptr<Node> node1 = CreateObject<Node>(0);

  // producer node is associated with system id 1 (or 0 when running on single CPU)
  Ptr<Node> node2 = CreateObject<Node>(systemCount == 2 ? 1 : 0);

  // Connecting nodes using a link
  PointToPointHelper p2p;
  NetDeviceContainer devices;
  devices = p2p.Install(node1, node2);

  InternetStackHelper stack;
  stack.InstallAll();

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  if(systemId == 1){
    UdpEchoServerHelper echoServer (9);

    ApplicationContainer serverApps = echoServer.Install (node2);
    serverApps.Start (Seconds (1.0));
    serverApps.Stop (Seconds (1000000000.0));
  }
  if(systemId == 0){
    UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
    echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
    echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
    echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

    ApplicationContainer clientApps = echoClient.Install (node1);
    clientApps.Start (Seconds (2.0));
    clientApps.Stop (Seconds (1000000000.0));
  }
  //Simulator::Stop(Seconds(400.0));
  std::cout << "Running Simulation.\n";
  Simulator::Run();
  Simulator::Destroy();

  MpiInterface::Disable();
  return 0;
}

} // namespace ns3


int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}