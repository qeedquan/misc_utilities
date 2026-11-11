using System;
using System.Threading;
using System.Numerics;
using System.IO;
using System.Security.Cryptography;
using System.Text;

public class MD5
{
    static public void Main(string[] args)
    {
        if (args.Length != 1)
        {
            Console.WriteLine("usage: file");
            return;
        }

		var data = File.ReadAllBytes(args[0]);
		var md5 = new MD5CryptoServiceProvider();
		var hash = md5.ComputeHash(data);
		Console.WriteLine(BitConverter.ToString(hash));
    }
}
