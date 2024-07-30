### **ZJU-Image Information Processing Course-Assignments**

#### 内容
1. BMP灰度图和真彩图读取和写入
2. 图像二值化和形态学操作
3. 直方图均值化和各种直方图变换（并加入jpeg编码）
4. 几何学操作和变形


#### 文件结构：
- `public`文件夹：存放bmp图像数据，其中test.bmp和testColor.bmp是输入图像
  - `verify`：(省略)matlab检验和结果对比
  - `stdio`：目录下是给jpeg编码使用的量化表和范式哈夫曼编码表，jpeg编码难度有点大，还在实验中。
- `src`：存放源代码和可执行文件Binarize.exe。其中
  - `io`：图像输入输出的程序，目前可以输入输出bmp和jpeg；
  - `core`：图像数据结构及对图像的各种操作方法；
  - `test.cpp`：测试程序。

#### 运行

使用g++或其他c++编译器来编译。

2.编译/运行方法：

进入src文件夹，并在终端中打开该文件夹。若使用make命令，windows系统可在终端输入

```ps
mingw32-make -s
```

在其他系统输入
```ps
make
```

即可生成可执行文件exe。
若makefile报错，可在命令行直接输入以下命令

```ps
g++ -std=c++11 ./io/readBMP.cpp  test.cpp -o Binarize.exe -I core/ -L core/ -lcore -static
```

经过编译后便可在该目录产生exe
