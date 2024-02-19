#include "game.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <random>

#include <SFML/Window/Event.hpp>

#include "vector2.h"

Game::Game(const std::string& config)
{
  this->init(config);
}

void Game::init(const std::string& path)
{
  // TODO: read in config fize here
  // use the premade PlayerConfig, EnemyConfig, BulletConfig variables
  std::ifstream fin(path);

  parseWindowConfig(fin);
  parseFontConfig(fin);
  parsePlayerConfig(fin);
  parseEnemyConfig(fin);
  parseBulletConfig(fin);

  // set up default window parameters
  m_window.create(sf::VideoMode(m_windowConfig.W, m_windowConfig.H), "solado v0.1.0");
  m_window.setFramerateLimit(m_windowConfig.FPS);

  if (!m_font.loadFromFile(m_fontConfig.F)) {
    std::cerr << "Failed to load font" << std::endl;
    exit (1);
  }
  m_text.setFont(m_font);
  m_text.setCharacterSize(m_fontConfig.S);
  m_text.setFillColor(sf::Color(m_fontConfig.R, m_fontConfig.G, m_fontConfig.B));
  m_text.setStyle(sf::Text::Bold);
  m_text.setPosition(10, 10);

  m_running = ImGui::SFML::Init(m_window);
  m_enemySpawnerInterval = m_enemyConfig.SI;

  spawnPlayer();
}

void Game::parseWindowConfig(std::ifstream& fin)
{
  fin >> m_windowConfig.W >> m_windowConfig.H >> m_windowConfig.FPS >> m_windowConfig.VSYNC;
}

void Game::parseFontConfig(std::ifstream& fin)
{
  fin >> m_fontConfig.F >> m_fontConfig.S >> m_fontConfig.R >> m_fontConfig.G >> m_fontConfig.B;
}

void Game::parsePlayerConfig(std::ifstream& fin)
{
  // shape radius -- collision radius -- speed
  fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S;
  // fill red -- fill green -- fill blue
  fin >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB;
  // outline red -- outline green -- outline blue -- outline thickness
  fin >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT;
  // shape vertices
  fin >> m_playerConfig.V;
}

void Game::parseEnemyConfig(std::ifstream& fin)
{
  // shape radius -- collision radius
  fin >> m_enemyConfig.SR >> m_enemyConfig.CR;
  // min speed -- max speed
  fin >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX;
  // outline red -- outline green -- outline blue
  fin >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB;
  // outline thickness
  fin >> m_enemyConfig.OT;
  // min vertices -- max vertices
  fin >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX;
  // small life span
  fin >> m_enemyConfig.L;
  // spawn interval
  fin >> m_enemyConfig.SI;
}

void Game::parseBulletConfig(std::ifstream& fin)
{
  // shape radius -- collision radius
  fin >> m_bulletConfig.SR >> m_bulletConfig.CR;
  // speed
  fin >> m_bulletConfig.S;
  // fill color
  fin >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB;
  // outline color
  fin >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB;
  // outline thickness
  fin >> m_bulletConfig.OT;
  // shape vertices
  fin >> m_bulletConfig.V;
  // lifespan
  fin >> m_bulletConfig.L;
}

void Game::run()
{
  while (m_running)
  {
    // update entity manager
    m_entityManager.update();

    // required call to imgui
    ImGui::SFML::Update(m_window, m_deltaClock.restart());

    sEnemySpawner();
    sMovement();
    sLifespan();
    sCollision();
    sCooldown();
    sUserInput();
    sGUI();
    sRender();

    ++m_currentFrame;
  }

  m_window.close();

  ImGui::SFML::Shutdown();
}

void Game::setPaused(bool paused)
{
  m_paused = paused;
}

void Game::spawnPlayer()
{
  auto entity = m_entityManager.addEntity("player");
  float x = (m_windowConfig.W - m_playerConfig.SR) / 2.f;
  float y = (m_windowConfig.H - m_playerConfig.SR) / 2.f;

  entity->cTransform = std::make_shared<solado::ecs::CTransform>(
    solado::math::Vector2(x, y),
    solado::math::Vector2(.0f, .0f),
    .0f,
    m_playerConfig.S
  );
  entity->cShape = std::make_shared<solado::ecs::CShape>(
    m_playerConfig.SR,
    m_playerConfig.V,
    sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
    sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
    m_playerConfig.OT
  );
  entity->cInput = std::make_shared<solado::ecs::CInput>();
  entity->cCollision = std::make_shared<solado::ecs::CCollision>(
    m_playerConfig.CR
  );
  entity->cScore = std::make_shared<solado::ecs::CScore>(0);
  entity->cCooldown = std::make_shared<solado::ecs::CCooldown>(240);

  m_player = entity;
}

