#include "psyqo/font.hh"
#include "third_party/nugget/psyqo/gpu.hh"
#include "third_party/nugget/psyqo/scene.hh"

namespace mi {
    namespace Scenes {
        class Geidontei final : public psyqo::Scene {
        private:
            // We'll have some simple animation going on, so we
            // need to keep track of our state here.
            uint8_t _anim = 0;
            bool _direction = true;

            psyqo::Font<> _font;
        public:
            Geidontei(psyqo::Font<> textFont);

            void frame() override;
        };
    }
}