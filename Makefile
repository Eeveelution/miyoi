TARGET = miyoi
TYPE = ps-exe

CPPFLAGS += -I./ -O0

SRCS = src/main.cpp src/GameBase.cpp src/scenes/Geidontei.cpp src/gpu/Rendering.cpp src/resources/TimFile.cpp src/game/Bullet.cpp

include third_party/nugget/psyqo/psyqo.mk