void Game::shoot(int x, int y)
{
  auto pPos = m_player->cTransform->position;
  auto bullet = m_entityManager.addEntity("bullet");
  auto velocity = (solado::math::Vector2(x, y) - pPos).normalize();

  bullet->cTransform = std::make_shared<solado::ecs::CTransform>(
    pPos,
    velocity,
    0.f,
    m_bulletConfig.S
  );
  bullet->cShape = std::make_shared<solado::ecs::CShape>(
    m_bulletConfig.SR,
    m_bulletConfig.V,
    sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
    sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB),
    m_bulletConfig.OT
  );
  bullet->cCollision = std::make_shared<solado::ecs::CCollision>(
    m_bulletConfig.CR
  );
  bullet->cLifespan = std::make_shared<solado::ecs::CLifespan>(
    m_bulletConfig.L
  );
}

void Game::spawnMiniBullet(float angle)
{
  float radians = angle * M_PI / 180.f;
  float cos = std::cos(radians);
  float sin = std::sin(radians);

  auto bullet = m_entityManager.addEntity("bullet");
  bullet->cTransform = std::make_shared<solado::ecs::CTransform>(
    m_player->cTransform->position,
    solado::math::Vector2(cos, sin),
    0.f,
    m_bulletConfig.S / 2.f
  );
  bullet->cShape = std::make_shared<solado::ecs::CShape>(
    m_bulletConfig.SR / 3.f,
    m_bulletConfig.V,
    sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
    sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
    m_playerConfig.OT
  );
  bullet->cCollision = std::make_shared<solado::ecs::CCollision>(
    m_bulletConfig.CR / 3.f
  );
  bullet->cLifespan = std::make_shared<solado::ecs::CLifespan>(
    m_bulletConfig.L / 4.f
  );
}

void Game::specialAction()
{
  auto& cd = m_player->cCooldown;

  if (cd->remaining > 0) return;

  int bullets = 18;
  int angle = 360 / bullets;

  for (int i = 0; i < bullets; ++i)
  {
    spawnMiniBullet(angle * (i + 1));
  }

  cd->remaining = cd->total;
}

int random(int min, int max)
{
  std::random_device rd;  // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd());
  std::uniform_int_distribution distrib(min, max);

  return distrib(gen);
}

void Game::spawnEnemy()
{
  // randomize enemy data
  int x = random(m_enemyConfig.CR + m_enemyConfig.OT, m_windowConfig.W - m_enemyConfig.CR - m_enemyConfig.OT);
  int y = random(m_enemyConfig.CR + m_enemyConfig.OT, m_windowConfig.H - m_enemyConfig.CR - m_enemyConfig.OT);
  int speed = random(m_enemyConfig.SMIN, m_enemyConfig.SMAX);
  int vertices = random(m_enemyConfig.VMIN, m_enemyConfig.VMAX);
  int fillR = random(0, 255);
  int fillG = random(0, 255);
  int fillB = random(0, 255);

  auto enemy = m_entityManager.addEntity("enemy");
  enemy->cTransform = std::make_shared<solado::ecs::CTransform>(
    solado::math::Vector2(x, y),
    solado::math::Vector2(fillR / 255.f, fillG / 255.f),
    0,
    speed
  );
  enemy->cShape = std::make_shared<solado::ecs::CShape>(
    m_enemyConfig.SR,
    vertices,
    sf::Color(fillR, fillG, fillB),
    sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB),
    m_enemyConfig.OT
  );
  enemy->cCollision = std::make_shared<solado::ecs::CCollision>(m_enemyConfig.CR);
  enemy->cScore = std::make_shared<solado::ecs::CScore>(vertices * 100);
  enemy->cEnemyType = std::make_shared<solado::ecs::CEnemyType>(solado::ecs::EnemyType::big);
}

