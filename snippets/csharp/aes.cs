using System;
using System.Threading;
using System.Numerics;
using System.IO;
using System.Security.Cryptography;
using System.Text;

public class AES
{
    static public void Main(string[] args)
    {
        if (args.Length != 4)
        {
            Console.WriteLine("usage: key iv infile outfile");
            return;
        }

		var key = Encoding.UTF8.GetBytes(args[0]);
		var iv = Encoding.UTF8.GetBytes(args[1]);
		var data = File.ReadAllBytes(args[2]);
		var aes = new RijndaelManaged();
		var decryptor = aes.CreateDecryptor(key, iv);
		var output = decryptor.TransformFinalBlock(data, 0, data.Length);
		File.WriteAllBytes(args[3], output); 
    }
}
