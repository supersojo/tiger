
-- 文件列表 --

o bin
  flex、bison命令行工具

o doc

o lib
  flex的库

o obj
  目标文件目录

o src
  符号表、抽象语法标识、代码生成、tiger解释器
  源码目录

o tools
  flex bison3 tarball

o .
  build.ninja
    ninja配置文件
  toolchain.ninja
    ninja配置文件
  tiger.ninja
    ninja配置文件
  tiger.lex
    tiger词法文件
  tiger.yy
    tiger语法文件
  test.tiger
    tiger测试文件

-- 使用说明 --

o $bin/ninja.exe
  生成tiger.exe t.exe
  tiger.exe执行词法、语法、语义分析并生成x64汇编代码src/t.asm
  t.exe由src/t.asm生成可执行文件

o $bin/ninja tiger
  生成tiger.exe

o $bin/ninja t
  把生成的汇编代码链接成可执行文件t.exe

-- 注意 --

o 仅实现简单的整数+,-运算和print语句，不支持优先级

o 本来想实现动态类型，发现动态类型无法生成汇编代码

o 动态类型都是解释执行的，对于函数要想生成汇编必须确切知道变量的类型
  否则无法分配变量执行运算

o 这里仅实现了整数类型，字符串考虑到内存分配未实现

o 实现了解释器解释抽象语法树

-- x64汇编 --

X64调用规范

1、地址都是64位，堆栈操作只能是64位的，不支持32位压栈出栈

2、参数传递，前4个rcx,rdx,r8,r9,剩下的由右往左压栈

3、长度不足64位不进行零扩展，高位值是不确定的

4、返回值如果小于64位由rax传递，大于64位通过堆栈传递，rcx保存地址

5、调用者负责开辟（函数调用的4个寄存器及参数）影子空间，即使不用堆栈传递也要开辟

6、寄存器rax,rcx,rdx,r8,r8,r10,r11常被子程序修改，如果主程序使用必须在调用子程序前堆栈保存，调用结束后从堆栈恢复再使用

7、rbx、rbp、rdi、rsi、r12、r13、r14、r15必须由子程序保存，主程序可以随意使用，不用怕被子程序修改