void Game::spawnSmallEnemy(const std::shared_ptr<solado::ecs::CTransform>& transform, const std::shared_ptr<solado::ecs::CShape>& shape, float angle)
{
  int vertices = shape->circle.getPointCount();
  float radians = angle * M_PI / 180.f;
  float cos = std::cos(radians);
  float sin = std::sin(radians);

  auto enemy = m_entityManager.addEntity("enemy");
  enemy->cTransform = std::make_shared<solado::ecs::CTransform>(
    transform->position,
    solado::math::Vector2(cos, sin),
    0,
    transform->speed
  );
  enemy->cShape = std::make_shared<solado::ecs::CShape>(
    m_enemyConfig.SR / 2,
    vertices,
    shape->circle.getFillColor(),
    sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB),
    m_enemyConfig.OT
  );
  enemy->cCollision = std::make_shared<solado::ecs::CCollision>(m_enemyConfig.CR / 2);
  enemy->cScore = std::make_shared<solado::ecs::CScore>(vertices * 200);
  enemy->cLifespan = std::make_shared<solado::ecs::CLifespan>(90);
  enemy->cEnemyType = std::make_shared<solado::ecs::CEnemyType>(solado::ecs::EnemyType::small);
}

void Game::sEnemySpawner()
{
  if (m_paused || !m_isSpawningActive || m_enemySpawnerInterval > (m_currentFrame - m_lastEnemySpawnTime)) return;

  spawnEnemy();
  m_lastEnemySpawnTime = m_currentFrame;
}

void Game::sMovement()
{
  if (!m_isMovementActive || m_paused) return;

  // update player velocity based on its input component
  auto& options = m_player->cInput->options;
  if (options[0])
  {
    m_player->cTransform->velocity.x = 0.f;
    m_player->cTransform->velocity.y = -1.f;
  }
  else if (options[1])
  {
    m_player->cTransform->velocity.x = 1.f;
    m_player->cTransform->velocity.y = 0.f;
  }
  else if (options[2])
  {
    m_player->cTransform->velocity.x = 0.f;
    m_player->cTransform->velocity.y = 1.f;
  }
  else if (options[3])
  {
    m_player->cTransform->velocity.x = -1.f;
    m_player->cTransform->velocity.y = 0.f;
  }
  else
  {
    m_player->cTransform->velocity.x = 0.f;
    m_player->cTransform->velocity.y = 0.f;
  }

  // entities movement
  for (auto& e : m_entityManager.getEntities())
  {
    e->cTransform->position.x += e->cTransform->velocity.x * e->cTransform->speed;
    e->cTransform->position.y += e->cTransform->velocity.y * e->cTransform->speed;
  }
}

void Game::sLifespan()
{
  if (!m_isLifespanActive || m_paused) return;

  for (auto& e : m_entityManager.getEntities())
  {
    // if entity has no lifespan component, skip it
    if (!e->isAlive() || !e->cLifespan) continue;

    if (e->cLifespan->remaining > 0)
    {
      // if entity has > 0 remaining lifespan, subtract 1
      // if it has lifespan and is alive, scale its alpha channel properly
      e->cLifespan->remaining -= 1;

      sf::Color fill = e->cShape->circle.getFillColor();
      sf::Color outline = e->cShape->circle.getOutlineColor();
      int alpha = 255 * (static_cast<float>(e->cLifespan->remaining) / e->cLifespan->total);

      e->cShape->circle.setFillColor(sf::Color(fill.r, fill.g, fill.b, alpha));
      e->cShape->circle.setOutlineColor(sf::Color(outline.r, outline.g, outline.b, alpha));
    }
    else
    {
      // if it has lifespan and its time is up destroy the entity
      e->destroy();
    }
  }
}

