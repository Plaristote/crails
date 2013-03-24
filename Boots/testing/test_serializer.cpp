#include <Utils/test.hpp>
#include <Utils/serializer.hpp>

using namespace std;

struct MySerializable : public Utils::Serializable
{
  void Serialize(Utils::Packet& packet) const
  {
    packet << x << y;
  }

  void Unserialize(Utils::Packet& packet)
  {
    packet >> x >> y;
  }

  int   x;
  float y;
};

struct UnsuportedType
{
  int x;
};

void TestsSerializer(UnitTest& tester)
{
  tester.AddTest("Serializer", "Values", []() -> string
  {
    Utils::Packet in;
    unsigned int   a = 42;
    unsigned short b = 21;
    int            c = 24;
    short          d = 12;
    float          e = 4.2f;

    in << a << b << c << d << e;
    
    Utils::Packet out(in.raw(), in.size());

    out >> a >> b >> c >> d >> e;

    if (a == 42 && b == 21 && c == 24 && d == 12 && e == 4.2f)
      return ("");
    return ("Serializer miserably failed the test");
  });

  tester.AddTest("Serializer", "Strings", []() -> string
  {
    Utils::Packet in;
    string  totest("Trololo");

    in << totest;

    Utils::Packet out(in.raw(), in.size());

    out >> totest;

    if (totest == "Trololo")
      return ("");
    return ("Serializer didn't pass the test");
  });

  tester.AddTest("Serializer", "Arrays", []() -> string
  {
    Utils::Packet    in;
    std::list<float> floats;

    floats.push_back(12);
    floats.push_back(15);
    floats.push_back(18);
    in << floats;

    Utils::Packet      out(in.raw(), in.size());
    std::vector<float> floats2;

    out >> floats2;
    if (floats2[0] == 12 && floats2[1] == 15 && floats2[2] == 18 && floats2.size() == 3)
      return ("");
    return ("Something was wrong with the array");
  });

  tester.AddTest("Serializer", "Serializable objects", []() -> string
  {
    Utils::Packet  in;
    MySerializable obj;

    obj.x = 42; obj.y = 24.f;
    in << obj;

    Utils::Packet out(in.raw(), in.size());
    MySerializable obj2;

    out >> obj2;
    return (obj2.x == 42 && obj2.y == 24.f ? "" : "Something went wrong");
  });

  tester.AddTest("Serializer", "Must dereference pointers", []() -> string
  {
    int  i  = 43;
    int* ii = &i;
    Utils::Packet in;

    in << ii;

    Utils::Packet out(in.raw(), in.size());

    int iii;

    out >> iii;
    return (i == iii ? "" : "Did not work");
  });

  tester.AddTest("Serializer", "Must throw an exception if an unsupported type is passed", []() -> string
  {
    Utils::Packet in;
    UnsuportedType obj;

    try
    {
      in << obj;
    }
    catch (Exception<Utils::Packet> e)
    {
      return ("");
    }
    return ("Didn't throw an exception");
  });
}

