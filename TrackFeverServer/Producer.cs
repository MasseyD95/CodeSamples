using System;
using System.Collections;
using System.Collections.Generic;

namespace EchoServerTrackFever
{
    public class Producer
    {
        public Producer(Queue<Message> q, SyncEvents e)
        {
            _queue = q;
            _syncEvents = e;
            
        }
        public void ThreadRun(Message message)
        {
            if (message != null)
            {
                //Add to the queue for consumer to take
                _queue.Enqueue(message);
                _syncEvents.NewItemEvent.Set();

                message = null;
            }
        }
        //Store the queue so it uses the same as the producer
        private Queue<Message> _queue;
        private SyncEvents _syncEvents;
    }
}
