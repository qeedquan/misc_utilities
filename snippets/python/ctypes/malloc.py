import ctypes
import sys

# 1. Load the appropriate C standard library based on the OS
if sys.platform.startswith("win32"):
    libc = ctypes.CDLL("msvcrt") # Windows Microsoft C Runtime
else:
    libc = ctypes.CDLL(None)     # Linux / macOS (loads standard symbols)

# 2. Configure malloc: takes a size (size_t), returns a generic pointer (void*)
libc.malloc.argtypes = [ctypes.c_size_t]
libc.malloc.restype = ctypes.c_void_p

# 3. Configure free: takes a generic pointer (void*), returns nothing (None)
libc.free.argtypes = [ctypes.c_void_p]
libc.free.restype = None

# --- Example Usage ---

# Allocate enough memory for an array of 5 integers (5 * 4 bytes = 20 bytes)
num_elements = 5
byte_size = num_elements * ctypes.sizeof(ctypes.c_int)
raw_pointer = libc.malloc(byte_size)

if not raw_pointer:
    raise MemoryError("Failed to allocate C memory.")

try:
    # Cast the void* pointer to a usable integer array pointer
    IntArray = ctypes.c_int * num_elements
    array_pointer = ctypes.cast(raw_pointer, ctypes.POINTER(IntArray))
    assert(len(array_pointer.contents) == num_elements)
    
    # Write values to the allocated memory block
    for i in range(num_elements):
        array_pointer.contents[i] = (i + 1) * 10

    # Read values back out
    print("Values in C memory:")
    for i in range(num_elements):
        print(f"Element {i}: {array_pointer.contents[i]}")

finally:
    # 4. ALWAYS free your raw pointers manually to prevent memory leaks!
    libc.free(raw_pointer)
    print("\nMemory successfully freed.")

