# Webcam firmware 


Questions to ask:

How big should the image buffer be?

Where should the memory address start for the pdc buffer? In example code it is 0x6000... but IRAM starts at 0x2000... and when I change it gives me an error.

How do we access the elements of that buffer to find the length of the image/send it to the wifi chip?

Notes on enclosure:
Make walls 1mm thick
Threaded holes should be about 0.25mm smaller than the actual screw diameter

