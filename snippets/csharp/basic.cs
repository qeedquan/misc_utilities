using System;
using System.Threading;
using System.Numerics;

public class Basic
{
    static public void Main(string[] args)
    {
        TestArgs(args);
        TestGoto();
        TestThreads();
        TestMath();
        TestPrint();
    }

    static public void TestArgs(string[] args)
    {
        Console.WriteLine("Testing Arguments");
        Console.WriteLine("Number of Arguments: " + args.Length);
        var idx = 0;
        foreach (var arg in args)
            Console.WriteLine("{0}: {1}", idx++, arg);
        Console.WriteLine();
    }

    static public void TestGoto()
    {
        Console.WriteLine("Testing Goto");

        while (true)
            goto Loop1;
    Loop1:
        Console.WriteLine("Broke out of Loop1");

        var i = 20;
        switch (i)
        {
        case 2:
            Console.WriteLine("case 2");
            goto case 3;
        case 20:
            Console.WriteLine("case 20");
            goto case 2;
        case 3:
            Console.WriteLine("case 3");
        case 4:
            Console.WriteLine("case 4");
            break;
        case 5:
            Console.WriteLine("unreachable");
            break;
        }

        Console.WriteLine();
    }

    static public void TestThreads()
    {
        var thread1 = new Thread(ThreadRunnerSleep);
        var thread2 = new Thread(ThreadRunnerArg);
        var thread3 = new Thread(ThreadRunnerArg);
        thread1.Start();
        thread2.Start(256);
        thread3.Start("ABC");
        thread1.Join();
        thread2.Join();
        thread3.Join();
        Console.WriteLine();
    }

    static public void ThreadRunnerSleep()
    {
        Console.WriteLine("Runner Sleep");
        Thread.Sleep(100);
        Console.WriteLine("Done Sleeping");
    }

    static public void ThreadRunnerArg(object arg)
    {
        Console.WriteLine("Runner Arg: {0}", arg);
        if (arg is object)
            Console.WriteLine("Is an object");
        if (arg is int)
            Console.WriteLine("Is an integer");
        if (arg is string)
            Console.WriteLine("Is a string");
    }

    static public void TestMath()
    {
        int Value = 0;

        Console.WriteLine("Testing Math");
        Console.WriteLine("Sqrt: {0}", Math.Sqrt(9.3));
        Console.WriteLine("Max: {0}", Math.Max(20, 4));
        Console.WriteLine("Min: {0}", Math.Min(20, 4));
        Console.WriteLine("Sin: {0}", Math.Sin(Math.PI));
        Console.WriteLine("Cos: {0}", Math.Cos(Math.PI));
        Console.WriteLine("Tan: {0}", Math.Tan(Math.PI));
        Console.WriteLine("Pow: {0}", Math.Pow(Math.E, Math.PI) - Math.PI);
        Console.WriteLine("Negate: {0} {1}", ~Value, ~1);
        Console.WriteLine();
    }

    static void Print(string format, params object[] args)
    {
        Console.WriteLine(format, args);
    }

    static public void TestPrint()
    {
        Print("Hello {0}", "World");
        Print("{0:0.000} {1:000.000}", 1.2, 4);
        Print("'{0,23}'", "Left Padding");
        Print("'{0,-23}'", "Right Padding");
        Print("'{0,-23}' '{1,-23}' '{2,23}'", "X", "Y", "Z");
    }
}
