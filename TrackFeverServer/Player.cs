using System.Net; // For IPAddress
using System.Net.Sockets; // For TcpListener, TcpClient

namespace EchoServerTrackFever
{
    public class Player
    {
        float posX;
        float posY;
        string playerID;
        string playerReady;
        Socket playersSocket;
        IPEndPoint playerEp;

        public Player(string id, Socket socket) { playerID = id; playersSocket = socket;}

        public void SetPos(float x, float y)
        {
            posX = x;
            posY = y;
        }
        public void SetEP(IPEndPoint ep)
        {
            playerEp = ep;
        }
        public void SetReady(string red)
        {
            playerReady = red;
        }
        public Socket GetSocket() { return playersSocket; }
        public string GetId() { return playerID; }
        public float GetPosX() { return posX; }
        public float GetPosY() { return posY; }
        public IPEndPoint GetEP() { return playerEp; }
        public string GetReady() { return playerReady; }

    }
}
