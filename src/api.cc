#include "rockre.h"
#include "rockre_private.h"
#include <stdlib.h>
#include <string.h>

struct rockre_regexp {
  RockRE::Irep irep;
};

struct rockre {
  char* errstr;
  std::vector<rockre_regexp> regexp_objects;
  std::vector<rockre_region> region_objects;
};

rockre* rockre_new()
{
  return new rockre();
}

void rockre_destroy(rockre* r)
{
  if (r->errstr) {
    free(r->errstr);
  }
  delete r;
}

const char* rockre_errstr(rockre* r)
{
  return r->errstr;
}

rockre_regexp* rockre_compile(rockre* r, const char*regexp, size_t regexp_len)
{
  RockRE::Node node;
  std::string errstr;
  if (!RockRE::parse(std::string(regexp, regexp_len), node, errstr)) {
    r->errstr = strdup(errstr.c_str());
  }
  r->regexp_objects.resize(r->regexp_objects.size() + 1);

  rockre_regexp* rrr = &(r->regexp_objects.back());
  RockRE::codegen(node, rrr->irep);
  return rrr;
}

rockre_region* rockre_region_new(rockre* r)
{
  r->region_objects.resize(r->region_objects.size() + 1);
  return &(r->region_objects.back());
}

void rockre_region_destroy(rockre* r, rockre_region* rrr)
{
  (void) r;
  if (rrr->beg) {
    delete rrr->beg;
  }
  if (rrr->end) {
    delete rrr->end;
  }
  return;
}

bool rockre_partial_match(rockre* r, rockre_regexp* rr, rockre_region* rrr, const char* str, size_t str_len)
{
  (void)r;
  std::vector< std::pair<int,int> > captured;
  bool ret = RockRE::partial_match(std::string(str, str_len), rr->irep, captured);
  rrr->num_regs = captured.size();
  rrr->beg = new int[rrr->num_regs];
  rrr->end = new int[rrr->num_regs];
  for (int i=0; i<rrr->num_regs; ++i) {
    rrr->beg[i] = captured[i].first;
    rrr->end[i] = captured[i].second;
  }
  return ret;
}

bool rockre_full_match(rockre* r, rockre_regexp* rr, rockre_region* rrr, const char* str, size_t str_len)
{
  (void)r;
  std::vector< std::pair<int,int> > captured;
  bool ret = RockRE::full_match(std::string(str, str_len), rr->irep, captured);
  rrr->num_regs = captured.size();
  return ret;
}

bool rockre_dump_node(rockre* r, const char* regexp, size_t regexp_len)
{
  RockRE::Node node;
  std::string errstr;
  if (!RockRE::parse(std::string(regexp, regexp_len), node, errstr)) {
    r->errstr = strdup(errstr.c_str());
    return false;
  }
  node.dump();
  return true;
}