void Game::sCollision()
{
  if (!m_isCollisionActive) return;

  // player data
  auto& pPos = m_player->cTransform->position;
  auto pSize = m_player->cShape->circle.getRadius() + m_player->cShape->circle.getOutlineThickness();

  for (auto& b : m_entityManager.getEntitiesByTag("bullet"))
  {
    for (auto& e : m_entityManager.getEntitiesByTag("enemy"))
    {
      auto& bPos = b->cTransform->position;
      auto bRadius = b->cShape->circle.getRadius() + b->cShape->circle.getOutlineThickness();

      auto& ePos = e->cTransform->position;
      auto eRadius = e->cShape->circle.getRadius() + e->cShape->circle.getOutlineThickness();

      auto dist = ePos - bPos;
      auto distSqr = (dist.x * dist.x) + (dist.y * dist.y);

      if (distSqr < ((bRadius + eRadius) * (bRadius + eRadius)))
      {
        m_player->cScore->score += e->cScore->score;

        b->destroy();
        e->destroy();

        if (e->cEnemyType->type == solado::ecs::EnemyType::small) continue;

        int smallEnemiesToSpawn = e->cShape->circle.getPointCount();
        float angle = 360 / smallEnemiesToSpawn;
        for (int i = 0; i < smallEnemiesToSpawn; ++i)
        {
          spawnSmallEnemy(e->cTransform, e->cShape, angle * (i + 1));
        }
      }
    }
  }

  for (auto& e : m_entityManager.getEntitiesByTag("enemy"))
  {
    if (!e->isAlive()) continue;

    auto& pos = e->cTransform->position;
    auto& vel = e->cTransform->velocity;
    auto size = e->cShape->circle.getRadius() + e->cShape->circle.getOutlineThickness();

    if (pos.x - size <= 0)
    {
      pos.x = size;
      vel.x *= -1;
    }
    else if (pos.x + size >= m_windowConfig.W)
    {
      pos.x = m_windowConfig.W - size;
      vel.x *= -1;
    }

    if (pos.y - size <= 0)
    {
      pos.y = size;
      vel.y *= -1;
    }
    else if (pos.y + size >= m_windowConfig.H)
    {
      pos.y = m_windowConfig.H - size;
      vel.y *= -1;
    }

    auto ePos = e->cTransform->position;
    auto eSize = e->cShape->circle.getRadius() + e->cShape->circle.getOutlineThickness();
    auto dist = ePos - pPos;
    auto distSqr = (dist.x * dist.x) + (dist.y * dist.y);

    if (distSqr < ((eSize + pSize) * (eSize + pSize)))
    {
      e->destroy();
      m_player->destroy();

      spawnPlayer();
    }
  }

  // check player collisions
  if (pPos.x - pSize <= 0)
  {
    pPos.x = pSize;
  }
  else if (pPos.x + pSize >= m_windowConfig.W)
  {
    pPos.x = m_windowConfig.W - pSize;
  }

  if (pPos.y - pSize <= 0)
  {
    pPos.y = pSize;
  }
  else if (pPos.y + pSize >= m_windowConfig.H)
  {
    pPos.y = m_windowConfig.H - pSize;
  }
  // end check player collisions
}

void Game::sGUI()
{
  ImGui::Begin("Systems");
  {
    ImGui::Checkbox("Movement", &m_isMovementActive);
    ImGui::Checkbox("Lifespan", &m_isLifespanActive);
    ImGui::Checkbox("Collision", &m_isCollisionActive);
    ImGui::Checkbox("Rendering", &m_isRenderingActive);
    ImGui::Checkbox("Spawning", &m_isSpawningActive);

    if (ImGui::Button("Spawn enemy"))
    {
      spawnEnemy();
    }
    ImGui::SameLine();
    ImGui::SliderInt("Spawning Interval", &m_enemySpawnerInterval, 15, 120);

    if (ImGui::CollapsingHeader("Player Stats"))
    {
      ImGui::Text("cd: %.2f", m_player->cCooldown->remaining);
    }
  }
  ImGui::End();

  ImGui::Begin("Entity Manager");
  if (ImGui::CollapsingHeader("Entities"))
  {
    for (auto& e : m_entityManager.getEntities())
    {
      
      sf::Color color = e->cShape->circle.getFillColor();
      ImGui::PushID(e->id());
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f));
      if (ImGui::Button("D"))
      {
        e->destroy();
      }
      ImGui::PopStyleColor();
      ImGui::SameLine();
      ImGui::Text("%zu", e->id());
      ImGui::SameLine();
      ImGui::Text("%s", e->tag().c_str());
      ImGui::SameLine();
      ImGui::Text(
        "(%i, %i)",
        static_cast<int>(e->cTransform->position.x),
        static_cast<int>(e->cTransform->position.y)
      );

      ImGui::PopID();
    }
  }

  if (ImGui::CollapsingHeader("Enemies"))
  {
    for (auto& e : m_entityManager.getEntitiesByTag("enemy"))
    {
      sf::Color fColor = e->cShape->circle.getFillColor();
      sf::Color oColor = e->cShape->circle.getOutlineColor();
      ImGui::PushID(e->id());
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(fColor.r / 255.f, fColor.g / 255.f, fColor.b / 255.f, fColor.a / 255.f));
      ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(oColor.r / 255.f, oColor.g / 255.f, oColor.b / 255.f, 1.f));
      if (ImGui::Button("D"))
      {
        e->destroy();
      }
      ImGui::PopStyleColor(2);
      ImGui::PopID();
      ImGui::SameLine();
      ImGui::Text("%zu", e->id());
      ImGui::SameLine();
      ImGui::Text(
        "(%i, %i)",
        static_cast<int>(e->cTransform->position.x),
        static_cast<int>(e->cTransform->position.y)
      );
    }
  }

  if (ImGui::CollapsingHeader("Bullets"))
  {
    for (auto& e : m_entityManager.getEntitiesByTag("bullet"))
    {
      sf::Color fColor = e->cShape->circle.getFillColor();
      sf::Color oColor = e->cShape->circle.getOutlineColor();
      ImGui::PushID(e->id());
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(fColor.r / 255.f, fColor.g / 255.f, fColor.b / 255.f, fColor.a / 255.f));
      ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(oColor.r / 255.f, oColor.g / 255.f, oColor.b / 255.f, 1.f));
      if (ImGui::Button("D"))
      {
        e->destroy();
      }
      ImGui::PopStyleColor(2);
      ImGui::PopID();
      ImGui::SameLine();
      ImGui::Text("%zu", e->id());
      ImGui::SameLine();
      ImGui::Text(
        "(%i, %i)",
        static_cast<int>(e->cTransform->position.x),
        static_cast<int>(e->cTransform->position.y)
      );

    }
  }
  ImGui::End();
}

