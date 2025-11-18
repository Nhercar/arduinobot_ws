import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/mnt/c/Users/nacho/OneDrive/Escritorio/Uni/Trabajo/TutorialROS2/arduinobot_ws/install/arduinobot_py_examples'
