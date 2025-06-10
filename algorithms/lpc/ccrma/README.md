LPC Examples
===
The examples to run start with the prefix `example_`.

There are 3 example files: 

* `example_lpc_simple.m` - a simple LPC example that only uses the signal power for analysis/resynthesis
* `example_lpc_pitch.m` - an LPC example that also performs a rudimentary pitch/noise detection
* `example_xsynth.m`- an example using LPC for cross-synthesis

Implementation
---
The files with the actual implementation are:

* `myLPC.m` - contains the implementation covered in [https://ccrma.stanford.edu/~hskim08/lpc/](https://ccrma.stanford.edu/~hskim08/lpc/)
* `lpcEncode.m` - runs `myLPC()` on windowed chunks of the input signal
* `lpcDecode.m` - renders the LPC coefficients from `lpcEncode()` to audio
* `lpcFindPitch.m` - rudimentary pitch/noise detector

Helper Functions
---
The following functions are used to simplify overlap-add (OLA).

* `stackOLA.m` - creates a stack(array) of OLA segments
* `pressStack.m` - renders the OLA stack into a single channel audio output



