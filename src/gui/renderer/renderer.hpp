#pragma once

class Renderer
{
public:
  ~Renderer() = default;
  Renderer(const Renderer &) = delete;
  Renderer(Renderer &&) = delete;
  Renderer &operator=(const Renderer &) = delete;
  Renderer &operator=(Renderer &&) = delete;

  static bool init();
  static void destroy();
  static void thread();

  static bool getIsOpen();
  static bool getIsFocused();

private:
  Renderer() {};

  static Renderer &getInstance()
  {
    static Renderer i{};
    return i;
  }

  bool initImpl();
  void threadImpl();
  void destroyImpl();

  void render();
  bool handleState();
  bool handleWindowOrder();

private:
  bool isRunning = true;
  bool isOpen = false;

  bool isFocused = false;
};