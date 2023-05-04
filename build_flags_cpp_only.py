Import("env")

env.Append(CXXFLAGS=["-felide-constructors", "-std=c++0x"])
