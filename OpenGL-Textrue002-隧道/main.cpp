//
//  main.m
//  OpenGL-Textrue002-隧道
//
//  Created by lxj on 2018/12/15.
//  Copyright © 2018 lxj. All rights reserved.
//

#include "GLShaderManager.h"
#include "GLBatch.h"
#include "GLFrustum.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"


#ifdef __APPLE__
#include <glut/glut.h>
#else
#include <GL/glut.h>
#endif


GLShaderManager shaderManager;
GLFrustum viewFrustum;//视景体
GLMatrixStack modelViewMatri;//模型视图矩阵
GLMatrixStack projectionMatri;//投影矩阵

//变换管道
GLGeometryTransform transformLine;

//三角形批次类
GLBatch leftWallBatch;
GLBatch rightBallBatch;
GLBatch topSkyBatch;
GLBatch floorBatch;

#define texture_floor 1
#define textrue_sky 2
#define textrue_wall 0
#define texture_count 3

const char *tgaNames[texture_count] = { "brick.tga", "floor.tga", "ceiling.tga" };

//纹理对象数组
GLuint textrues[texture_count];

GLfloat viewZ = -60.0f;


void specailKeys(int key , int x, int y){
    
    if(key == GLUT_KEY_UP)
        viewZ += 1.0f;
    
    if (key == GLUT_KEY_DOWN)
        viewZ -= 1.0f;
    
    glutPostRedisplay();
}

//窗口尺寸改变
void changeSize(int w, int h){
    glViewport(0, 0, w, h);
    
//    viewFrustum.SetOrthographic(-10, 10, -10, 10, -10, 10);
    viewFrustum.SetPerspective(80.0f, float(w)/float(h), 1.0f, 120.0f);
    
    projectionMatri.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    transformLine.SetMatrixStacks(modelViewMatri, projectionMatri);
}

//基本配置
void setup(void){
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    shaderManager.InitializeStockShaders();
    
    
    //初始化纹理对象
    glGenTextures(texture_count, textrues);
    
    GLbyte *pByte;//纹理数据
    
    GLint width, height, components;
    GLenum format;
    
    for (int i = 0; i < texture_count; i++) {
      
        //绑定纹理状态
        glBindTexture(GL_TEXTURE_2D, textrues[i]);

        //读取纹理数据
        pByte = gltReadTGABits(tgaNames[i],&width, &height,
                                &components, &format);
        
        //过滤方式
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        
        //环绕方式
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        //载入纹理
        glTexImage2D(GL_TEXTURE_2D, 0, components, width, height, 0, format, GL_UNSIGNED_BYTE, pByte);
        
        /**生成Mip图层 glGenerateMipmap
         参数1：纹理维度，GL_TEXTURE_1D,GL_TEXTURE_2D,GL_TEXTURE_2D
         */
        glGenerateMipmap(GL_TEXTURE_2D);
        free(pByte);
    }
    
    //建立几何图形
    GLfloat z;
    
    /*
     GLTools库中的容器类，GBatch，
     void GLBatch::Begin(GLenum primitive,GLuint nVerts,GLuint nTextureUnits = 0);
     参数1：图元枚举值
     参数2：顶点数
     参数3：1组或者2组纹理坐标
     */
    
    floorBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    //参考PPT图6-10
    //Z表示深度，隧道的深度
    for(z = 60.0f; z >= 0.0f; z -=10.0f)
    {
        floorBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        floorBatch.Vertex3f(-10.0f, -10.0f, z);
        
        floorBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        floorBatch.Vertex3f(10.0f, -10.0f, z);
        
        floorBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        floorBatch.Vertex3f(-10.0f, -10.0f, z - 10.0f);
        
        floorBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        floorBatch.Vertex3f(10.0f, -10.0f, z - 10.0f);
    }
    floorBatch.End();
    
    //参考PPT图6-11
    topSkyBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    for(z = 60.0f; z >= 0.0f; z -=10.0f)
    {
        topSkyBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        topSkyBatch.Vertex3f(-10.0f, 10.0f, z - 10.0f);
        
        topSkyBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        topSkyBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);
        
        topSkyBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        topSkyBatch.Vertex3f(-10.0f, 10.0f, z);
        
        topSkyBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        topSkyBatch.Vertex3f(10.0f, 10.0f, z);
    }
    topSkyBatch.End();
    
    //参考PPT图6-12
    leftWallBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    for(z = 60.0f; z >= 0.0f; z -=10.0f)
    {
        leftWallBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        leftWallBatch.Vertex3f(-10.0f, -10.0f, z);
        
        leftWallBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        leftWallBatch.Vertex3f(-10.0f, 10.0f, z);
        
        leftWallBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        leftWallBatch.Vertex3f(-10.0f, -10.0f, z - 10.0f);
        
        leftWallBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        leftWallBatch.Vertex3f(-10.0f, 10.0f, z - 10.0f);
    }
    leftWallBatch.End();
    
    //参考PPT图6-13
    rightBallBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    for(z = 60.0f; z >= 0.0f; z -=10.0f)
    {
        rightBallBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        rightBallBatch.Vertex3f(10.0f, -10.0f, z);
        
        rightBallBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        rightBallBatch.Vertex3f(10.0f, 10.0f, z);
        
        rightBallBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        rightBallBatch.Vertex3f(10.0f, -10.0f, z - 10.0f);
        
        rightBallBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        rightBallBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);
    }
    rightBallBatch.End();
}

