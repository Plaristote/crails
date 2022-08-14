#include "index.hpp"
#include "builder.hpp"
#include "formats.hpp"

using namespace std;

TemplatesIndex::TemplatesIndex()
{
  add_command("build",   []() { return make_shared<TemplateBuilder>(); });
  add_command("formats", []() { return make_shared<TemplateFormatsManager>(); });
}
