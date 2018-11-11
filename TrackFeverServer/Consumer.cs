using System;
using System.Net; // For IPAddress
using System.Net.Sockets; // For TcpListener, TcpClient
using System.Threading;
using System.Collections;
using System.Collections.Generic;

namespace EchoServerTrackFever
{
    public class Consumer
    {
        public Consumer(Queue<Message> q, SyncEvents e, PlayerDictionary d)
        {
        	_queue = q;
        	_syncEvents = e;
            _myDictionary = d;
        }
        // Consumer.ThreadRun
        public void ThreadRun()
        {
        	int count = 0;
        	while (WaitHandle.WaitAny(_syncEvents.EventArray) != 1)
        	{
        		lock (((ICollection)_queue).SyncRoot)
        		{
                    if(_queue.Count > 0)
                    {
                        Message item = _queue.Dequeue();                  

                        if (item != null)
                        {
                            Char seperator = ('/');
                            String[] subString = item.GetMessage().Split(seperator);

                            String playerCount = subString[2];

                            // Register == /Register/positon/id::
                            // Position == /Position/Vec2.x*Vec2.y*angle/id::

                            if (subString[1] == "Register")
                            {
                                //Make player and add him to dictionary
                                Player NewPlayer = new Player(positions[playerID], item.GetSocket());

                                _myDictionary.AddPlayer(NewPlayer);
                                //Counts amount of players already in game
                                int enumerator = 0;

                                enumerator = _myDictionary.GetDictionarySize();

                                playerCount = enumerator.ToString();
                                String result = subString[0] + "/" + subString[1] + "/" + playerCount + "/" + positions[playerID];

                                foreach (KeyValuePair<int, Player> players in _myDictionary.GetDictionary())
                                {
                                    //Loop through and send all clients the tcp message
                                    AsynchronousSocketListener.TCP_Send(players.Value.GetSocket(), result);

                                }
                                //Used to calculate ID
                                if (playerID < 4)
                                    playerID += 1;
                                //reset string so not sent twice
                                result = null;
                            }
                            else if (subString[1] == "Pos")
                            {
                                //Seperate up the numbers and set them in the dictionary to correct id
                                Char sep = ('*');
                                String[] splitString = subString[2].Split(sep);

                                float x = 0;
                                float y = 0;
                                float angle = 0;
                                String id = "";
                                x = float.Parse(splitString[0], System.Globalization.CultureInfo.InvariantCulture.NumberFormat);
                                y = float.Parse(splitString[1], System.Globalization.CultureInfo.InvariantCulture.NumberFormat);
                                angle = float.Parse(splitString[2], System.Globalization.CultureInfo.InvariantCulture.NumberFormat);
                                String result = "";

                                foreach (KeyValuePair<int, Player> players in _myDictionary.GetDictionary())
                                {
                                    if (players.Value.GetId() == subString[3])
                                    {
                                        players.Value.SetPos(x, y);
                                        id = players.Value.GetId();
                                        players.Value.SetEP(item.GetEP());
                                    }
                                }
                                //Make string to send the values back
                                result = subString[0] + "/" + subString[1] + "/" + x.ToString() + "*" + y.ToString() + "*" + angle.ToString() + "/" + id;

                                foreach (KeyValuePair<int, Player> players in _myDictionary.GetDictionary())
                                {
                                    IPEndPoint ep = players.Value.GetEP();
                                    //Sends to everyone with and Endpoint but not the sender
                                    if (ep != null)
                                    {
                                        if (players.Value.GetId() != id)
                                            AsynchronousSocketListener.UDP_SendMessage(players.Value.GetEP(), result);
                                    }

                                }
                                result = null;
                            }
                            else if (subString[1] == "Win")
                            {
                                //Repeats above
                                foreach (KeyValuePair<int, Player> players in _myDictionary.GetDictionary())
                                {
                                    AsynchronousSocketListener.TCP_Send(players.Value.GetSocket(), item.GetMessage());
                                }
                            }
                            else if (subString[1] == "RegEP")
                            {
                                foreach (KeyValuePair<int, Player> players in _myDictionary.GetDictionary())
                                {
                                    if (players.Value.GetId() == subString[3])
                                    {
                                        if (item.GetEP() != null)
                                        {
                                            players.Value.SetEP(item.GetEP());
                                            AsynchronousSocketListener.UDP_SendMessage(players.Value.GetEP(), item.GetMessage());                                            

                                        }
                                    }
                                }
                            }
                            item = null;
                        }
                    }
        		}
        	}
        	Console.WriteLine("Consumer Thread: consumed {0} items", count);
        }
        private int playerID = 0;
        private Queue<Message> _queue;
        private SyncEvents _syncEvents;
        private PlayerDictionary _myDictionary;
        private String[] positions = new String[] { "1::", "2::", "3::", "4::" };

    }
}
