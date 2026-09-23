# https://trac.ffmpeg.org/wiki/FancyFilteringExamples

# Extract a single chunk from an audio stream
ffmpeg -i input.mp3 -ss 00:01:30 -to 00:05:45 -c copy output.mp3

# Lowpass filter, cut out all frequencies above a specific threshol
ffmpeg -i input.mp4 -af "lowpass=f=1000" output.mp4
# Specify number of poles (more poles mean steeper cutoff at cutoff frequency)
ffmpeg -i input.wav -af "lowpass=f=1500:poles=1" output.wav

# Reduce background noise (use highpass to filter low frequency rumbles and lowpass to remove highpass hisses)
ffmpeg -i input.mp3 -af "highpass=f=200,lowpass=f=3000" output.mp3

# Render mandelbrot set
ffplay -f lavfi -i mandelbrot=s=1280x720:rate=60
ffmpeg -f lavfi -i mandelbrot=size=1920x1080:rate=60 -c:v libx264 -pix_fmt yuv420p mandelbrot.mp4

# Render cellular automata rule 110
ffplay -f lavfi -i cellauto=rule=110

# Render gradients
ffplay -f lavfi -i gradients=n=3:type=radial,format=rgb0
ffplay -f lavfi -i gradients=n=7:type=circular,format=rgb0
ffplay -f lavfi -i gradients=n=3:type=linear,format=rgb0
ffplay -f lavfi -i gradients=n=7:type=spiral,format=rgb0

# Show audio spectrum
ffmpeg -i input.flac -lavfi showspectrumpic=s=hd720 out.jpg
