# using cmake
adding option info in cmd line:
> cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-ggdb -g -pg -O0"

# handle makefile

in the dir with makefile, input cmd:
> mingw32-make

# add debugging information:

in cmakelist.txt:
> add_definitions("-Wall -ggdb -g -pg -O0")  
> -g 编译带调试信息的可执行文件  
> -O[n] 优化源代码  
>> -O 同时减小代码的长度和执行时间，其效果等价于-O1  
 -O0 表示不做优化  
 -O1 为默认优化  
 -O2 除了完成-O1的优化之外，还进行一些额外的调整工作，如指令调整等  
 -O3 则包括循环展开和其他一些与处理特性相关的优化工作  
> -Wall 打印警告信息  
> -pg 生成gmon.out文件    

# 运行gprof

in dir with .exe, input cmd:  
> gprof renderer.exe gmon.out > ana.txt   
I got this error:  
> BFD: Dwarf Error: Could not find abbrev number 3926.  
not solved yet.  

# Debugging

ctrl + f5
