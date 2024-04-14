# back-face culling

计算光线方向与三角形法线方向的夹角，来判断光是否从三角形的背面射过来，若是，不进行rasterization

# obj File

> f xxx/xxx/xxx xxx/xxx/xxx xxx/xxx/xxx

表示 f vert1/tex1/norm1 vert2/tex2/norm2 vert3/tex3/norm3  
纹理坐标的取值范围为[0, 1],取值时需要根据纹理图片的大小映射到正确的纹理坐标范围

# NDC (Normalized Device Coordinates)

模型文件经过坐标变换之后，坐标区间在[-1, 1]之间

# MVP变换

1. 把相机移到（0，0，0）  
相机的三个参数：  
> Position  
Look-at direction  
Up direction
2. 对应的模型也做相应的移动，所以本质上默认相机一直在原点，移动模型
3. projection:  
    - orthographic projection
    - perspective projection

# 右手系
该项目是右手系，屏幕内的空间坐标为负值

# Shading frequency
- flat shading: **triangle fasce** as shading point
- Gouraud shading: **triangle vertices** as shading point (vertex processing,顶点着色)
- phong shading: **pixel** as shading point (fragment processing，片元着色)

# OpenGL2 pipeline
Vertes Data -> Primitive Processing -> **Vertex Shader** -> Primitive Assemblly ->Rasterizer -> **Fragment Shader** ->Depth&Stencil -> Color Buffer Blend -> Dither -> Frame Buffer  
- Vertex Shader:  
    - transform the coordinates of the vertices
    - prepare data for the fragment shader  
- Fragment Shader:  
    - determine the color of the current pixel
    - discard current pixel by returning true

# 切线空间（Darboux frame，so-called tangent space）
切线空间三个轴分别是tangent轴、bitangent轴、法线轴，简称TBN坐标系。
局部空间的法线乘以变换矩阵得到切线空间的法线信息，再映射到rgb就是切线空间的法线贴图  
- z-与物体表面垂直
- x-主曲率方向(principal curvature direction)
- y-z^x

# Blinn-Phong 光照模型


