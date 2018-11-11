using System; // For Console, Int32, ArgumentException,Environment
using System.Net; // For IPAddress
using System.Net.Sockets; // For TcpListener, TcpClient
using System.Text; //For StringBuilder
using System.Threading;
using System.Collections;
using System.Collections.Generic;

namespace EchoServerTrackFever
{


	// State object for reading client data asynchronously  
	public class StateObject
	{
		// Client  socket.  
		public Socket workSocket = null;
		// Size of receive buffer.  
		public const int BufferSize = 1024;
		// Receive buffer.  
		public byte[] buffer = new byte[BufferSize];
		// Received data string.  
		public StringBuilder sb = new StringBuilder();


	}

	public class AsynchronousSocketListener
	{
        
        public static Producer producer;

		// Thread signal.  
		public static ManualResetEvent allDone = new ManualResetEvent(false);

        #region 
        public static bool hasStarted = false;

        public static void TCP_StartListening()
		{
            // Data buffer for incoming data.  
            byte[] bytes = new Byte[1024];
			int localPort = 13000;
			// Establish the local endpoint for the socket.  
			// The DNS name of the computer  
			// running the listener is "host.contoso.com".  

			IPHostEntry ipHostInfo = Dns.GetHostEntry(Dns.GetHostName());
			IPAddress ipAddress = ipHostInfo.AddressList[1];
			IPEndPoint localEndPoint = new IPEndPoint(IPAddress.Any, localPort);

            // Create a TCP/IP socket.  
            Socket TCP_listener = new Socket(ipAddress.AddressFamily,
				SocketType.Stream, ProtocolType.Tcp);

			// Bind the socket to the local endpoint and listen for incoming connections.  
			try
			{
				TCP_listener.Bind(localEndPoint);
				TCP_listener.Listen(100);

				while (true)
				{
					// Set the event to nonsignaled state.  
					allDone.Reset();

					// Start an asynchronous socket to listen for connections.  
					Console.WriteLine("Waiting for a TCP connection...");

                    TCP_listener.BeginAccept(new AsyncCallback(TCP_AcceptCallback), TCP_listener);
					// Wait until a connection is made before continuing.  
					allDone.WaitOne();
				}

			}
			catch (Exception e)
			{
				Console.WriteLine(e.ToString());
			}

			Console.WriteLine("\nPress ENTER to continue...");
			Console.Read();

		}

        public static void TCP_AcceptCallback(IAsyncResult ar)
		{
			// Signal the main thread to continue.  
			allDone.Set();

			// Get the socket that handles the client request.  
			Socket listener = (Socket)ar.AsyncState;
			Socket handler = listener.EndAccept(ar);

			// Create the state object.  
			StateObject state = new StateObject();
			state.workSocket = handler;
			handler.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
				new AsyncCallback(TCP_ReadCallback), state);
		}


		public static void TCP_ReadCallback(IAsyncResult ar)
		{

			String content = String.Empty;

			// Retrieve the state object and the handler socket  
			// from the asynchronous state object.  
			StateObject state = (StateObject)ar.AsyncState;
			Socket handler = state.workSocket;

			// Read data from the client socket.   
			int bytesRead = handler.EndReceive(ar);

			if (bytesRead > 0)
            {
                // Check for end-of-file tag. If it is not there, read   
                // more data.  
                content = Encoding.ASCII.GetString(state.buffer, 0, bytesRead);
                if (content.IndexOf("::") > -1)
                {
                    // All the data has been read from the   
                    // client. Display it on the console.  
                    Console.WriteLine("Read {0} bytes from socket. \n Data : {1}",
                        content.Length, content);
                    
                    Message tempMessage = new Message(content);
                    tempMessage.SetSocket(handler);

                    producer.ThreadRun(tempMessage);
                    // Echo the data back to the client.
                    tempMessage = null;

                    //TCP_Send(handler, content);
                }
                // Not all data received. Get more.  
                handler.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                new AsyncCallback(TCP_ReadCallback), state);
            }
		}

		public static void TCP_Send(Socket handler, String data)
		{
			// Convert the string data to byte data using ASCII encoding.  
			byte[] byteData = Encoding.ASCII.GetBytes(data);
            Console.WriteLine("Data : {0}", data);

			// Begin sending the data to the remote device.  
			handler.BeginSend(byteData, 0, byteData.Length, 0,
				new AsyncCallback(TCP_SendCallback), handler);
		}

		private static void TCP_SendCallback(IAsyncResult ar)
		{
			try
			{
				// Retrieve the socket from the state object.  
				Socket handler = (Socket)ar.AsyncState;

				// Complete sending the data to the remote device.  
				int bytesSent = handler.EndSend(ar);
				Console.WriteLine("Sent {0} bytes to client.", bytesSent);
			}
			catch (Exception e)
			{
				Console.WriteLine(e.ToString());
			}
		}
		#endregion TCP Server

