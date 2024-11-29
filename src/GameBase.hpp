#include "psyqo/scene.hh"
#include "third_party/nugget/psyqo/application.hh"
#include "third_party/nugget/psyqo/font.hh"

class GameBase final : public psyqo::Application {
private:
    psyqo::Font<> m_Font;

    psyqo::Scene* m_currentScene;
public:
    void prepare() override;
    void createScene() override;

  public:
    
};