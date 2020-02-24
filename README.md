# EECS326_webcam
Webcam firmware 


Questions to ask Ilya:

How does is the USART RX buffer supposed to be setup? Using PDC (Peripheral DMA)? Or another method (FIFO)? FIFO

If using FIFO then how big should the buffer be? 1000, clear buffer with for loop, setting every entry to zero.

What should the size of the image buffer be? Image width and height specifically. Not known yet. Look at board.h files for camera configurations (hard)