#region 
		public static int UDP_localPort = 13002;

		public static void UDP_StartListening()
		{
            Console.WriteLine("Waiting for a UDP connection...");

            UDP_ReceiveMessages();
        }

		public static bool messageReceieved = false;

		public static void UDP_ReceiveCallback(IAsyncResult ar)
		{
			UdpClient u = (UdpClient)((UdpState)(ar.AsyncState)).u;


			IPEndPoint e = (IPEndPoint)((UdpState)(ar.AsyncState)).e;

			Byte[] receieveBytes = u.EndReceive(ar, ref e);
			string receiveString = Encoding.ASCII.GetString(receieveBytes);

			Console.WriteLine("Read {0} bytes from socket. \n Data : {1}", receieveBytes.Length, receiveString);

            if (receieveBytes != null)
            {
                //If not empty then make a message class and send it to producer
                Message localMessage = new Message(receiveString);
                localMessage.SetEndPoint(e);
                producer.ThreadRun(localMessage);
            }
			u.BeginReceive(new AsyncCallback(UDP_ReceiveCallback), (UdpState)(ar.AsyncState));

		}

		public static void UDP_ReceiveMessages()
		{
			// Receive a message and write it to the console.
			IPEndPoint e = new IPEndPoint(IPAddress.Any, UDP_localPort);

			UdpClient u = new UdpClient(e);
			UdpState s = new UdpState();
            u.EnableBroadcast = true;

			s.e = e;
			s.u = u;
            
			Console.WriteLine("listening for messages");
			u.BeginReceive(new AsyncCallback(UDP_ReceiveCallback), s);
        }

		public static bool messageSent = false;

		public static void UDP_SendCallback(IAsyncResult ar)
		{
			UdpClient u = (UdpClient)ar.AsyncState;
			Console.WriteLine("Number of bytes send: {0}", u.EndSend(ar));

			messageSent = true;
		}

		public static void UDP_SendMessage(IPEndPoint server, string message)
		{
			// create the udp socket
			UdpClient u = new UdpClient();
            u.Connect(server);

            Byte[] sendBytes = Encoding.ASCII.GetBytes(message);
            Console.WriteLine("Data : {0}", message);

			// send the message
			// the destination is defined by the call to .Connect()

			u.EnableBroadcast = true;
			u.BeginSend(sendBytes, sendBytes.Length, new AsyncCallback(UDP_SendCallback), u);

			// Do some work while we wait for the send to complete. For
			// this example, we 'll just sleep
			while (!messageSent)
			{
				Thread.Sleep(100);
			}
		}
		#endregion UDP Server  


		public static int Main(String[] args)
		{
            bool closeServer = false;
            //Making queues and dictionary to pass to producer and consumer 
            Queue<Message> queue = new Queue<Message>();
            SyncEvents syncEvents = new SyncEvents();
            PlayerDictionary dictonary = new PlayerDictionary();
            producer = new Producer(queue, syncEvents);

            Console.WriteLine("Configuring worker threads...");
            Consumer consumer = new Consumer(queue, syncEvents, dictonary);
            Thread consumerThread = new Thread(consumer.ThreadRun);

            Console.WriteLine("Launching producer and consumer threads");
            consumerThread.Start();
            //Load threads to launch recieve callbacks on
			Thread TCP_Thread;
			Thread UDP_Thread;

            TCP_Thread = new Thread(new ThreadStart (TCP_StartListening));
			UDP_Thread = new Thread(new ThreadStart(UDP_StartListening));

			TCP_Thread.Start();
			UDP_Thread.Start();

            while (!closeServer)
            {

            }
            Console.WriteLine("Signaling threads to terminate...");
            syncEvents.ExitThreadEvent.Set();

            consumerThread.Join();
    		return 0;
		}

        private static void ShowQueueContents(Queue<int> q)
        {
        	lock (((ICollection)q).SyncRoot)
        	{
        		foreach (int item in q)
        		{
        			Console.Write("{0} ", item);
        		}
        	}
        	Console.WriteLine();
        }
	}
}
