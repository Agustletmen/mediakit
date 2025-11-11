```shell
ABI

ZERO_CHECK
ALL_BUILD

ffmpeg -version
ffmpeg -devices    
ffmpeg -protocols
ffmpeg -filters
ffmpeg -muxers     
ffmpeg -demuxers
ffmpeg -decoders   
ffmpeg -encoders
ffmpeg -pix_fmts   
ffmpeg -sample_fmts
ffmpeg -layouts
```

一、基础图像操作算子

1. 像素级操作算子，直接操作图像像素（最底层算子）：

- cv::abs()：计算数组元素的绝对值（支持单通道 / 多通道图像）；
- cv::add()：图像加法（像素级相加，支持饱和运算，避免溢出）；
- cv::subtract()：图像减法；
- cv::multiply()：图像乘法（像素级相乘，可用于亮度调节、掩码叠加）；
- cv::divide()：图像除法；
- cv::addWeighted()：图像加权融合（αimg1 + βimg2 + γ，常用于图像叠加、透明度调节）；
- cv::bitwise_and/or/not/xor()：位运算（用于掩码操作、图像分割后处理）；
- cv::inRange()：筛选指定像素范围（如提取特定颜色区域，输出二值掩码）；
- cv::threshold()：全局阈值分割（将图像转为二值图，支持多种阈值模式：二进制、反二进制、截断等）；
- cv::adaptiveThreshold()：自适应阈值分割（根据局部区域亮度动态调整阈值，适合光照不均匀图像）。

2. 图像几何变换算子

- 改变图像尺寸、形状、位置：
- cv::resize()：图像缩放（支持多种插值方法：线性插值、双线性插值、立方插值等）；
- cv::rotate()：图像旋转（90°/180°/270° 固定角度旋转）；
- cv::warpAffine()：仿射变换（平移、旋转、缩放、剪切，需传入变换矩阵）；
- cv::warpPerspective()：透视变换（矫正倾斜图像，如文档扫描矫正，需传入透视矩阵）；
- cv::flip()：图像翻转（水平翻转、垂直翻转、水平 + 垂直翻转）；
- cv::transpose()：图像转置（行列互换）；
- cv::remap()：图像重映射（自定义像素映射关系，实现任意变形）。

二、滤波与降噪算子，用于平滑图像、去除噪声或增强细节：

线性滤波

- cv::blur()：均值滤波（简单平滑，用邻域像素均值替代当前像素，去噪但模糊细节）；
- cv::GaussianBlur()：高斯滤波（权重基于高斯分布，去噪同时保留细节，最常用）；
- cv::bilateralFilter()：双边滤波（同时考虑空间距离和像素相似度，去噪 + 保边，适合人像磨皮、边缘保留平滑）；
- cv::boxFilter()：方框滤波（类似均值滤波，可选择是否归一化）。

非线性滤波

- cv::medianBlur()：中值滤波（用邻域像素中值替代当前像素，对椒盐噪声、脉冲噪声去噪效果极佳）；
- cv::bilateralFilter()：双边滤波（非线性，保边去噪）；
- cv::fastNlMeansDenoising()：非局部均值去噪（针对高斯噪声，去噪效果优于高斯滤波，但速度较慢）；
- cv::guidedFilter()：导向滤波（保边平滑、图像增强，常用于 HDR 合成、图像去雾）。

三、边缘检测与特征提取算子

经典边缘检测算子

- cv::Canny()：Canny 边缘检测（多阶段边缘检测，抗噪声强、边缘连续，最常用）；
- cv::Sobel()：Sobel 算子（计算水平 / 垂直方向梯度，检测边缘，速度快）；
- cv::Scharr()：Scharr 算子（Sobel 算子的改进，对边缘的响应更灵敏，抗锯齿）；
- cv::Laplacian()：拉普拉斯算子（检测图像二阶导数，对快速变化的区域敏感，可检测边缘和细节，但对噪声敏感）；
- cv::HoughLines()/cv::HoughLinesP()：霍夫直线检测（从边缘图中检测直线，P 版为概率霍夫变换，速度更快）；
- cv::HoughCircles()：霍夫圆检测（检测图像中的圆形目标）。

特征点检测算子

- cv::cornerHarris()：Harris 角点检测（检测图像中的角点，用于特征匹配、图像拼接）；
- cv::goodFeaturesToTrack()：Shi-Tomasi 角点检测（Harris 算子的改进，更稳定，常用作特征点初始化）；
- cv::ORB()：ORB 算子（高效特征点检测 + 描述子提取，结合 FAST 角点检测和 BRIEF 描述子，开源免费，替代专利算法 SIFT/SURF）；
- cv::SIFT()：SIFT 算子（尺度不变特征变换，对尺度、旋转、光照变化鲁棒，但专利已过期，速度较慢）；
- cv::SURF()：SURF 算子（加速稳健特征，比 SIFT 快，同样鲁棒，但专利限制）；
- cv::FAST()：FAST 角点检测（超快速角点检测，用于实时场景）；
- cv::BRISK()：BRISK 算子（快速特征点检测 + 描述子，适合实时视觉任务）。

四、形态学操作算子，基于图像形状的操作，常用于分割、去噪、轮廓提取：

