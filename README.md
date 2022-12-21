# Multi-Resolution Isosurface Rendering

[TOC]

## 1. 我们提供的输入

我们提供了 4 个 `vdb` 文件和 1 个 `obj` 文件，分别是两个 single-resolution 的 `vdb`，两个 multi-resolution 的 `vdb`，和一个单位球的 mesh。

这 4 个 `vdb` 来自于 fluid simulation，是**速度场，因此是一个向量场**。而各位需要对一个数量场做可视化，因此**可以计算速度场的模值，随后调用 `OpenVDB` 的某些函数创建一个数量场**。



## 2. VDB 文件里有什么？

一个 `vdb` 文件可能包含一个或多个 uniform grid。我们提供的 4 个文件中，single-resolution 的 `vdb` 包含 1 个 uniform grid，而 multi-resolution 的 `vdb` 文件包含 3 个 uniform grid。

我们在 `vdb` 文件的 `metadata` 中提供了 uniform grid 的世界坐标信息：

1. `origin`: 类型为 `openvdb::Vec3S` 或 `openvdb::Vec3d` 中的一种 (猜猜是哪种呢？)，记录了 uniform grid **“左下角” 的坐标**，
2. `dx`: 类型为 `float` 或 `double` (再猜？)，记录了 uniform grid **一个格子的宽度**。

各位可以**调用 `OpenVDB` 的某些函数来读取某个 uniform grid 的 metadata**。

值得注意的是，grid cell 的 **值总是存储在 cell 的 “角上”，而不是 cell 的中心**。

举个例子：

1. Cell `(0, 0, 0)` 的世界坐标为 `p0 = origin + (0, 0, 0) * dx`，值存储在 `p0` 处，而不是 `p0 + (0.5, 0.5, 0.5) * dx`，
2. Cell `(1, 2, 3)` 的世界坐标为 `p1 = origin + (1, 2, 3) * dx`，值存储在 `p1` 处，而不是 `p1 + (0.5, 0.5, 0.5) * dx`。

还需要注意，某些 cell 可能是被 “挖空” 的，比如你可以想象绝大多数位置只会落在 3 个 uniform grid 中的一个，各位需要**调用 `OpenVDB` 的某些函数来判断某个 cell 是否是被 “挖空” 的**。



## 3. 什么是 Multi-Resolution 的 Uniform Grid？

我们提供的 `vdb` 和文章中的 `AMR` 非常像：

1. Coarser 层 `dx` 总是为 finer 层 `dx` 的两倍，
2. Coarser 层和 finer 层的格子是对齐的 (各位可以检查一下相邻两层 grid 的 `origin`，就能理解了)。

但是也有一些小区别：

1. 我们提供的 `vdb` 相邻两层可能会有 overlap，这时候请总是取 finer 那层的数据。



## 4. 各位需要实现什么？

能够快速 (至少得加个 OpenMP) 而优美地可视化出这 4 个 `vdb` 文件。

能处理 volume 中嵌入 surface 的情况，即带上 `sphere.obj`。球的世界坐标设为 `(5, 2, 3)`，缩放设为 `(4.0 / 3.0)`。



## 5. 太简单了，我想写 GPU 版的

可以用 NanoVDB (OpenVDB 的 GPU 版) 搭配 CUDA 来实现，有亿点麻烦，我们不做要求。如果你真的想写 GPU 版，请单独联系我们，我们可以提供一些基本的帮助。



## 6. 太难了啊喂

每周一、周三的上课时间各位可以向我们求助，但自己写的 bug 还是得自己 de。祝好！