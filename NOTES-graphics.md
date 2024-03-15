# back-face culling

计算光线方向与三角形法线方向的夹角，来判断光是否从三角形的背面射过来，若是，不进行rasterization

# obj File

> f xxx/xxx/xxx xxx/xxx/xxx xxx/xxx/xxx

表示 f vert1/tex1/norm1 vert2/tex2/norm2 vert3/tex3/norm3  
纹理坐标的取值范围为[0, 1],取值时需要根据纹理图片的大小映射到正确的纹理坐标范围

# NDC (Normalized Device Coordinates)

模型文件经过坐标变换之后，坐坐标区间在[-1, 1]之间