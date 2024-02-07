This is a linked-list based project in which a world is represented by a 2D array of Districts. 
Each of these districts keeps track of the people living within it using linked lists.
There is a series of functions that can be accessed when compiled with the driver.cpp file, that allow for births, deaths, movements etc.
All functions besides listing all the members of a district are bigO(1), with listing the members of the district being bigO(N).

To compile: g++ -std=c++11 driver.cpp -o driver
