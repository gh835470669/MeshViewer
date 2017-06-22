# MeshViewer
This is the personal project for Lesson Computer  Graphics

## Functionalities
### WireFrame and Fill Mode
| Wireframe | Fill |
| ---- | ---- |
|![](/readme/wireframe.png "wireframe") | ![](/readme/smooth.png "smooth") |

### Flat and Smooth Shading Mode
| Flat | Smooth |
| ---- | ---- |
|![](/readme/flat.png "flat") | ![](/readme/smooth.png "smooth") |

### Gourand and Phong Shading Mode
| Gourand | Phong |
| ---- | ---- |
|![](/readme/smooth.png "smooth") | ![](/readme/phong_color.png "phong") |

### Color and Texture Mode
| Color | Texture |
| ---- | ---- |
|![](/readme/phong_color.png "color") | ![](/readme/texture.png "texture") |

___

## Technology
* OpenGL
  * Vertex Array Object(VAO), Vertex Buffer Object(VBO), Texture Buffer Object(TBO)
  * Shaders:Vertex Shader and Fragment Shader
* OpenGL Extention Libraries
  * GLEW : glUniform\* glGen\* glBind\*
  * Assimp : read 3D model file and get the model data with the built-in data structures
  * SOIL : read the texture image files
  * GLM : OpenGL Mathematics Library
* Blinn-Phong illumination Model
* Lighting Material

## Manaul
* ctrl + left mouse button ： Translation
* ctrl + right mouse button : Rotation about Y-axis
* ctrl + middle mouse button : Scalling
