# EcoMapleGraphics


>[!NOTE]
>  Aun no se incluye la carpeta models debido a que como va aumnetar de tamaño estos se deben de decargar por separado, lo más probable es que hasta que este final se comprima y seba añadir manualmente al proyecto.
>Para ello descargar la carpeta models del siguiente link y meterlo en la carpeta BIN del [***Descarga Carpeta models***](https://drive.google.com/drive/folders/1eo8KdIWDBilb1HSjECFdnIou3gmr_QRI?usp=sharing)

>[!WARNING]
> Configurar como Archivos de Origen Existentes **TODOS** los `.cpp` y en archivos de encabezado agregar como existentes todos los que estan dentro del `include`

# ¿Cómo usar el entorno de Pruebas? (IMPORTANTE LEER ESTE PUNTO)

**Solo leer este punto si deseas probar el entorno**

El siguiente código es la base para implementar el entorno final del proyecto, se tomo como referencia el código enviado por Tenshi

**NO ASUSTAR** no dejarse abrumar por demasido código, esto es debido a que el entorno del bosque se genero de manera procedural y de misma manera las mecanicas deben afectar a todo lo generado proceduralmente.

Para probar modelos y empezar a testear cosas es necesario empezar a familiarizarse con estos archivos
* `globals.h`
* `src_manager.h`
* `src_manager.cpp`
* `render.cpp`
* `main.cpp`

**1.** en el `main.cpp`, en la linea 20 verificar que se encuentre activado el entorno de prueba
~~~C++
bool g_runTestEnvironment = true;
~~~
En caso de querer ver el bosque que implemento angel, poner en `false` este valor.

**2.** En el archivo `globales.h`, en este archivo se especifican variables globales y se definen `structs` para agrupar varibles, de forma que:

Para un implementar objetos en el variable entorno en este archivo en el `struct` `TestAssets` se declaran, las luces, materiales, modelos que se van a usar. (Si se van a experimentar modificar este struct)
~~~C++
struct TestAssets { //Elementos Para el entorno de Prueba
    Light light01;
    Light light02;
    Material steel;
    Material asphalt;
    Model* car;
    Model* luminaire;
    Model* stop;
    Model* floor;
};
~~~

**Por cada variable que queramos que sea global es necesario definir con `extern` en este archivo**

Por ejemplo ya que se definiio esta estructura debemos de declararla globalmente.
~~~C++
// -- Assets de Test
extern TestAssets ta;
~~~

**3.** Una vez definido las variables que vamos a usar en el archivo `src_manager.cpp` se tienen 3 funciones
`initializeModelsTest`, `initilizeMaterialsTest`, ` initilizeLightsTest`, aqui se asigna el nombre donde se encuentra el modelo, las componentes de los materiales que se definio en el estruct y las caracteristicas de la luz.

**4.** En el archivo render.cpp, se tiene la lógica de la generación procedural, **NO AUSUSTAR**, solo enfocarse en la función `renderTestEnvironment`, aqui se van a implementar los objetos en el entorno.

A partir de la linea 70 se empiezan a cargar los modelos.

>[!TIP]
>Para no batallar adivinando coordenadas, se recomiennda en blender a cada objeto aplicarle con  `CTRL + A` la opción `ALL TRANSFOMRS` y si el cursor 3D (cirulo rojo punteado) se encuentra en el centro darle click derecho en `Set Origin` --> `Origin to 3D Cursor`, guardar el archivo y despues exportar a `.FBX`


De esta forma se conservan la escala, y localizacion de blender de forma que solo es necesario declarar una matriz de modelo unitaria
~~~C++
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
~~~

y posteriormente por medio de la función `drawObject` se le pasa el shader que vamos a ocupar en este caso mlightsShader (phong-multiluz), el modelo que quermeos implementar, el material que se va aplicar y la matriz de modelo. El modelo y material que se definiio en el struct se accede mediante el `.` 

~~~C++
    //Dibujado de los Objetos
    drawObject(mLightsShader, ta.car, ta.steel, model);
~~~

En caso de que sea necesario implementar animaciones o una matriz de transformación diferente, es necesario crear antes de llamar a drawObject otra matriz de modelado.

Ejemplo
~~~C++
model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.5f, 1.1f, 1.0f));
    drawObject(mLightsShader, ta.luminaire, ta.steel, model);
~~~

Se recomienda darle un vistazo al código completo, de preferencia revisar todos los .cpp y sus .h correspondientes


## Uso Skybox
Para la skybox:
mycube son dos archivos para la skybox, que es la skybox de dia y la de noche

## Interfaz Gráfica (UI)
Para el IU:
models/image estan los imagenes que ocupamos en la pantalla 
**Interaccion con el mundo:**

*  `A,W,S,D` : es para **moverse**
* `P` y viendo hacia una parte cercada del suelo: se **planta un arbol**:
* `F`: Se **inicia el incendio** dura alrededor de 90 segundos, pueden cambiarlo desde las variables del codigo:
* `G`: Se **apaga el incendio** 
* `espacio`: **volar**
* `ctrl`: **descender**

### PICKING (Interacición Objetos)
***Mecanica de talar arboles***

Para esta mecanica se ocupo la cruz que tenemos en el centro de pantalla y es necesario hacer in clic izquierdo sobre el tronco para talar el arbol, pasara de un arbol con hojas a un arbol sin hojas y despues a una base de arbol cortada si se clikea en ese orden


### Iluminacion

Para mover el sol y la luna se mueve con las teclas de arriba, abajo, izqueirdaa y derecha

# TO DO (Pendientes)
* [X] Refactorizar Codigo
* [X] Verificar el funcionamiento
* [X] Implementación de funciones de utilidad
* [X] Implementación de los shaders para varias fuentes de iluminación (ver con angel las diferencias)

# CAMBIOS IMPORTANTES

Se modifico el archivo `modelstructs.h` se añadio a la palabra clave `inline` para que no marque error, asegura que todas las copias de esta función son iguales
~~~c++
inline unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false)
{
    // ...
}
~~~