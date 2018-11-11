using System; // For Console, Int32, ArgumentException,Environment
using System.Net; // For IPAddress
using System.Net.Sockets; // For TcpListener, TcpClient

namespace EchoServerTrackFever
{
    public class Message
    {
        String message;
 
        Socket playersSocket;
        IPEndPoint serverEP;
        public Message(String mess) { message = mess;}

        public void SetSocket(Socket socket)
        {
            playersSocket = socket;
        }

        public void SetEndPoint(IPEndPoint endPoint)
        {
            serverEP = endPoint;
        }

        public Socket GetSocket() { return playersSocket; }
        public String GetMessage() 
        {
            return message; 
        }
        public IPEndPoint GetEP()
        {
            return serverEP;
        }
    }
}