- cv::erode()：腐蚀（收缩白色区域，去除小噪声、断开细连接）；
- cv::dilate()：膨胀（扩大白色区域，填补小空洞、连接断开的区域）；
- cv::morphologyEx()：形态学开运算 / 闭运算 / 梯度 / 顶帽 / 黑帽（基于腐蚀和膨胀的组合）：
- 开运算（MORPH_OPEN）：先腐蚀后膨胀 → 去除小噪声、保留大目标；
- 闭运算（MORPH_CLOSE）：先膨胀后腐蚀 → 填补小空洞；
- 形态学梯度（MORPH_GRADIENT）：膨胀 - 腐蚀 → 提取目标边缘；
- 顶帽（MORPH_TOPHAT）：原图 - 开运算 → 突出亮背景中的暗目标；
- 黑帽（MORPH_BLACKHAT）：闭运算 - 原图 → 突出暗背景中的亮目标；
- cv::getStructuringElement()：创建形态学操作的核（如矩形核、椭圆核、十字核，控制形态学操作的范围和形状）；
- cv::morphologyDefaultBorderValue()：形态学操作的默认边界值（处理图像边缘像素）。

五、图像分割算子，将图像分成多个有意义的区域：

- cv::inRange()：基于颜色的阈值分割（如 HSV 空间筛选特定颜色）；
- cv::findContours()：轮廓检测（从二值图中提取目标轮廓，常用于分割后目标定位）；
- cv::watershed()：分水岭算法（基于标记的分割，适合粘连目标分割，需手动或自动设置前景 / 背景标记）；
- cv::grabCut()：GrabCut 分割（交互式 / 自动分割，基于图割算法，适合前景提取，如抠图）；
- cv::threshold()/cv::adaptiveThreshold()：阈值分割（二值化分割的基础）；
- cv::segmentation::selectiveSearchSegmentation()：选择性搜索（生成候选目标区域，用于目标检测预处理）。

六、颜色空间转换与通道操作算子

- cv::cvtColor()：颜色空间转换（最常用，如 BGR↔RGB、BGR↔GRAY、BGR↔HSV、BGR↔YCrCb 等）；
- cv::split()：图像通道分离（如将 BGR 图像拆分为 B、G、R 三个单通道）；
- cv::merge()：图像通道合并（将单通道图像合并为多通道）；
- cv::mixChannels()：通道重映射（灵活调整通道顺序或组合，如将 R 通道替换为 G 通道）；
- cv::inRange()：通道阈值筛选（如筛选 HSV 空间中 Hue 通道的特定范围）；
- cv::equalizeHist()：直方图均衡化（增强单通道图像对比度，常用于灰度图增强）；
- cv::createCLAHE()：自适应直方图均衡化（避免全局均衡化导致的过曝光，保细节）。

七、直方图与统计算子，用于图像统计分析、特征提取：

- cv::calcHist()：计算图像直方图（单通道 / 多通道，统计像素值分布）；
- cv::compareHist()：直方图比较（计算两个直方图的相似度，用于图像匹配、目标识别）；
- cv::minMaxLoc()：查找图像中像素的最小值、最大值及其位置；
- cv::meanStdDev()：计算图像的均值和标准差（用于亮度分析、噪声评估）；
- cv::countNonZero()：统计图像中非零像素的数量（如二值图中目标的像素数）；
- cv::moments()：计算图像矩（一阶矩、二阶矩等，用于目标中心、面积、方向等特征计算）。

八、高级视觉任务算子

目标检测

- cv::CascadeClassifier::detectMultiScale()： Haar 级联检测（基于训练好的模型，检测人脸、眼睛、车牌等）
- cv::dnn::readNet()/cv::dnn::forward()：DNN 模块（加载预训练的深度学习模型，如 YOLO、SSD、MobileNet，实现高精度目标检测）。

光流估计

- cv::calcOpticalFlowPyrLK()：Lucas-Kanade 光流（稀疏光流，跟踪特征点的运动）；
- cv::calcOpticalFlowFarneback()：Farneback 光流（稠密光流，计算所有像素的运动向量）。

图像匹配

- cv::matchTemplate()：模板匹配（在图像中查找与模板最相似的区域，用于简单目标定位）；
- cv::flannMatch()/cv::BFMatcher::match()：特征匹配（基于 ORB/SIFT 等描述子，匹配两张图像中的对应特征点，用于图像拼接、目标跟踪）。

立体视觉

- cv::stereoCalibrate()：立体标定（校准双目相机的内参和外参）；
- cv::stereoRectify()：立体校正（将双目图像校正为行对齐，便于视差计算）；
- cv::computeCorrespondEpilines()：计算极线（立体匹配中的约束条件）；
- cv::StereoBM::compute()/cv::StereoSGBM::compute()：视差图计算（基于块匹配或半全局块匹配，恢复三维深度信息）。

九、常用辅助算子

- cv::imread()/cv::imwrite()/cv::imshow()：图像读取 / 保存 / 显示（你之前代码中用到的）；
- cv::VideoCapture::read()：读取视频帧（视频处理核心）；
- cv::putText()：在图像上绘制文字（可视化结果）；
- cv::line()/cv::rectangle()/cv::circle()/cv::polylines()：绘制基本图形（标注目标、绘制轮廓）；
- cv::resize()/cv::crop()：图像缩放 / 裁剪（预处理常用）；
- cv::fillPoly()：填充多边形（如填充轮廓内部）；
- cv::filter2D()：自定义卷积滤波（手动设计卷积核，实现自定义效果，如锐化、边缘检测）。
