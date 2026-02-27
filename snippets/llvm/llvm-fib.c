// clang -o fib llvm-fib.c `llvm-config --cflags` -lLLVM
// transliterated from the C++ version in LLVM fibonacii example
// https://github.com/llvm-mirror/llvm/blob/master/examples/Fibonacci/fibonacci.cpp
#include <llvm-c/Core.h>
#include <llvm-c/Target.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Analysis.h>
#include <stdio.h>
#include <stdlib.h>

static LLVMValueRef
CreateFibFunction(LLVMModuleRef M, LLVMContextRef Context)
{
	LLVMBuilderRef B = LLVMCreateBuilderInContext(Context);

	// Create the fib function and insert it into module M. This function is said
	// to return an int and take an int parameter.
	LLVMTypeRef  ParamTypes[] = {LLVMInt32TypeInContext(Context)};
	LLVMTypeRef  ReturnType   = LLVMInt32TypeInContext(Context);
	LLVMTypeRef  FunctionTy   = LLVMFunctionType(ReturnType, ParamTypes, 1, 0);
	LLVMValueRef FibF         = LLVMAddFunction(M, "fib", FunctionTy);

	// Add a basic block to the function.
	LLVMBasicBlockRef BB = LLVMAppendBasicBlockInContext(Context, FibF, "EntryBlock");

	// Get pointers to the constants.
	LLVMValueRef One = LLVMConstInt(LLVMInt32TypeInContext(Context), 1, 0);
	LLVMValueRef Two = LLVMConstInt(LLVMInt32TypeInContext(Context), 2, 0);

	// Get pointer to the integer argument of the add1 function...
	LLVMValueRef ArgX = LLVMGetFirstParam(FibF); // Get the arg.
	LLVMSetValueName(ArgX, "AnArg");             // Give it a nice symbolic name for fun.

	// Create the true_block.
	LLVMBasicBlockRef RetBB = LLVMAppendBasicBlockInContext(Context, FibF, "return");

	// Create an exit block.
	LLVMBasicBlockRef RecurseBB = LLVMAppendBasicBlockInContext(Context, FibF, "recurse");

	// Create the "if (arg <= 2) goto exitbb"
	LLVMPositionBuilderAtEnd(B, BB);
	LLVMValueRef CondInst = LLVMBuildICmp(B, LLVMIntSLE, ArgX, Two, "cond");
	LLVMBuildCondBr(B, CondInst, RetBB, RecurseBB);

	// Create: ret int 1
	LLVMPositionBuilderAtEnd(B, RetBB);
	LLVMBuildRet(B, One);

	// create fib(x-1)
	LLVMPositionBuilderAtEnd(B, RecurseBB);
	LLVMValueRef Sub       = LLVMBuildSub(B, ArgX, One, "arg");
	LLVMValueRef CallFibX1 = LLVMBuildCall(B, FibF, &Sub, 1, "fibx1");
	LLVMSetTailCall(CallFibX1, 1);

	// create fib(x-2)
	LLVMPositionBuilderAtEnd(B, RecurseBB);
	Sub                    = LLVMBuildSub(B, ArgX, Two, "arg");
	LLVMValueRef CallFibX2 = LLVMBuildCall(B, FibF, &Sub, 1, "fibx2");
	LLVMSetTailCall(CallFibX2, 1);

	// fib(x-1)+fib(x-2)
	LLVMPositionBuilderAtEnd(B, RecurseBB);
	LLVMValueRef Sum = LLVMBuildAdd(B, CallFibX1, CallFibX2, "addresult");

	// Create the return instruction and add it to the basic block
	LLVMPositionBuilderAtEnd(B, RecurseBB);
	LLVMBuildRet(B, Sum);

	return FibF;
}

int
main(int argc, char **argv)
{
	int n = argc > 1 ? atol(argv[1]) : 24;

	LLVMInitializeNativeTarget();
	LLVMLinkInInterpreter();

	LLVMContextRef Context = LLVMContextCreate();

	// Create some module to put our function into it.
	LLVMModuleRef M = LLVMModuleCreateWithNameInContext("test", Context);

	// We are about to create the "fib" function:
	LLVMValueRef FibF = CreateFibFunction(M, Context);

	// Now we going to create JIT
	LLVMExecutionEngineRef EE;
	char *                 outError;

	if (LLVMCreateInterpreterForModule(&EE, M, &outError) != 0) {
		printf("%s\n", outError);
		return 1;
	}

	printf("verifying...\n");
	if (LLVMVerifyModule(M, LLVMReturnStatusAction, &outError) != 0) {
		printf("%s\n", outError);
		return 1;
	}

	printf("OK\n");
	printf("We just constructed this LLVM module:\n\n---------\n");
	printf("%s\n", LLVMPrintModuleToString(M));

	LLVMGenericValueRef Args   = LLVMCreateGenericValueOfInt(LLVMInt32TypeInContext(Context), n, 0);
	LLVMGenericValueRef Result = LLVMRunFunction(EE, FibF, 1, &Args);

	printf("Result: %llu\n", LLVMGenericValueToInt(Result, 0));

	return 0;
}
