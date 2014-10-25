### DynStruct
DynStruct is an object that serializes/unserializes data in text mode. Any node from the root of the DynStruct can
contain a value or a set of other nodes. Unexisting nodes are created on the fly and garbage collected when they go
out of scope. This means you can do stuff like this:

```C++
  DynStruct render_data;

  if ((render_data["whatever"]["something"]["something_else"].Nil()))
    std::cout << "This node does not exist" << std::endl;
```

If you set a value on an unexisting node, it will be saved along with all the unexisting parent nodes. This means that
this is a correct use of the DynStruct:

```C++
  render_data["nonexisting-node"]["a key"] = "A value";
```

They also support any type that is suppoted by std streams. So this s correct as well:

```C++
  render_data["key"] = 42.f;
```
It also automatically cast to the expected type:

```C++
  unsigned int number = render_data["key"];
```

Use the "value" method to explicitely get the value as a `std::string`:

```C++
  std::string mystring = render_data["key"].Value();
```
