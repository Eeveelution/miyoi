TARGET = miyoi
TYPE = ps-exe

CPPFLAGS += -I./ -O2

SRCS = src/main.cpp src/GameBase.cpp src/scenes/Geidontei.cpp src/gpu/Rendering.cpp src/resources/TimFile.cpp src/game/Bullet.cpp src/game/Enemy.cpp

include third_party/nugget/psyqo/psyqo.mk