//图形渲染
void renderScene(void){
    
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    modelViewMatri.PushMatrix();
    modelViewMatri.Translate(0, 0, viewZ);
    
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE,transformLine.GetModelViewProjectionMatrix(),0);

    
    //地板
    glBindTexture(GL_TEXTURE_2D, textrues[texture_floor]);
    floorBatch.Draw();
    
    //天花板
    glBindTexture(GL_TEXTURE_2D, textrues[textrue_sky]);
    topSkyBatch.Draw();
    
    
    //墙壁
    glBindTexture(GL_TEXTURE_2D, textrues[textrue_wall]);
    leftWallBatch.Draw();
    rightBallBatch.Draw();
    
    modelViewMatri.PopMatrix();
    
    glutSwapBuffers();
    
}


void rightButtonAction(int value){
    
    GLint iLoop;
    
    for(iLoop = 0; iLoop < texture_count; iLoop++)
    {
        /**绑定纹理 glBindTexture
         参数1：GL_TEXTURE_2D
         参数2：需要绑定的纹理对象
         */
        glBindTexture(GL_TEXTURE_2D, textrues[iLoop]);
        
        /**配置纹理参数 glTexParameteri
         参数1：纹理模式
         参数2：纹理参数
         参数3：特定纹理参数
         
         */
        switch(value)
        {
            case 0:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_NEAREST（最邻近过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                break;
                
            case 1:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_LINEAR（线性过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                break;
                
            case 2:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_NEAREST_MIPMAP_NEAREST（选择最邻近的Mip层，并执行最邻近过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                break;
                
            case 3:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_NEAREST_MIPMAP_LINEAR（在Mip层之间执行线性插补，并执行最邻近过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
                break;
                
            case 4:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_NEAREST_MIPMAP_LINEAR（选择最邻近Mip层，并执行线性过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                break;
                
            case 5:
                //GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER(缩小过滤器)，GL_LINEAR_MIPMAP_LINEAR（在Mip层之间执行线性插补，并执行线性过滤，又称为三线性过滤）
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                break;
                
            case 6:
                //设置各向异性过滤
                GLfloat fLargest;
                //获取各向异性过滤的最大数量
                glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
                //设置纹理参数
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
                break;
            case 7:
                //设置各向同性过滤，数量为1.0表示各向同性过
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
                break;
                
        }
    }
    
    //触发重画
    glutPostRedisplay();

}

int main(int argc,  char * argv[]) {
    
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("隧道");
    
    
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutSpecialFunc(specailKeys);
    
    //右键菜单
    glutCreateMenu(rightButtonAction);
    
    glutAddMenuEntry("GL_NEAREST",0);
    glutAddMenuEntry("GL_LINEAR",1);
    glutAddMenuEntry("GL_NEAREST_MIPMAP_NEAREST",2);
    glutAddMenuEntry("GL_NEAREST_MIPMAP_LINEAR", 3);
    glutAddMenuEntry("GL_LINEAR_MIPMAP_NEAREST", 4);
    glutAddMenuEntry("GL_LINEAR_MIPMAP_LINEAR", 5);
    glutAddMenuEntry("Anisotropic Filter", 6);
    glutAddMenuEntry("Anisotropic Off", 7);
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    GLenum err = glewInit();
    
    if (err != GLEW_OK) {
        printf("出错");
        return 1;
    }
    
    setup();
    glutMainLoop();
    
    //删除纹理
    glDeleteTextures(texture_count, textrues);
    
    return 0;
}