void Game::sRender()
{
  if (!m_isRenderingActive) return;

  m_window.clear();

  for (auto& e : m_entityManager.getEntities())
  {
    e->cShape->circle.setPosition(e->cTransform->position.x, e->cTransform->position.y);
    e->cShape->circle.rotate(3.0f);

    m_window.draw(e->cShape->circle);
  }

  // draw score
  m_text.setString("Score: " + std::to_string(m_player->cScore->score));
  m_window.draw(m_text);

  // draw the ui last
  ImGui::SFML::Render(m_window);

  m_window.display();
}

void Game::sCooldown()
{
  if (m_paused) return;

  auto& cd = m_player->cCooldown;
  if (cd->remaining > 0 && cd->remaining <= cd->total)
  {
    cd->remaining -= 1;
  } 
}

void Game::sUserInput()
{
  sf::Event event;
  while (m_window.pollEvent(event))
  {
    ImGui::SFML::ProcessEvent(m_window, event);

    if (event.type == sf::Event::Closed)
    {
      m_running = false;
    }

    if (event.type == sf::Event::KeyPressed)
    {
      if (ImGui::GetIO().WantCaptureKeyboard) { continue; }

      switch (event.key.code)
      {
      case sf::Keyboard::W:
        {

          m_player->cInput->up = true;
          m_player->cInput->options.set(0);
          break;
        }

      case sf::Keyboard::D:
        {

          m_player->cInput->right = true;
          m_player->cInput->options.set(1);
          break;
        }

      case sf::Keyboard::S:
        {

          m_player->cInput->down = true;
          m_player->cInput->options.set(2);
          break;
        }

      case sf::Keyboard::A:
        {

          m_player->cInput->left = true;
          m_player->cInput->options.set(3);
          break;
        }

      default:
        break;
      }
    }

    if (event.type == sf::Event::KeyReleased)
    {
      if (ImGui::GetIO().WantCaptureKeyboard) { continue; }

      switch (event.key.code)
      {
      case sf::Keyboard::W:
        {
          m_player->cInput->up = false;
          m_player->cInput->options.reset(0);
          break;
        }

      case sf::Keyboard::D:
        {
          m_player->cInput->right = false;
          m_player->cInput->options.reset(1);
          break;
        }

      case sf::Keyboard::S:
        {
          m_player->cInput->down = false;
          m_player->cInput->options.reset(2);
          break;
        }

      case sf::Keyboard::A:
        {
          m_player->cInput->left = false;
          m_player->cInput->options.reset(3);
          break;
        }

      case sf::Keyboard::P:
        {
          m_paused = true;
          break;
        }

      case sf::Keyboard::Escape:
        {
          m_running = false;
          break;
        }

      default:
        break;
      }
    }

    if (event.type == sf::Event::MouseButtonReleased)
    {
      if (ImGui::GetIO().WantCaptureMouse) { continue; }
      switch (event.mouseButton.button)
      {
      case sf::Mouse::Left:
        {
          shoot(event.mouseButton.x, event.mouseButton.y);
          break;
        }

      case sf::Mouse::Right:
        {
          specialAction();
          break;
        }

      default:
        break;
      }
    }
  }
}
