# EcoMapleGraphics

>[!WARNING]
> Aun no se incluye la carpeta models debido a que como va aumnetar de tamaño estos se deben de decargar por separado, lo más probable es que hasta que este final se comprima y seba añadir manualmente al proyecto.

## Modelos Implementados Hasta el momento

En el código los modelos cargados son : (Ya estan optimizado todos estos modelos y tienen textura cada uno)

1. **Arbol** (se tienen 3 modelos de arboles diferentes cargados)
* Arbol con hojas arce
* Arbol quemado  troncoquemado
* Arbol sin hojas  troncomuerto
* tronco basecortada

2. **Hojas  hojaarce**: (contiene una animacion basada en puros cos y sen directametne en c++ no en blender)

3. **Plano** (bloque de tierra)
(usamo chunks que se generan de manera procedural)
3. **Montaña** (escalado desde blender)
4. **Pidra**
5. **Pasto**
6. **Sol**
7. **Luna**
8. **Nubes**

## Uso Skybox
Para la skybox:
mycube son dos archivos para la skybox, que es la skybox de dia y la de noche

## Interfaz Gráfica (UI)
Para el IU:
models/image estan los imagenes que ocupamos en la pantalla 
**Interaccion con el mundo:**

*  `A,W,S,D` : es para **moverse**
* P y viendo hacia una parte cercada del suelo: se planta un arbol:
* F: Se inicia el incendio dura alrededor de 90 segundos, pueden cambiarlo desde las variables del codigo:
* G: Se apaga el incendio 
* espacio: volar
* ctrl:descender
* Mecanica de talar arboles

### PICKING (Interacición Objetos)
Para esta mecanica se ocupo la cruz que tenemos en el centro de pantalla y es necesario hacer in clic izquierdo sobre el tronco para talar el arbol, pasara de un arbol con hojas a un arbol sin hojas y despues a una base de arbol cortada si se clikea en ese orden


### Iluminacion

Para mover el sol y la luna se mueve con las teclas de arriba, abajo, izqueirdaa y derecha