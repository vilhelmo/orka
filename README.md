orka
====

Linux image viewer/player

TODO
====
* Create control bar widget at the bottom, play pause stop, time/frame-slider etc.
* Support opencolorio
* Create settings window for gamma control, exposure, LUT and any other image settings we can think of.
* Create commands to reset viewport to different sizes (1-to-1 pixel, adjust to screen size, double size, half size etc)

Test Images
====
I ran the following to download a few hundred frames from Big Buck Bunny (http://www.bigbuckbunny.org/) to the current folder:
for frame in `seq 379 553`; do 
wget http://media.xiph.org/BBB/BBB-1080-png/big_buck_bunny_00$frame.png; 
done

Test movies
====
Just download any mpeg or other movie file to test it, libvlc should support most of them.

Dependencies
====
Currently these are the dependencies:
OpenImageIO - 1.2.0
libvlc - 2.0.8
Qt5 - not sure :)

They can be installed on a debian-like system by running
apt-get install libopenimageio-dev libvlc-dev
and
doing a google search on how to install qt5.

In the future opencolorio might become a dependency. 

Style
====
Try to follow the Google C++ style guide here:
http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
