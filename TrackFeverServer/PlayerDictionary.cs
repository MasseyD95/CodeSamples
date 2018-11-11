using System; // For Console, Int32, ArgumentException,Environment
using System.Collections.Generic;

namespace EchoServerTrackFever
{
    public class PlayerDictionary
    {
        private Dictionary<int, Player> players = new Dictionary<int, Player>();
        int current = 0;
        public int GetDictionarySize()
        {
            int count;

            count = players.Count;

            return count;
        }
        public Dictionary<int, Player> GetDictionary()
        {
            return players;
        }


        public void AddPlayer(Player newPlayer) 
        { 
            
            players.Add(current, newPlayer);
            current += 1;
        }
    }
}
