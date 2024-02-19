#ifndef SOLADO_GAME_H
#define SOLADO_GAME_H

#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include "entity.h"
#include "entity-manager.h"

struct WindowConfig { int W, H, FPS, VSYNC; };
struct FontConfig { std::string F; int S, R, G, B; };
struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game
{
public:
  Game(const std::string& path);

  void run();

private:
  void init(const std::string& config);

  // systems
  void sEnemySpawner();
  void sMovement();
  void sLifespan();
  void sCollision();
  void sUserInput();
  void sCooldown();
  void sGUI();
  void sRender();

  // actions
  void shoot(int x, int y);
  void specialAction();

  // utils
  void spawnPlayer();
  void spawnMiniBullet(float angle);
  void spawnEnemy();
  void spawnSmallEnemy(
    const std::shared_ptr<solado::ecs::CTransform>& transform,
    const std::shared_ptr<solado::ecs::CShape>& shape,
    float angle
  );
  void setPaused(bool paused);

  // config parsers
  void parseWindowConfig(std::ifstream& fin);
  void parseFontConfig(std::ifstream& fin);
  void parsePlayerConfig(std::ifstream& fin);
  void parseEnemyConfig(std::ifstream& fin);
  void parseBulletConfig(std::ifstream& fin);

  // fields
  sf::RenderWindow  m_window;
  sf::Font          m_font;
  sf::Text          m_text;
  sf::Clock         m_deltaClock;

  solado::ecs::EntityManager            m_entityManager;
  std::shared_ptr<solado::ecs::Entity>  m_player;

  WindowConfig  m_windowConfig;
  FontConfig    m_fontConfig;
  PlayerConfig  m_playerConfig;
  EnemyConfig   m_enemyConfig;
  BulletConfig  m_bulletConfig;

  int   m_score = 0;
  int   m_currentFrame = 0;
  int   m_lastEnemySpawnTime = 0;
  int   m_enemySpawnerInterval = 0;
  bool  m_paused = false;
  bool  m_running = true;

  // GUI
  bool  m_isMovementActive = true;
  bool  m_isLifespanActive = true;
  bool  m_isCollisionActive = true;
  bool  m_isSpawningActive = true;
  bool  m_isRenderingActive = true;
};

#endif
