using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TestApiClientNet
{
    class Program
    {
        static void Main(string[] args)
        {
            TestApiDllNet.ItemCollection collection = new TestApiDllNet.ItemCollection();
            collection.addItem(new TestApiDllNet.Item { I = 98, S = "bob" });
            collection.addItem(new TestApiDllNet.Item { I = 76, S = "joe" });
            collection.addItem(new TestApiDllNet.Item { I = 54, S = "rick" });
            collection.addItem(new TestApiDllNet.Item { I = 32, S = "jim" });
            collection.addItem(new TestApiDllNet.Item { I = 10, S = "jack" });
            collection.addItem(new TestApiDllNet.Item { I = -123, S = "john" });

            Console.WriteLine("Iterating on the collection:");
            foreach(var item in collection)
            {
                Console.WriteLine("Item: i={0}, s={1}", item.I, item.S);
            }
        }
    }
}
