#!/bin/bash

# Permitir conexiones gráficas
xhost +local:docker

# Definir la imagen
IMAGE_NAME="nhcar/arduinobot:v1"

# Obtener la ruta absoluta de la carpeta donde ESTÁ el script
# Esto evita que el volumen falle si lanzas el script desde otra carpeta
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

sudo docker run -e DISPLAY=$DISPLAY \
           -e USER=$USER \
           -v /tmp/.X11-unix/:/tmp/.X11-unix/ \
           -v "$DIR":/home/sistemas/arduinobot_ws/ \
           --device /dev/dri:/dev/dri \
           -it \
           --rm \
           --network=host \
           --name ros2_jazzy \
           --hostname $USER \
           $IMAGE_NAME
