<div aligne="center">
# FAST MALLOC!
</div>
what do you think that it do?

## Fast , Multithreading safe Memory allocater for Linux and Unix-like OSs
this header only library is a simple memory allocater built around proformanse without scrfising safty and memory,

using only the system calls brk and sbrk to achive a nice proformanse

## how it works?
when ever the user (ie the deviloper) calld malloc, it checks if ther is already a free chunck, if ther is a other thread working on that chunck,

if the requasted memory is tiny , it trust the locked thread and pass to the next memory chunck, 

giving us some air to breath in term of multithread proformance.



## Notes:
- NEVER , NEVER , if you use this library, call the standerd allocater, NEVER, if you have not a problam, trust me , YOU WILL!
- This project is still in Alpha, if you have any bugs, or any notes, pleas let me know
## With that been sed, Fear Allah in your work, and i hope you get a nice day
