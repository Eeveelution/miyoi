TARGET = miyoi
TYPE = ps-exe

CPPFLAGS += -I.

SRCS = src/main.cpp src/GameBase.cpp src/scenes/Geidontei.cpp src/gpu/Rendering.cpp src/resources/TimFile.cpp src/game/Bullet.cpp src/game/Enemy.cpp src/game/BulletList.cpp

include third_party/nugget/psyqo/psyqo.mk