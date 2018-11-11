using System.Net; // For IPAddress
using System.Net.Sockets; // For TcpListener, TcpClient

namespace EchoServerTrackFever
{
    public class UdpState
    {
	    public IPEndPoint e;
	    public UdpClient u;
    }
}
