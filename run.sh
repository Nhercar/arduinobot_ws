xhost +local:docker
sudo docker run -e DISPLAY=$DISPLAY \
           -e USER=$USER \
           -v /tmp/.X11-unix/:/tmp/.X11-unix/ \
           -v ./arduinobot_ws:/home/$USER/arduinobot_ws/ \
           --device /dev/dri:/dev/dri \
           -it \
           --rm \
           --network=host \
           --name ros2_jazzy \
           --hostname $USER \
           nhcar/arduinobot:v1

