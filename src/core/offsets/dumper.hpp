#pragma once

class Dumper
{
public:
  static bool init();

private:
  Dumper() = default;

  static Dumper &getInstance()
  {
    static Dumper instance;
    return instance;
  }

  bool initImpl();
  bool runDumper();
  bool findCHud();
  bool findVEngineCVar();
  bool loadOffsets();
};